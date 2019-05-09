/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Dialogue code compiler.
 *
 * ---------------------------------------------------------------------------|
 */

#if LITHIUM
#include "common.h"
#include "w_world.h"
#include "p_player.h"
#include "dialogue.h"
#include "m_file.h"
#include "m_tokbuf.h"

#define LogTok(tok, s) \
   Log(c"(%i:%i) %s: " s " (%i:\"%s\")", tok->orig.line, tok->orig.colu, \
       __func__, tok->type, tok->textV ? tok->textV : c"<no string>")

/* Types ------------------------------------------------------------------- */

struct parse_state {
   struct tokbuf tb;
   struct dlg_def *def;
};

struct ptr2 {
   u32 l, h;
};

/* Static Functions -------------------------------------------------------- */

script static void GetStatement(struct parse_state *d);

static void PushB1(struct parse_state *d, u32 b)
{
   size_t pc = d->def->codeP++;

   if(pc + 1 > d->def->codeC * 4)
   {
      Vec_Grow(d->def->code, 1);
      d->def->codeC++;
   }

   if(b > 255) Log("%s: byte error (overflow) %u", b);

   Cps_SetC(d->def->codeV, pc, b);
}

static void PushB2(struct parse_state *d, u32 word)
{
   PushB1(d, word & 0xFF);
   PushB1(d, word >> 8);
}

static void PushLdv(struct parse_state *d, u32 action)
{
   PushB1(d, DCD_LDV_VI);
   PushB1(d, action);
}

static struct ptr2 PushLoadAdr(struct parse_state *d, u32 at, u32 set)
{
   struct ptr2 adr;

   PushB1(d, DCD_LDA_VI);
   PushB1(d, set & 0xFF);
   adr.l = d->def->codeP - 1;

   PushB1(d, DCD_STA_AI);
   PushB2(d, at);

   PushB1(d, DCD_LDA_VI);
   PushB1(d, set >> 8);
   adr.h = d->def->codeP - 1;

   PushB1(d, DCD_STA_AI);
   PushB2(d, at + 1);

   return adr;
}

static void SetB1(struct parse_state *d, u32 ptr, u32 b)
{
   if(b > 255) Log("%s: byte error (overflow) %u", b);

   Cps_SetC(d->def->codeV, ptr, b);
}

static void SetB2(struct parse_state *d, u32 ptr, u32 word)
{
   SetB1(d, ptr + 0, word & 0xFF);
   SetB1(d, ptr + 1, word >> 8);
}

static u32 PushST(struct parse_state *d, cstr s, u32 l)
{
   u32  p = d->def->stabP;
   u32 vl = Cps_Adjust(p + l) - d->def->stabC;

   Dbg_Log(log_dlg, "%s: (%3u %3u) '%s'", __func__, l, vl, s);

   Vec_Grow(d->def->stab, vl);
   d->def->stabC += vl;
   d->def->stabP += l;

   for(u32 i = 0; i < l; i++) Cps_SetC(d->def->stabV, p + i, s[i]);

   return STR_START + p;
}

static i32 CodeABS(struct parse_state *d, cstr reg)
{
   struct token *tok = d->tb.get();

   if(tok->type == tok_number) {
      i32 n = strtoi(tok->textV, nil, 0);

      if(n <= 0xFFFF) {
         if(!reg) {
            return n;
         } else if(d->tb.drop(tok_comma)) {
            tok = d->tb.get();

            if(tok->type == tok_identi && faststrcmp(tok->textV, reg) == 0) {
               return n;
            }

            d->tb.unget();
         }
      }
   }

   d->tb.unget();

   return false;
}

static i32 CodeZPG(struct parse_state *d, cstr reg)
{
   struct token *tok = d->tb.get();

   if(tok->type == tok_number) {
      i32 n = strtoi(tok->textV, nil, 0);

      if(n <= 0xFF) {
         if(!reg) {
            return n;
         } else if(d->tb.drop(tok_comma)) {
            tok = d->tb.get();

            if(tok->type == tok_identi && faststrcmp(tok->textV, reg) == 0) {
               return n;
            }

            d->tb.unget();
         }
      }
   }

   d->tb.unget();

   return -1;
}

static bool CodeAI(struct parse_state *d, u32 code)
{
   ifw(i32 n = CodeABS(d, nil), n > 0) {
      PushB1(d, code);
      PushB1(d, n & 0xFF);
      PushB1(d, n >> 8);
      return true;
   }

   return false;
}

