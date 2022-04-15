// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Dialogue code compiler.                                                  │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "d_compile.h"
#include "m_trie.h"

#ifndef NDEBUG
script static
bool chtf_dbg_dlg(cheat_params_t const params) {
   if(!IsDigit(params[0]) || !IsDigit(params[1])) {
      return false;
   }

   i32 p0 = params[0] - '0';
   i32 p1 = params[1] - '0';
   i32 n  = p0 * 10 + p1;

   struct dlg_def *def = &dlgdefs[n];

   ACS_BeginPrint();
   if(def->codeV) {
      PrintChrLi("--- Script ");
      ACS_PrintHex(n);
      PrintChrLi(" ---\n");
      __nprintf("Disassembling(%p,%u,%u)...\n", def->codeV, def->codeC,
                def->codeP);

      for(u32 i = 0; i < def->codeP;) {
         __nprintf("%04X ", PRG_BEG + i);

         u32 c = Cps_GetC(def->codeV, i++);
         i = Dlg_WriteCode(def, c, i);
         ACS_PrintChar('\n');
      }
      PrintChrLi("Dumping code...\n");
      Dbg_PrintMemC(def->codeV, def->codeC);
      __nprintf("Dumping string table(%p,%u,%u)...\n", def->stabV, def->stabC,
                def->stabP);
      Dbg_PrintMemC(def->stabV, def->stabC);
   } else {
      __nprintf("dialogue %u has no code", n);
   }
   ACS_EndLog();

   return true;
}

struct cheat cht_dbg_dlg = cheat_s("pgdoki", 2, chtf_dbg_dlg, "Doki Doki Dialogue Club initialized. \CgJust Mab");
#endif

static
void FinishDef(struct compiler *d) {
   if(d->def.codeV) dlgdefs[d->num] = d->def;
   fastmemset(&d->def, 0, sizeof d->def);
}

static
void Dlg_GetItem_Page(struct compiler *d, u32 num, u32 act) {
   d->page = num;

   d->def.pages[num] = d->def.codeP;

   Dbg_Log(log_dlg,
           _l("--- page "), _p(num),
           _c(' '), _c('('), _p(d->def.codeP), _c(')'));

   Dlg_GetStmt(d); unwrap(&d->res);

   if(act != ACT_NONE) {
      Dlg_PushLdVA(d, act); unwrap(&d->res);
   }
   Dlg_PushB1(d, DCD_BRK_NP); unwrap(&d->res);
}

static
bool Dlg_GetItem(struct compiler *d, u32 act) {
   switch(Dlg_ItemName(d->tb.get()->textV)) {
   case _dlg_item_page: {
      struct token *tok = d->tb.expc(&d->res, d->tb.get(), tok_identi, 0);
      unwrap(&d->res);

      i32 num = Dlg_CheckNamePool(d, _name_pool_pages, tok->textV);
      if(num >= DPAGE_NORMAL_MAX || num < 0) {
         d->tb.err(&d->res, "bad page name '%s'", tok->textV);
         unwrap(&d->res);
      }

      Dlg_GetItem_Page(d, num, act);
      break;
   }
   case _dlg_item_failure:
      Dlg_GetItem_Page(d, DPAGE_FAILURE, act);
      break;
   case _dlg_item_finished:
      Dlg_GetItem_Page(d, DPAGE_FINISHED, act);
      break;
   case _dlg_item_unfinished:
      Dlg_GetItem_Page(d, DPAGE_UNFINISHED, act);
      break;
   case _dlg_item_page_names:
      Dlg_GetNamePool(d, _name_pool_pages);
      break;
   default:
      d->tb.unget();
      return false;
   }
   unwrap(&d->res);
   return true;
}

static
void Dlg_GetTop_Prog(struct compiler *d, i32 pool, u32 act, u32 beg, u32 end) {
   struct token *tok = d->tb.expc(&d->res, d->tb.get(), tok_identi, 0);
   unwrap(&d->res);

   i32 name = Dlg_CheckNamePool(d, pool, tok->textV);
   i32 num  = beg + name;
   if(num > end || num < beg) {
      d->tb.err(&d->res, "invalid dialogue name '%s'", tok->textV);
      unwrap(&d->res);
   }

   d->tb.expdr(&d->res, tok_braceo);
   unwrap(&d->res);

   FinishDef(d);

   Dbg_Log(log_dlg, _l("\n---\nheading "), _p(num), _l("\n---"));

   d->num  = num;
   d->pool = pool;
   d->name = name;
   Dlg_ClearNamePool(d, _name_pool_pages);

   while(!d->tb.drop(tok_bracec)) {
      Dlg_GetItem(d, act); unwrap(&d->res);
   }
}

