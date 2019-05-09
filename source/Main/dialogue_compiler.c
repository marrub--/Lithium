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

#include <setjmp.h>

#define Error(d) longjmp((d)->env, 1)

#define ErrF(d, fmt, ...) \
   (Log("%s: " fmt, __func__, __VA_ARGS__), Error(d))

#define Err(d, fmt) \
   (Log("%s: " fmt, __func__), Error(d))

#define ErrT(d, tok, s) \
   (Log("(%i:%i) %s: " s " (%i:'%s')", tok->orig.line, tok->orig.colu, \
        __func__, tok->type, tok->textV ? tok->textV : "<no string>"), \
    Error(d))

#define Expect(d, tok, typ) \
   if(tok->type != typ) ErrT(d, tok, "expected " #typ)

#define Expect2(d, tok, typ1, typ2) \
   if(tok->type != typ1 && tok->type != typ2) \
      ErrT(d, tok, "expected " #typ1 " or " #typ2)

#define Expect3(d, tok, typ1, typ2, typ3) \
   if(tok->type != typ1 && tok->type != typ2 && tok->type != typ3) \
      ErrT(d, tok, "expected " #typ1 ", " #typ2 " or " #typ3)

#define ExpectDrop(d, typ) \
   if(!d->tb.drop(typ)) {\
      struct token *_tok = d->tb.reget(); \
      ErrT(d, _tok, "expected " #typ); \
   }

#define CheckKw(tok, kw) \
   (tok->type == tok_identi && faststrcmp(tok->textV, kw) == 0)

/* Types ------------------------------------------------------------------- */

struct compiler {
   struct tokbuf   tb;
   struct dlg_def *def;
   jmp_buf         env;
   bool            ok;
};

struct ptr2 {
   u32 l, h;
};

/* Static Functions -------------------------------------------------------- */

script static void GetStatement(struct compiler *d);

static void PushB1(struct compiler *d, u32 b)
{
   size_t pc = d->def->codeP++;

   if(pc + 1 > PRG_END - PRG_BEG) Err(d, "PRG segment overflow");

   if(pc + 1 > d->def->codeC * 4) {
      Vec_Grow(d->def->code, 1);
      d->def->codeC++;
   }

   if(b > 0xFF) ErrF(d, "byte error (overflow) %u", b);

   Cps_SetC(d->def->codeV, pc, b);
}

static void PushB2(struct compiler *d, u32 word)
{
   PushB1(d, word & 0xFF);
   PushB1(d, word >> 8);
}

static void PushLdv(struct compiler *d, u32 action)
{
   PushB1(d, DCD_LDV_VI);
   PushB1(d, action);
}

static struct ptr2 PushLoadAdr(struct compiler *d, u32 at, u32 set)
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

static void SetB1(struct compiler *d, u32 ptr, u32 b)
{
   if(b > 0xFF) ErrF(d, "byte error (overflow) %u", b);

   Cps_SetC(d->def->codeV, ptr, b);
}

static void SetB2(struct compiler *d, u32 ptr, u32 word)
{
   SetB1(d, ptr + 0, word & 0xFF);
   SetB1(d, ptr + 1, word >> 8);
}

static u32 PushST(struct compiler *d, cstr s, u32 l)
{
   u32  p = d->def->stabP;
   u32 vl = Cps_Adjust(p + l) - d->def->stabC;

   if(p + l > STR_END - STR_BEG) Err(d, "STR segment overflow");

   Dbg_Log(log_dlg, "%s: (%3u %3u) '%s'", __func__, l, vl, s);

   Vec_Grow(d->def->stab, vl);
   d->def->stabC += vl;
   d->def->stabP += l;

   for(u32 i = 0; i < l; i++) Cps_SetC(d->def->stabV, p + i, s[i]);

   return STR_BEG + p;
}

static i32 CodeABS(struct compiler *d, cstr reg)
{
   struct token *tok = d->tb.get();

   if(tok->type == tok_number) {
      i32 n = strtoi(tok->textV, nil, 0);

      if(n <= 0xFFFF) {
         if(!reg) {
            return n;
         } else if(d->tb.drop(tok_comma)) {
            tok = d->tb.get();

            if(CheckKw(tok, reg)) return n;

            d->tb.unget();
         }
      }
   }

   d->tb.unget();

   return false;
}

static i32 CodeZPG(struct compiler *d, cstr reg)
{
   struct token *tok = d->tb.get();

   if(tok->type == tok_number) {
      i32 n = strtoi(tok->textV, nil, 0);

      if(n <= 0xFF) {
         if(!reg) {
            return n;
         } else if(d->tb.drop(tok_comma)) {
            tok = d->tb.get();

            if(CheckKw(tok, reg)) return n;

            d->tb.unget();
         }
      }
   }

   d->tb.unget();

   return -1;
}

static bool CodeAI(struct compiler *d, u32 code)
{
   ifw(i32 n = CodeABS(d, nil), n > 0) {
      PushB1(d, code);
      PushB1(d, n & 0xFF);
      PushB1(d, n >> 8);
      return true;
   }

   return false;
}

static bool CodeAX(struct compiler *d, u32 code)
{
   ifw(i32 n = CodeABS(d, "X"), n > 0) {
      PushB1(d, code);
      PushB1(d, n & 0xFF);
      PushB1(d, n >> 8);
      return true;
   }

   return false;
}

static bool CodeAY(struct compiler *d, u32 code)
{
   ifw(i32 n = CodeABS(d, "Y"), n > 0) {
      PushB1(d, code);
      PushB1(d, n & 0xFF);
      PushB1(d, n >> 8);
      return true;
   }

   return false;
}

static bool CodeII(struct compiler *d, u32 code)
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

static bool CodeIX(struct compiler *d, u32 code)
{
   if(d->tb.drop(tok_pareno)) {
      struct token *tok = d->tb.get();

      if(tok->type == tok_number) {
         i32 n = strtoi(tok->textV, nil, 0);

         if(n <= 0xFF && d->tb.drop(tok_comma)) {
            tok = d->tb.get();

            if(CheckKw(tok, "X") && d->tb.drop(tok_parenc)) {
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

static bool CodeIY(struct compiler *d, u32 code)
{
   if(d->tb.drop(tok_pareno)) {
      struct token *tok = d->tb.get();

      if(tok->type == tok_number) {
         i32 n = strtoi(tok->textV, nil, 0);

         if(n <= 0xFF && d->tb.drop(tok_parenc) && d->tb.drop(tok_comma)) {
            tok = d->tb.get();

            if(CheckKw(tok, "Y")) {
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

static bool CodeVI(struct compiler *d, u32 code)
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

static bool CodeZI(struct compiler *d, u32 code)
{
   ifw(i32 n = CodeZPG(d, nil), n > 0) {
      PushB1(d, code);
      PushB1(d, n);
      return true;
   }

   return false;
}

static bool CodeZX(struct compiler *d, u32 code)
{
   ifw(i32 n = CodeZPG(d, "X"), n > 0) {
      PushB1(d, code);
      PushB1(d, n);
      return true;
   }

   return false;
}

static bool CodeZY(struct compiler *d, u32 code)
{
   ifw(i32 n = CodeZPG(d, "Y"), n > 0) {
      PushB1(d, code);
      PushB1(d, n);
      return true;
   }

   return false;
}

static void GetCode_Asm(struct compiler *d)
{
   register struct token *tok;

   #define DCD(n, op, ty) \
      tok = d->tb.reget(); \
      if(faststrcasecmp(tok->textV, #op) == 0) \
         if(Code##ty(d, DCD_##op##_##ty)) return;
   #include "dialogue.h"

   tok = d->tb.reget();
   ErrT(d, tok, "no function found with this syntax");
}

static void GetCode_Text(struct compiler *d, struct token *tok, u32 act)
{
   PushLoadAdr(d, VAR_ADRL, PushST(d, tok->textV, tok->textC));
   PushLdv(d, act);
}

static void GetCode_Cond(struct compiler *d)
{
   struct token *tok = d->tb.get();
   Expect(d, tok, tok_identi);

   u32 ins = DCD_BNE_RI;

   if(faststrcmp(tok->textV, "item") == 0) {
      tok = d->tb.get();
      Expect(d, tok, tok_string);

      ins = DCD_BEQ_RI;

      PushLoadAdr(d, VAR_ADRL, PushST(d, tok->textV, tok->textC));

      PushLdv(d, ACT_LD_ITEM);
   } else if(faststrcmp(tok->textV, "class") == 0) {
      tok = d->tb.get();
      Expect(d, tok, tok_identi);

      PushB1(d, DCD_LDA_AI);
      PushB2(d, VAR_PCLASS);

      PushB1(d, DCD_CMP_VI);
      #define LITH_X(shr, lng) \
         if(faststrcmp(tok->textV, #shr) == 0) {PushB1(d, shr); goto ok;}
      #include "p_player.h"
      ErrT(d, tok, "invalid playerclass type");
   ok:;
   } else {
      ErrT(d, tok, "invalid conditional type");
   }

   PushB1(d, ins);
   PushB1(d, 0); /* placeholder */
   u32 ptr = d->def->codeP;

   GetStatement(d);

   u32 rel = d->def->codeP - ptr;
   if(rel > 0x7F) Err(d, "bad jump (too much code)");

   SetB1(d, ptr - 1, rel);

   tok = d->tb.get();
   if(CheckKw(tok, "else")) {
      PushB1(d, DCD_JMP_AI);
      PushB2(d, 0);
      ptr = d->def->codeP;

      GetStatement(d);

      SetB2(d, ptr - 2, PRG_BEG + d->def->codeP);
   } else {
      d->tb.unget();
   }
}

static void GetCode_Option(struct compiler *d)
{
   struct ptr2 adr;

   struct token *tok = d->tb.get();
   Expect2(d, tok, tok_identi, tok_string);

   PushLoadAdr(d, VAR_ADRL, PushST(d, tok->textV, tok->textC));
   adr = PushLoadAdr(d, VAR_RADRL, 0); /* placeholder */

   PushLdv(d, ACT_LD_OPT);

   PushB1(d, DCD_JMP_AI);
   PushB2(d, 0); /* placeholder */

   u32 ptr = d->def->codeP;
   u32 rel = PRG_BEG + ptr;

   SetB1(d, adr.l, rel & 0xFF);
   SetB1(d, adr.h, rel >> 8);

   GetStatement(d);

   SetB2(d, ptr - 2, PRG_BEG + d->def->codeP);
}

static void GetCode_Str(struct compiler *d, u32 adr)
{
   struct token *tok = d->tb.get();
   Expect3(d, tok, tok_identi, tok_string, tok_number);

   PushLoadAdr(d, adr, PushST(d, tok->textV, tok->textC));
}

static void GetCode_Script(struct compiler *d)
{
   struct token *tok = d->tb.get();
   Expect2(d, tok, tok_string, tok_number);

   u32 act;

   if(tok->type == tok_string) {
      act = ACT_SCRIPT_S;
      PushLoadAdr(d, VAR_ADRL, PushST(d, tok->textV, tok->textC));
   } else {
      u32 prm = strtoi(tok->textV, nil, 0);

      act = ACT_SCRIPT_I;

      PushB1(d, DCD_LDA_VI);
      PushB1(d, prm);

      PushB1(d, DCD_STA_AI);
      PushB2(d, VAR_SCP0);
   }

   for(u32 i = 0; i < 4 && d->tb.drop(tok_comma); i++) {
      tok = d->tb.get();
      Expect(d, tok, tok_number);

      u32 prm = strtoi(tok->textV, nil, 0);

      PushB1(d, DCD_LDA_VI);
      PushB1(d, prm);

      PushB1(d, DCD_STA_AI);
      PushB2(d, VAR_SCP1 + i);
   }

   PushLdv(d, act);
}

static void GetCode_Line(struct compiler *d)
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
            Expect(d, tok, tok_number);
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
      case tok_quote:
         GetCode_Text(d, tok, ACT_TEXT_ADDI);
         break;
      case tok_hash:
         tok = d->tb.get();
         Expect2(d, tok, tok_identi, tok_string);
         GetCode_Text(d, tok, ACT_TEXT_ADDL);
         break;
      case tok_semico:
         break;
      default:
         ErrT(d, tok, "invalid token in line");
         break;
   }
}

static void GetBlock(struct compiler *d)
{
   while(!d->tb.drop(tok_bracec)) GetStatement(d);
}

static void GetConcatBlock(struct compiler *d)
{
   PushB1(d, DCD_INC_AI);
   PushB2(d, VAR_CONCAT);

   while(!d->tb.drop(tok_at2)) GetStatement(d);

   PushB1(d, DCD_DEC_AI);
   PushB2(d, VAR_CONCAT);
}

script static void GetStatement(struct compiler *d)
{
        if(d->tb.drop(tok_braceo)) GetBlock(d);
   else if(d->tb.drop(tok_at2))    GetConcatBlock(d);
   else                            GetCode_Line(d);
}

static void GetItem_Head(struct compiler *d, i32 head)
{
   struct token *tok = d->tb.get();
   Expect(d, tok, tok_number);

   i32 num = head + strtoi(tok->textV, nil, 0);

   struct dlg_def *last = d->def;

   d->def = Salloc(struct dlg_def);
   d->def->num = num;

   if(!last) dlgdefs    = d->def;
   else      last->next = d->def;

   Dbg_Log(log_dlg, "\n---\nheading %i (%i)\n---", d->def->num, d->def->codeP);

   ExpectDrop(d, tok_semico);
}

static void SetupPage(struct compiler *d, u32 num)
{
   /* TODO: make this syntax dependent */
   if(!d->def) Err(d, "not in a definition");

   if(num > countof(d->def->pages)) Err(d, "bad page index");

   d->def->pages[num] = d->def->codeP;

   Dbg_Log(log_dlg, "--- page %u (%u)", num, d->def->codeP);
}

static void GetItem_Page(struct compiler *d)
{
   struct token *tok = d->tb.get();
   Expect(d, tok, tok_number);

   SetupPage(d, strtoi(tok->textV, nil, 0));

   GetStatement(d);

   PushLdv(d, ACT_DLG_WAIT);
   PushB1(d, DCD_BRK_NP);
}

/*
static void GetItem_NumPage(struct compiler *d, i32 num)
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
   for(u32 i = 0; i < def->codeP;) {
      printf("%04X ", PRG_BEG + i);

      u32 c = Cps_GetC(def->codeV, i++);
      printf("%02X ", c);

      struct dcd_info const *inf = &dcdinfo[c];

      if(inf->name[0]) i = WriteCode(def, inf, i);
      else             printf("\t\tinvalid opcode\n");
   }
}

script static void LoadFile(FILE *fp)
{
   struct compiler d = {{.bbeg = 14, .bend = 28, .fp = fp}, .ok = true};

   TBufCtor(&d.tb);

   if(setjmp(d.env)) {
      d.ok = false;
      goto done;
   }

   for(struct token *tok; (tok = d.tb.get())->type != tok_eof;) {
      Expect(&d, tok, tok_identi);

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
         ErrF(&d, "invalid identifier '%s'", tok->textV);
      }
   }

done:
   TBufDtor(&d.tb);
   fclose(d.tb.fp);

   if(d.ok && dbglevel & log_dlg) {
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
   if(dlgdefs) {
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
