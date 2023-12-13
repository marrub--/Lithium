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
script static bool chtf_dbg_dlg(cheat_params_t const params) {
   if(!IsDigit(params[0]) || !IsDigit(params[1])) {
      return false;
   }
   i32 p0 = params[0] - '0';
   i32 p1 = params[1] - '0';
   i32 n  = p0 * 10 + p1;
   struct dlg_def *def = &dlgdefs[n];
   ACS_BeginPrint();
   if(def->codeV) {
      PrintStrL("--- Script ");
      ACS_PrintHex(n);
      PrintStrL(" ---\n");
      __nprintf("Disassembling(%p,%u,%u)...\n", def->codeV, def->codeC,
                def->codeP);
      for(mem_size_t i = 0; i < def->codeP;) {
         __nprintf("%04X ", PRG_BEG + i);
         mem_size_t c = Cps_GetC(def->codeV, i++);
         i = Dlg_WriteCode(def, c, i);
         ACS_PrintChar('\n');
      }
      PrintStrL("Dumping code...\n");
      Dbg_PrintMemC(def->codeV, def->codeC);
      __nprintf("Dumping string table(%p,%u,%u)...\n", def->stabV, def->stabC, def->stabP);
      Dbg_PrintMemC(def->stabV, def->stabC);
   } else {
      __nprintf("dialogue %u has no code", n);
   }
   EndLogEx(_pri_critical|_pri_nonotify);
   return true;
}

struct cheat cht_dbg_dlg = cheat_s("pgdoki", 2, chtf_dbg_dlg, "Doki Doki Dialogue Club initialized. \CgJust Mab");
#endif

static void FinishDef(struct compiler *d) {
   if(d->def.codeV) {
      dlgdefs[d->num] = d->def;
   }
   fastmemset(&d->def, 0, sizeof d->def);
}

static void Dlg_GetItem_Page(struct compiler *d, mem_size_t num) {
   d->page = num;
   d->def.pages[num] = d->def.codeP;
   Dbg_Log(log_gsinfo,
           _l("--- page "), _p(num),
           _c(' '), _c('('), _p(d->def.codeP), _c(')'));
   Dlg_GetStmt(d); unwrap(&d->res);
   Dlg_PushLdVA(d, ACT_WAIT); unwrap(&d->res);
}

static bool Dlg_GetItem(struct compiler *d) {
   switch(Dlg_ItemName(tb_get(&d->tb)->textV)) {
   case _dlg_item_page:
      struct token *tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_number, 0);
      unwrap(&d->res);
      i32 num = faststrtoi32(tok->textV);
      if(num >= DPAGE_NORMAL_MAX || num < 0) {
         tb_err(&d->tb, &d->res, "bad page number", tok, _f);
         unwrap(&d->res);
      }
      Dlg_GetItem_Page(d, num);
      break;
   case _dlg_item_failure:    Dlg_GetItem_Page(d, DPAGE_FAILURE);    break;
   case _dlg_item_finished:   Dlg_GetItem_Page(d, DPAGE_FINISHED);   break;
   case _dlg_item_unfinished: Dlg_GetItem_Page(d, DPAGE_UNFINISHED); break;
   default:
      tb_unget(&d->tb);
      return false;
   }
   unwrap(&d->res);
   return true;
}

static void Dlg_GetTop_Prog(struct compiler *d, i32 beg, i32 end) {
   struct token *tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_number, 0);
   unwrap(&d->res);
   i32 num = beg + faststrtoi32(tok->textV);
   if(num > end || num < beg) {
      tb_err(&d->tb, &d->res, "invalid dialogue name", tok, _f);
      unwrap(&d->res);
   }
   tb_expdr(&d->tb, &d->res, tok_braceo);
   unwrap(&d->res);
   FinishDef(d);
   Dbg_Log(log_gsinfo, _l("\n---\nheading "), _p(num), _l("\n---"));
   d->num = num;
   while(!tb_drop(&d->tb, tok_bracec)) {
      Dlg_GetItem(d); unwrap(&d->res);
   }
}

mem_size_t Dlg_WriteCode(struct dlg_def const *def, mem_size_t c, mem_size_t i) {
   struct dcd_info const *inf = &dcdinfo[byte(c)];
   __nprintf("%02X ", c);
   if(c > 0xFF || !inf->name[0]) {
      PrintStrL("      ???.??        ");
      return i;
   }
   mem_size_t c2, c3;
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

static void Dlg_Compile(cstr fname) {
   noinit static struct compiler d;
   Dbg_Log(log_gsinfo, _l("begin compiling file "), _p(fname));
   FILE *fp = W_Open(fast_strdup(fname), 't');
   if(!fp) {
      PrintErr(_l("couldn't open file "), _p(fname));
      return;
   }
   fastmemset(&d, 0, sizeof d);
   tb_ctor(&d.tb, fp, fname);
   for(;;) {
      struct token *tok = tb_expc(&d.tb, &d.res, tb_get(&d.tb), tok_identi, tok_eof, 0);
      unwrap_goto(&d.res, done);
      if(tok->type == tok_eof) {
         break;
      }
      switch(Dlg_TopLevelName(tok->textV)) {
      case _dlg_toplevel_program:
         Dlg_GetTop_Prog(&d, DNUM_PRG_BEG, DNUM_PRG_END);
         break;
      case _dlg_toplevel_dialogue:
         Dlg_GetTop_Prog(&d, DNUM_DLG_BEG, DNUM_DLG_END);
         break;
      case _dlg_toplevel_terminal:
         Dlg_GetTop_Prog(&d, DNUM_TRM_BEG, DNUM_TRM_END);
         break;
      case _dlg_toplevel_var:
         Dlg_GetNamePool(&d, _name_pool_variables);
         break;
      case _dlg_toplevel_name:
         struct token *tok = tb_expc(&d.tb, &d.res, tb_get(&d.tb), tok_identi, 0);
         unwrap(&d.res);
         faststrcpy(d.name, tok->textV);
         tok = tb_expc(&d.tb, &d.res, tb_get(&d.tb), tok_semico, 0);
         unwrap(&d.res);
         break;
      default:
         tb_err(&d.tb, &d.res, "invalid toplevel", tok, _f);
      }
      unwrap_goto(&d.res, done);
   }
   FinishDef(&d);
done:
   unwrap_print(&d.res);
   tb_dtor(&d.tb);
   fclose(d.tb.fp);
   Xalloc(_tag_dlgc);
}

void Dlg_MInit(void) {
   Xalloc(_tag_dlgv);
   fastmemset(dlgdefs, 0, sizeof dlgdefs);
   if(!get_bit(ml.mi->use, _mi_key_script)) {
      return;
   }
   static char tmp[64];
   Dlg_Compile("lmisc/Common.mmmm");
   faststrcpy_str(tmp, ml.mi->keys[_mi_key_script].s);
   Dlg_Compile(tmp);
}

/* EOF */