static bool CodeAX(struct parse_state *d, u32 code)
{
   ifw(i32 n = CodeABS(d, "X"), n > 0) {
      PushB1(d, code);
      PushB1(d, n & 0xFF);
      PushB1(d, n >> 8);
      return true;
   }

   return false;
}

static bool CodeAY(struct parse_state *d, u32 code)
{
   ifw(i32 n = CodeABS(d, "Y"), n > 0) {
      PushB1(d, code);
      PushB1(d, n & 0xFF);
      PushB1(d, n >> 8);
      return true;
   }

   return false;
}

static bool CodeII(struct parse_state *d, u32 code)
{
   if(d->tb.drop(tok_pareno)) {
      struct token *tok = d->tb.get();

      if(tok->type == tok_number) {
         i32 n = strtoi(tok->textV, nil, 0);

         if(n <= 0xFFFF && d->tb.drop(tok_parenc)) {
            PushB1(d, code);
            PushB1(d, n & 0xFF);
            PushB1(d, n >> 8);
            return true;
         }
      }

      d->tb.unget();
   }

   return false;
}

static bool CodeIX(struct parse_state *d, u32 code)
{
   if(d->tb.drop(tok_pareno)) {
      struct token *tok = d->tb.get();

      if(tok->type == tok_number) {
         i32 n = strtoi(tok->textV, nil, 0);

         if(n <= 0xFF && d->tb.drop(tok_comma)) {
            tok = d->tb.get();

            if(tok->type == tok_identi &&
               faststrcmp(tok->textV, "X") == 0 &&
               d->tb.drop(tok_parenc))
            {
               PushB1(d, code);
               PushB1(d, n);
               return true;
            }

            d->tb.unget();
         }
      }

      d->tb.unget();
   }

   return false;
}

static bool CodeIY(struct parse_state *d, u32 code)
{
   if(d->tb.drop(tok_pareno)) {
      struct token *tok = d->tb.get();

      if(tok->type == tok_number) {
         i32 n = strtoi(tok->textV, nil, 0);

         if(n <= 0xFF && d->tb.drop(tok_parenc) && d->tb.drop(tok_comma)) {
            tok = d->tb.get();

            if(tok->type == tok_identi && faststrcmp(tok->textV, "Y") == 0) {
               PushB1(d, code);
               PushB1(d, n);
               return true;
            }

            d->tb.unget();
         }
      }

      d->tb.unget();
   }

   return false;
}

#define CodeNP(d, code) (PushB1(d, code), true)
#define CodeRI CodeZI

static bool CodeVI(struct parse_state *d, u32 code)
{
   if(d->tb.drop(tok_hash)) {
      struct token *tok = d->tb.get();

      if(tok->type == tok_number) {
         i32 n = strtoi(tok->textV, nil, 0);

         if(n <= 0xFF) {
            PushB1(d, code);
            PushB1(d, n);
            return true;
         }
      }

      d->tb.unget();
   }

   return false;
}

static bool CodeZI(struct parse_state *d, u32 code)
{
   ifw(i32 n = CodeZPG(d, nil), n > 0) {
      PushB1(d, code);
      PushB1(d, n);
      return true;
   }

   return false;
}

static bool CodeZX(struct parse_state *d, u32 code)
{
   ifw(i32 n = CodeZPG(d, "X"), n > 0) {
      PushB1(d, code);
      PushB1(d, n);
      return true;
   }

   return false;
}

static bool CodeZY(struct parse_state *d, u32 code)
{
   ifw(i32 n = CodeZPG(d, "Y"), n > 0) {
      PushB1(d, code);
      PushB1(d, n);
      return true;
   }

   return false;
}