u32 Dlg_WriteCode(struct dlg_def const *def, u32 c, u32 i) {
   u32 c2, c3;

   struct dcd_info const *inf = &dcdinfo[c];

   __nprintf("%02X ", c);

   if(!inf->name[0]) {
      PrintChrLi("       invalid opcode");
      return i;
   }

   switch(inf->adrm) {
   case ADRM_AI:
      c2 = Cps_GetC(def->codeV, i++);
      c3 = Cps_GetC(def->codeV, i++);
      __nprintf("%02X %02X %s $%02X%02X  ", c2, c3, inf->name, c3, c2);
      break;
   case ADRM_AX:
      c2 = Cps_GetC(def->codeV, i++);
      c3 = Cps_GetC(def->codeV, i++);
      __nprintf("%02X %02X %s $%02X%02X,X", c2, c3, inf->name, c3, c2);
      break;
   case ADRM_AY:
      c2 = Cps_GetC(def->codeV, i++);
      c3 = Cps_GetC(def->codeV, i++);
      __nprintf("%02X %02X %s $%02X%02X,Y", c2, c3, inf->name, c3, c2);
      break;
   case ADRM_II:
      c2 = Cps_GetC(def->codeV, i++);
      c3 = Cps_GetC(def->codeV, i++);
      __nprintf("%02X %02X %s ($%02X%02X)", c2, c3, inf->name, c3, c2);
      break;
   case ADRM_IX:
      c2 = Cps_GetC(def->codeV, i++);
      __nprintf("%02X    %s ($%02X,X) ", c2, inf->name, c2);
      break;
   case ADRM_IY:
      c2 = Cps_GetC(def->codeV, i++);
      __nprintf("%02X    %s ($%02X),Y", c2, inf->name, c2);
      break;
   case ADRM_NP:
      __nprintf("      %s        ", inf->name);
      break;
   case ADRM_ZI:
   case ADRM_RI:
      c2 = Cps_GetC(def->codeV, i++);
      __nprintf("%02X    %s $%02X    ", c2, inf->name, c2);
      break;
   case ADRM_VI:
      c2 = Cps_GetC(def->codeV, i++);
      __nprintf("%02X    %s #$%02X   ", c2, inf->name, c2);
      break;
   case ADRM_ZX:
      c2 = Cps_GetC(def->codeV, i++);
      __nprintf("%02X    %s $%02X,X  ", c2, inf->name, c2);
      break;
   case ADRM_ZY:
      c2 = Cps_GetC(def->codeV, i++);
      __nprintf("%02X    %s $%02X,Y  ", c2, inf->name, c2);
      break;
   }

   return i;
}

void Dlg_MInit(void) {
   noinit static struct compiler d;

   /* Free any previous dialogue definitions. */
   Xalloc(_tag_dlgv);
   fastmemset(dlgdefs, 0, sizeof dlgdefs);
   fastmemset(&d,      0, sizeof d);

   FILE *fp =
      W_Open(strp(_l("lfiles/Dialogue_"), _p(ml.maplump), _l(".txt")), 't');

   if(fp) {
      TBufCtor(&d.tb, fp, "Dialogue file");

      for(;;) {
         struct token *tok = d.tb.expc(&d.res, d.tb.get(), tok_identi, tok_eof, 0);
         unwrap_do(&d.res, goto done;);
         if(tok->type == tok_eof) break;

         switch(Dlg_TopLevelName(tok->textV)) {
         case _dlg_toplevel_program:
            Dlg_GetTop_Prog(&d, _name_pool_program, ACT_NONE, DNUM_PRG_BEG, DNUM_PRG_END);
            break;
         case _dlg_toplevel_dialogue:
            Dlg_GetTop_Prog(&d, _name_pool_dialogue, ACT_DLG_WAIT, DNUM_DLG_BEG, DNUM_DLG_END);
            break;
         case _dlg_toplevel_terminal:
            Dlg_GetTop_Prog(&d, _name_pool_terminal, ACT_TRM_WAIT, DNUM_TRM_BEG, DNUM_TRM_END);
            break;
         case _dlg_toplevel_program_names:
            Dlg_GetNamePool(&d, _name_pool_program);
            break;
         case _dlg_toplevel_dialogue_names:
            Dlg_GetNamePool(&d, _name_pool_dialogue);
            break;
         case _dlg_toplevel_terminal_names:
            Dlg_GetNamePool(&d, _name_pool_terminal);
            break;
         case _dlg_toplevel_var:
            Dlg_GetNamePool(&d, _name_pool_variables);
            break;
         default:
            d.tb.err(&d.res, "invalid toplevel item '%s'", tok->textV);
         }
         unwrap_do(&d.res, goto done;);
      }

      FinishDef(&d);

   done:
      unwrap_print(&d.res);

      TBufDtor(&d.tb);
      fclose(d.tb.fp);
      Xalloc(_tag_dlgc);
   }
}

/* EOF */
