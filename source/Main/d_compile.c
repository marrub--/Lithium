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
#include "d_compile.h"

/* Static Functions -------------------------------------------------------- */

script static u32 WriteCode(struct dlg_def  const lmvar *def,
                            struct dcd_info const       *inf,
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

script static void Disassemble(struct dlg_def const lmvar *def)
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

/* Extern Functions -------------------------------------------------------- */

void Dlg_ClearDef(struct compiler *d)
{
   if(d->def.codeV) dlgdefs[d->num] = d->def;
   fastmemset(&d->def, 0, sizeof d->def);
}

void Dlg_GetItem_Page(struct compiler *d, u32 num, u32 act)
{
   d->def.pages[num] = d->def.codeP;

   Dbg_Log(log_dlg, "--- page %u (%u)", num, d->def.codeP);

   Dlg_GetStmt(d);

   if(act) Dlg_PushLdVA(d, act);
   Dlg_PushB1(d, DCD_BRK_NP);
}

bool Dlg_GetItem(struct compiler *d, u32 act)
{
   struct token *tok = d->tb.get();

   if(faststrcmp(tok->textV, "page") == 0) {
      tok = d->tb.get();
      Expect(d, tok, tok_number);

      u32 num = strtoi(tok->textV, nil, 0);
      if(num > DPAGE_NORMAL_MAX) Err(d, "bad page index");

      Dlg_GetItem_Page(d, num, act);
      return true;
   } else if(act == ACT_TRM_WAIT) {
      if(faststrcmp(tok->textV, "failure") == 0) {
         Dlg_GetItem_Page(d, DPAGE_FAILURE, ACT_TRM_WAIT);
         return true;
      } else if(faststrcmp(tok->textV, "finished") == 0) {
         Dlg_GetItem_Page(d, DPAGE_FINISHED, ACT_TRM_WAIT);
         return true;
      } else if(faststrcmp(tok->textV, "unfinished") == 0) {
         Dlg_GetItem_Page(d, DPAGE_UNFINISHED, ACT_TRM_WAIT);
         return true;
      }
   }

   d->tb.unget();
   return false;
}

void Dlg_GetTop_Prog(struct compiler *d, u32 act, u32 beg, u32 end)
{
   struct token *tok = d->tb.get();
   Expect(d, tok, tok_number);

   u32 num = beg + strtoi(tok->textV, nil, 0);
   if(num > end) ErrF(d, "invalid dialogue number %u", num);

   ExpectDrop(d, tok_semico);

   Dlg_ClearDef(d);
   d->num = num;

   Dbg_Log(log_dlg, "\n---\nheading %u\n---", num);

   while(Dlg_GetItem(d, act));
}

bool Dlg_GetTop(struct compiler *d)
{
   struct token *tok = d->tb.get();
   if(tok->type == tok_eof) return false;

   Expect(d, tok, tok_identi);

   if(faststrcmp(tok->textV, "program") == 0)
      Dlg_GetTop_Prog(d, 0, DNUM_PRG_BEG, DNUM_PRG_END);
   else if(faststrcmp(tok->textV, "dialogue") == 0)
      Dlg_GetTop_Prog(d, ACT_DLG_WAIT, DNUM_DLG_BEG, DNUM_DLG_END);
   else if(faststrcmp(tok->textV, "terminal") == 0)
      Dlg_GetTop_Prog(d, ACT_TRM_WAIT, DNUM_TRM_BEG, DNUM_TRM_END);
   else
      ErrF(d, "invalid toplevel item '%s'", tok->textV);

   return true;
}

script void Dlg_LoadFile(FILE *fp)
{
   struct compiler d = {{.bbeg = 14, .bend = 28, .fp = fp}, .ok = true};

   TBufCtor(&d.tb);

   if(setjmp(d.env)) {
      d.ok = false;
      goto done;
   }

   while(Dlg_GetTop(&d));

   Dlg_ClearDef(&d);

done:
   TBufDtor(&d.tb);
   fclose(d.tb.fp);
}

void Dlg_GInit(void)
{
}

void Dlg_MInit(void)
{
   /* Free any previous dialogue definitions. */
   for(u32 i = 0; i < countof(dlgdefs); i++) {
      struct dlg_def lmvar *def = &dlgdefs[i];

      if(def->codeV) {
         Vec_Clear(def->code);
         Vec_Clear(def->stab);
      }

      fastmemset(def, 0, sizeof *def, lmvar);
   }

   FILE *fp = W_Open(StrParam("lfiles/Dialogue_%tS.txt", PRINTNAME_LEVEL), "r");
   if(fp) Dlg_LoadFile(fp);

   if(dbglevel & log_dlg) {
      for(u32 i = 0; i < countof(dlgdefs); i++) {
         struct dlg_def lmvar *def = &dlgdefs[i];

         if(def->codeV) {
            printf("Disassembling script %u(%p,%u,%u)...\n", i, def->codeV,
                   def->codeC, def->codeP);
            Disassemble(def);
            printf("Dumping code...\n");
            Dbg_PrintMemC(def->codeV, def->codeC);

            printf("Dumping string table for script %u(%p,%u,%u)...\n", i,
                   def->stabV, def->stabC, def->stabP);
            Dbg_PrintMemC(def->stabV, def->stabC);
         }
      }

      printf("Done.\n");
   }
}

#endif

/* EOF */