static void GetCode_Asm(struct parse_state *d)
{
   register struct token *tok;

   #define DCD(n, op, ty) \
      tok = d->tb.reget(); \
      if(faststrcasecmp(tok->textV, #op) == 0) \
         if(Code##ty(d, DCD_##op##_##ty)) return;
   #include "dialogue.h"

   tok = d->tb.reget();
   LogTok(tok, "no function found with this syntax");
}

static void GetCode_Text(struct parse_state *d, struct token *tok, u32 act)
{
   PushLoadAdr(d, VAR_ADRL, PushST(d, tok->textV, tok->textC));
   PushLdv(d, act);
}

static void GetCode_Cond(struct parse_state *d)
{
   struct token *tok = d->tb.get();
   u32 ins = DCD_BNE_RI;

   if(tok->type == tok_identi)
   {
      if(faststrcmp(tok->textV, "item") == 0) {
         tok = d->tb.get();
         ins = DCD_BEQ_RI;

         PushLoadAdr(d, VAR_ADRL, PushST(d, tok->textV, tok->textC));

         PushLdv(d, ACT_LD_ITEM);
      } else if(faststrcmp(tok->textV, "class") == 0) {
         tok = d->tb.get();

         PushB1(d, DCD_LDA_AI);
         PushB2(d, VAR_PCLASS);

         PushB1(d, DCD_CMP_VI);
         #define LITH_X(shr, lng) \
            if(faststrcmp(tok->textV, #shr) == 0) {PushB1(d, shr); goto ok;}
         #include "p_player.h"
         LogTok(tok, "invalid playerclass type");
         ok:;
      } else {
         LogTok(tok, "invalid conditional");
      }
   }
   else
      LogTok(tok, "expected identifier");

   PushB1(d, ins);
   PushB1(d, 0); /* placeholder */
   u32 ptr = d->def->codeP;

   GetStatement(d);

   u32 rel = d->def->codeP - ptr;
   if(rel > 0x7F) Log("%s: bad jump (too much code)", __func__);

   SetB1(d, ptr - 1, rel);

   tok = d->tb.get();

   if(tok->type == tok_identi && faststrcmp(tok->textV, "else") == 0) {
      PushB1(d, DCD_JMP_AI);
      PushB2(d, 0);
      ptr = d->def->codeP;

      GetStatement(d);

      SetB2(d, ptr - 2, PRG_START + d->def->codeP);
   } else {
      d->tb.unget();
   }
}

static void GetCode_Option(struct parse_state *d)
{
   struct token *tok = d->tb.get();

   if(tok->type == tok_identi || tok->type == tok_string)
   {
      PushLoadAdr(d, VAR_ADRL, PushST(d, tok->textV, tok->textC));
      struct ptr2 adr = PushLoadAdr(d, VAR_RADRL, 0); /* placeholder */

      PushLdv(d, ACT_LD_OPT);

      PushB1(d, DCD_JMP_AI);
      PushB2(d, 0); /* placeholder */

      u32 ptr = d->def->codeP;
      u32 rel = PRG_START + ptr;

      SetB1(d, adr.l, rel & 0xFF);
      SetB1(d, adr.h, rel >> 8);

      GetStatement(d);

      SetB2(d, ptr - 2, PRG_START + d->def->codeP);
   }
   else
      LogTok(tok, "invalid option parameter");
}

static void GetCode_Str(struct parse_state *d, u32 adr)
{
   struct token *tok = d->tb.get();

   switch(tok->type) {
      case tok_identi:
      case tok_string:
      case tok_number:
         PushLoadAdr(d, adr, PushST(d, tok->textV, tok->textC));
         break;
      default:
         LogTok(tok, "invalid string parameter");
         break;
   }
}

static void GetCode_Script(struct parse_state *d)
{
   struct token *tok = d->tb.get();
   u32 act;

   if(tok->type == tok_string) {
      act = ACT_SCRIPT_S;
      PushLoadAdr(d, VAR_ADRL, PushST(d, tok->textV, tok->textC));
   } else if(tok->type == tok_number) {
      u32 prm = strtoi(tok->textV, nil, 0);

      act = ACT_SCRIPT_I;

      PushB1(d, DCD_LDA_VI);
      PushB1(d, prm);

      PushB1(d, DCD_STA_AI);
      PushB2(d, VAR_SCP0);
   } else {
      LogTok(tok, "invalid script name or number");
   }

   for(u32 i = 0; i < 4 && d->tb.drop(tok_comma); i++) {
      u32 prm = strtoi(d->tb.get()->textV, nil, 0);

      PushB1(d, DCD_LDA_VI);
      PushB1(d, prm);

      PushB1(d, DCD_STA_AI);
      PushB2(d, VAR_SCP1 + i);
   }

   PushLdv(d, act);
}

static void GetCode_Line(struct parse_state *d)
{
   struct token *tok = d->tb.get();

   switch(tok->type) {
      case tok_identi:
         Dbg_Log(log_dlg, "%s: %s", __func__, tok->textV);

         if(faststrcmp(tok->textV, "if") == 0) {
            GetCode_Cond(d);
         } else if(faststrcmp(tok->textV, "option") == 0) {
            GetCode_Option(d);
         } else if(faststrcmp(tok->textV, "page") == 0) {
            tok = d->tb.get();
            if(tok->type != tok_number) LogTok(tok, "invalid token for page");

            PushB1(d, DCD_JPG_VI);
            PushB1(d, strtoi(tok->textV, nil, 0));
         } else if(faststrcmp(tok->textV, "name") == 0) {
            GetCode_Str(d, VAR_NAMEL);
         } else if(faststrcmp(tok->textV, "icon") == 0) {
            GetCode_Str(d, VAR_ICONL);
         } else if(faststrcmp(tok->textV, "remote") == 0) {
            GetCode_Str(d, VAR_REMOTEL);
         } else if(faststrcmp(tok->textV, "pict") == 0) {
            GetCode_Str(d, VAR_PICTL);
         } else if(faststrcmp(tok->textV, "script") == 0) {
            GetCode_Script(d);
         } else {
            GetCode_Asm(d);
         }
         break;
      case tok_quote: GetCode_Text(d, d->tb.reget(), ACT_TEXT_ADDI); break;
      case tok_hash:  GetCode_Text(d, d->tb.get(),   ACT_TEXT_ADDL); break;
      case tok_semico:
      case tok_eof:
         break;
      default:
         LogTok(tok, "invalid token in line");
   }
}

static void GetBlock(struct parse_state *d)
{
   while(!d->tb.drop(tok_bracec) && !d->tb.drop(tok_eof))
      GetStatement(d);
}

static void GetConcatBlock(struct parse_state *d)
{
   PushB1(d, DCD_INC_AI);
   PushB2(d, VAR_CONCAT);

   while(!d->tb.drop(tok_at2) && !d->tb.drop(tok_eof)) GetStatement(d);

   PushB1(d, DCD_DEC_AI);
   PushB2(d, VAR_CONCAT);
}

script static void GetStatement(struct parse_state *d)
{
        if(d->tb.drop(tok_braceo)) GetBlock(d);
   else if(d->tb.drop(tok_at2))    GetConcatBlock(d);
   else                            GetCode_Line(d);
}

static void GetItem_Head(struct parse_state *d, i32 head)
{
   struct token *tok = d->tb.get();

   if(tok->type == tok_number) {
      i32            num  = head + strtoi(tok->textV, nil, 0);
      struct dlg_def *last = d->def;

      d->def = Salloc(struct dlg_def);
      d->def->num = num;

      if(!last) dlgdefs    = d->def;
      else      last->next = d->def;

      Dbg_Log(log_dlg, "\n---\nheading %i (%i)\n---",
         d->def->num, d->def->codeP);
   } else {
      LogTok(tok, "invalid terminal number token");
   }

   if(!d->tb.drop(tok_semico)) {
      LogTok(tok, "semicolon required after heading");
   }
}

static void SetupPage(struct parse_state *d, u32 num)
{
   if(num > countof(d->def->pages))
      Log("%s: bad page index", __func__);

   d->def->pages[num] = d->def->codeP;

   Dbg_Log(log_dlg, "--- page %u (%u)", num, d->def->codeP);
}

static void GetItem_Page(struct parse_state *d)
{
   struct token *tok = d->tb.get();

   if(tok->type == tok_number)
      SetupPage(d, strtoi(tok->textV, nil, 0));
   else
      LogTok(tok, "invalid page number token");

   GetStatement(d);

   PushLdv(d, ACT_DLG_WAIT);

   PushB1(d, DCD_BRK_NP);
}

/*
static void GetItem_NumPage(struct parse_state *d, i32 num)
{
   SetupPage(d, num);

   GetStatement(d);

   PushLdv(d, ACT_TRM_WAIT);

   PushB1(d, DCD_BRK_NP);
}
*/

script static u32 WriteCode(struct dlg_def  const *def,
                            struct dcd_info const *inf,
                            u32 i)
{
   u32 c2, c3;

   switch(inf->adrm) {
      case ADRM_AI:
         c2 = Cps_GetC(def->codeV, i++);
         c3 = Cps_GetC(def->codeV, i++);
         printf("%02X %02X\t%s $%02X%02X\n", c2, c3, inf->name, c3, c2);
         break;
      case ADRM_AX:
         c2 = Cps_GetC(def->codeV, i++);
         c3 = Cps_GetC(def->codeV, i++);
         printf("%02X %02X\t%s $%02X%02X,X\n", c2, c3, inf->name, c3, c2);
         break;
      case ADRM_AY:
         c2 = Cps_GetC(def->codeV, i++);
         c3 = Cps_GetC(def->codeV, i++);
         printf("%02X %02X\t%s $%02X%02X,Y\n", c2, c3, inf->name, c3, c2);
         break;
      case ADRM_II:
         c2 = Cps_GetC(def->codeV, i++);
         c3 = Cps_GetC(def->codeV, i++);
         printf("%02X %02X\t%s ($%02X%02X)\n", c2, c3, inf->name, c3, c2);
         break;
      case ADRM_IX:
         c2 = Cps_GetC(def->codeV, i++);
         printf("%02X\t%s ($%02X,X)\n", c2, inf->name, c2);
         break;
      case ADRM_IY:
         c2 = Cps_GetC(def->codeV, i++);
         printf("%02X\t%s ($%02X),Y\n", c2, inf->name, c2);
         break;
      case ADRM_NP:
         printf("\t%s\n", inf->name);
         break;
      case ADRM_ZI:
      case ADRM_RI:
         c2 = Cps_GetC(def->codeV, i++);
         printf("%02X\t%s $%02X\n", c2, inf->name, c2);
         break;
      case ADRM_VI:
         c2 = Cps_GetC(def->codeV, i++);
         printf("%02X\t%s #$%02X\n", c2, inf->name, c2);
         break;
      case ADRM_ZX:
         c2 = Cps_GetC(def->codeV, i++);
         printf("%02X\t%s $%02X,X\n", c2, inf->name, c2);
         break;
      case ADRM_ZY:
         c2 = Cps_GetC(def->codeV, i++);
         printf("%02X\t\t%s $%02X,Y\n", c2, inf->name, c2);
         break;
   }

   return i;
}

script static void Disassemble(struct dlg_def const *def)
{
   for(u32 i = 0; i < def->codeP;)
   {
      printf("%04X ", PRG_START + i);

      u32 c = Cps_GetC(def->codeV, i++);
      printf("%02X ", c);

      struct dcd_info const *inf = &dcdinfo[c];

      if(inf->name[0]) {
         i = WriteCode(def, inf, i);
      } else {
         printf("\t\tinvalid opcode\n");
      }
   }
}

script static void LoadFile(FILE *fp)
{
   struct parse_state d = {{.bbeg = 14, .bend = 28, .fp = fp}};

   TBufCtor(&d.tb);

   for(struct token *tok; (tok = d.tb.get())->type != tok_eof;)
   {
      if(tok->type != tok_identi) {
         LogTok(tok, "invalid toplevel token");
         break;
      }

      if(faststrcmp(tok->textV, "program") == 0) {
         GetItem_Head(&d, 0);
      } else if(faststrcmp(tok->textV, "dialogue") == 0) {
         GetItem_Head(&d, DPAGE_DIALOGUE);
      } else if(faststrcmp(tok->textV, "terminal") == 0) {
         GetItem_Head(&d, DPAGE_TERMINAL);
      } else if(faststrcmp(tok->textV, "page") == 0 ||
                faststrcmp(tok->textV, "entry") == 0) {
         GetItem_Page(&d);
      /*
      } else if(faststrcmp(tok->textV, "failure") == 0) {
         GetItem_NumPage(&d, DPAGE_FAILURE);
      } else if(faststrcmp(tok->textV, "finished") == 0) {
         GetItem_NumPage(&d, DPAGE_FINISHED);
      } else if(faststrcmp(tok->textV, "unfinished") == 0) {
         GetItem_NumPage(&d, DPAGE_UNFINISHED);
      */
      } else {
         Log("%s: invalid identifier \"%s\"", __func__, tok->textV);
         break;
      }
   }

   TBufDtor(&d.tb);
   fclose(d.tb.fp);

   if(dbglevel & log_dlg)
   {
      for(struct dlg_def *def = dlgdefs; def; def = def->next) {
         printf("Disassembling script %i(%p,%u,%u)...\n", def->num, def->codeV,
             def->codeC, def->codeP);
         Disassemble(def);
         printf("Dumping code...\n");
         Dbg_PrintMemC(def->codeV, def->codeC);

         printf("Dumping string table for script %i(%p,%u,%u)...\n", def->num,
             def->stabV, def->stabC, def->stabP);
         Dbg_PrintMemC(def->stabV, def->stabC);
      }

      printf("Done.\n");
   }
}

/* Extern Functions -------------------------------------------------------- */

void Dlg_GInit(void)
{
}

void Dlg_MInit(void)
{
   /* Free any previous dialogue definitions. */
   if(dlgdefs)
   {
      for(struct dlg_def *def = dlgdefs; def;) {
         struct dlg_def *next = def->next;
         Vec_Clear(def->code);
         Vec_Clear(def->stab);
         Dalloc(def);
         def = next;
      }

      Dalloc(dlgdefs);
      dlgdefs = nil;
   }

   FILE *fp = W_Open(StrParam("lfiles/Dialogue_%tS.txt", PRINTNAME_LEVEL), "r");

   if(fp) LoadFile(fp);
}

#endif

/* EOF */
