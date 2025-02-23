// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Dialogue code compiler.                                                  │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "d_compile.h"
#include "m_trie.h"

noinit static struct compiler_var vars[256];
static i32 varn;

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
   d->def.pages[num] = PRG_BEG + d->def.codeP;
   Dbg_Log(log_gsinfo,
           _l("--- page "), _p(num),
           _c(' '), _c('('), _p(d->def.codeP), _c(')'));
   Dlg_GetStmt(d); unwrap(&d->res);
   Dlg_PushLdVA(d, ACT_WAIT); unwrap(&d->res);
}

static void Dlg_GetItem_Var(struct compiler *d) {
   i32 last_val = -1;
   bool loop = tb_drop(&d->tb, tok_braceo);
   do {
      struct token *tok =
         tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_identi, 0);
      unwrap(&d->res);
      struct compiler_var *var = Dlg_GetVar(d, tok->textV);
      char *id;
      if(!var) {
         id = Malloc(tok->textC + 1, _tag_dlgc);
         faststrcpy(id, tok->textV);
      }
      i32 val;
      if(loop) {
         if(tb_drop(&d->tb, tok_eq)) {
            val = Dlg_Evaluate(d, tok_comma);
            unwrap(&d->res);
         } else {
            val = last_val + 1;
            tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_comma, 0);
            unwrap(&d->res);
         }
      } else {
         tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_eq, 0);
         unwrap(&d->res);
         val = Dlg_Evaluate(d, tok_semico);
         unwrap(&d->res);
      }
      last_val = val;
      if(!var) {
         var = &vars[varn++];
         var->name  = id;
         var->value = val;
         Dlg_SetVar(d, var);
      } else {
         var->value = val;
      }
      Dbg_Log(log_gsinfo, _l("set variable "), _p(var->name),
              _l(" to "), _p(var->value));
   } while(loop && !tb_drop(&d->tb, tok_bracec));
}

static bool Dlg_GetItem(struct compiler *d) {
   struct token *tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_identi, tok_bracec, 0);
   unwrap(&d->res);
   if(tok->type == tok_bracec) return false;
   switch(Dlg_ItemName(tok->textV)) {
   case _dlg_item_page:
      struct token *tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_pareno, 0);
      unwrap(&d->res);
      i32 num = Dlg_Evaluate(d, tok_parenc);
      unwrap(&d->res);
      if(num >= DPAGE_MAX || num < 0) {
         tb_err(&d->tb, &d->res, "bad page number %i", tok, _f, num);
         unwrap(&d->res);
      }
      Dlg_GetItem_Page(d, num);
      break;
   case _dlg_item_var:
      Dlg_GetItem_Var(d);
      break;
   default:
      tb_unget(&d->tb);
      return false;
   }
   unwrap(&d->res);
   return true;
}

script static void Dlg_GetTop_Prog(struct compiler *d) {
   struct token *tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_pareno, 0);
   unwrap(&d->res);
   i32 num = Dlg_Evaluate(d, tok_parenc);
   unwrap(&d->res);
   if(num > PNUM_MAX || num < 0) {
      tb_err(&d->tb, &d->res, "invalid dialogue number", tok, _f);
      unwrap(&d->res);
   }
   tb_expdr(&d->tb, &d->res, tok_braceo);
   unwrap(&d->res);
   FinishDef(d);
   Dbg_Log(log_gsinfo, _l("\n---\nheading "), _p(num), _l("\n---"));
   d->num = num;
   while(Dlg_GetItem(d)) {
      unwrap(&d->res);
   }
}

script static void Dlg_GetTop_Name(struct compiler *d) {
   struct token *tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_identi, 0);
   unwrap(&d->res);
   faststrcpy(d->name, tok->textV);
   tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_semico, 0);
   unwrap(&d->res);
}

script static void Dlg_Compile(cstr fname, varmap_t *varmap) {
   noinit static struct compiler d;
   Dbg_Log(log_gsinfo, _l("begin compiling file "), _p(fname));
   FILE *fp = W_Open(fast_strdup(fname), 't');
   if(!fp) {
      PrintErr(_l("couldn't open file "), _p(fname));
      return;
   }
   fastmemset(&d, 0, sizeof d);
   tb_ctor(&d.tb, fp, fname);
   d.vars = varmap;
   for(;;) {
      struct token *tok = tb_expc(&d.tb, &d.res, tb_get(&d.tb), tok_identi, tok_eof, 0);
      unwrap_goto(&d.res, done);
      if(tok->type == tok_eof) {
         break;
      }
      switch(Dlg_TopLevelName(tok->textV)) {
      case _dlg_toplevel_program:
         Dlg_GetTop_Prog(&d);
         break;
      case _dlg_toplevel_var:
         Dlg_GetItem_Var(&d);
         break;
      case _dlg_toplevel_name:
         Dlg_GetTop_Name(&d);
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
}

void Dlg_MInit(void) {
   /* clear VM data */
   Xalloc(_tag_dlgv);
   fastmemset(dlgdefs, 0, sizeof dlgdefs);
   varn = 0;
   /* always compile common file */
   varmap_t varmap;
   varmap_t_ctor(&varmap, 16, 16);
   Dlg_Compile("lmisc/Common.mmmm", &varmap);
   /* if scripts are used, compile them */
   if(get_bit(ml.mi->use, _mi_key_script)) {
      static char tmp[64];
      faststrcpy_str(tmp, ml.mi->keys[_mi_key_script].s);
      Dlg_Compile(tmp, &varmap);
   }
   /* clean up compiler data */
   varmap_t_dtor(&varmap);
   Xalloc(_tag_dlgc);
}

/* EOF */
