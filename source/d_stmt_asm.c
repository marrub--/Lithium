// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Dialogue compiler assembly statements.                                   │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "d_compile.h"

static
i32 CodeABS(struct compiler *d, cstr reg) {
   struct token *tok = tb_get(&d->tb);
   i32 n;
   switch(tok->type) {
   case tok_number:
      n = faststrtoi32(tok->textV);
      if(n <= 0xFFFF) {
         if(!reg) {
            return n;
         } else if(tb_drop(&d->tb, tok_comma)) {
            tok = tb_get(&d->tb);

            if(TokIsKw(tok, reg)) return n;

            tb_unget(&d->tb);
         }
      }
      break;
   case tok_identi:
      n = -1;
      switch(Dlg_ItemName(tok->textV)) {
      case _dlg_item_page:
         tok = tb_get(&d->tb);
         if(tok->type == tok_number) {
            n = faststrtoi32(tok->textV);
            if(n >= DPAGE_MAX || n < 0) {
               tb_err(&d->tb, &d->res, "bad page number", tok, _f);
               unwrap(&d->res);
            }
         } else {
            tb_unget(&d->tb);
         }
         break;
      case _dlg_item_failure:    n = DPAGE_FAILURE;    break;
      case _dlg_item_finished:   n = DPAGE_FINISHED;   break;
      case _dlg_item_unfinished: n = DPAGE_UNFINISHED; break;
      }
      if(n >= 0) {
         return PRG_BEG + d->def.pages[n];
      }
      break;
   case tok_mod:
      tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_identi, 0);
      unwrap(&d->res);
      n = Dlg_CheckNamePool(d, _name_pool_variables, tok->textV);
      if(n >= 0) {
         return VAR_END - n;
      } else {
         tb_err(&d->tb, &d->res, "unknown variable", tok, _f);
         unwrap(&d->res);
      }
      break;
   }
   tb_unget(&d->tb);
   return -1;
}

static
i32 CodeZPG(struct compiler *d, cstr reg) {
   struct token *tok = tb_get(&d->tb);
   if(tok->type == tok_number) {
      i32 n = faststrtoi32(tok->textV);
      if(n <= 0xFF) {
         if(!reg) {
            return n;
         } else if(tb_drop(&d->tb, tok_comma)) {
            tok = tb_get(&d->tb);
            if(TokIsKw(tok, reg)) return n;
            tb_unget(&d->tb);
         }
      }
   }
   tb_unget(&d->tb);
   return -1;
}

static
bool CodeAI(struct compiler *d, i32 code) {
   ifw(i32 n = CodeABS(d, nil), n >= 0) {
      Dlg_PushB1(d, code); unwrap(&d->res);
      Dlg_PushB2(d, n); unwrap(&d->res);
      return true;
   }
   return false;
}

static
bool CodeAX(struct compiler *d, i32 code) {
   ifw(i32 n = CodeABS(d, "X"), n >= 0) {
      Dlg_PushB1(d, code); unwrap(&d->res);
      Dlg_PushB2(d, n); unwrap(&d->res);
      return true;
   }
   return false;
}

static
bool CodeAY(struct compiler *d, i32 code) {
   ifw(i32 n = CodeABS(d, "Y"), n >= 0) {
      Dlg_PushB1(d, code); unwrap(&d->res);
      Dlg_PushB2(d, n); unwrap(&d->res);
      return true;
   }
   return false;
}

static
bool CodeII(struct compiler *d, i32 code) {
   if(tb_drop(&d->tb, tok_pareno)) {
      struct token *tok = tb_get(&d->tb);
      if(tok->type == tok_number) {
         i32 n = faststrtoi32(tok->textV);
         if(n <= 0xFFFF && tb_drop(&d->tb, tok_parenc)) {
            Dlg_PushB1(d, code); unwrap(&d->res);
            Dlg_PushB2(d, n); unwrap(&d->res);
            return true;
         }
      }
      tb_unget(&d->tb);
   }
   return false;
}

static
bool CodeIX(struct compiler *d, i32 code) {
   if(tb_drop(&d->tb, tok_pareno)) {
      struct token *tok = tb_get(&d->tb);
      if(tok->type == tok_number) {
         i32 n = faststrtoi32(tok->textV);
         if(n <= 0xFF && tb_drop(&d->tb, tok_comma)) {
            tok = tb_get(&d->tb);
            if(TokIsKw(tok, "X") && tb_drop(&d->tb, tok_parenc)) {
               Dlg_PushB1(d, code); unwrap(&d->res);
               Dlg_PushB1(d, n); unwrap(&d->res);
               return true;
            }
            tb_unget(&d->tb);
         }
      }
      tb_unget(&d->tb);
   }
   return false;
}

static
bool CodeIY(struct compiler *d, i32 code) {
   if(tb_drop(&d->tb, tok_pareno)) {
      struct token *tok = tb_get(&d->tb);
      if(tok->type == tok_number) {
         i32 n = faststrtoi32(tok->textV);
         if(n <= 0xFF && tb_drop(&d->tb, tok_parenc) && tb_drop(&d->tb, tok_comma)) {
            tok = tb_get(&d->tb);
            if(TokIsKw(tok, "Y")) {
               Dlg_PushB1(d, code); unwrap(&d->res);
               Dlg_PushB1(d, n); unwrap(&d->res);
               return true;
            }
            tb_unget(&d->tb);
         }
      }
      tb_unget(&d->tb);
   }
   return false;
}

static
bool CodeNP(struct compiler *d, i32 code) {
   Dlg_PushB1(d, code); unwrap(&d->res);
   return true;
}

#define CodeRI CodeZI

static
bool CodeVI(struct compiler *d, i32 code) {
   if(tb_drop(&d->tb, tok_hash)) {
      struct token *tok = tb_get(&d->tb);
      if(tok->type == tok_number) {
         i32 n = faststrtoi32(tok->textV);
         if(n <= 0xFF) {
            Dlg_PushB1(d, code); unwrap(&d->res);
            Dlg_PushB1(d, n); unwrap(&d->res);
            return true;
         }
      }
      tb_unget(&d->tb);
   }
   return false;
}

static
bool CodeZI(struct compiler *d, i32 code) {
   ifw(i32 n = CodeZPG(d, nil), n >= 0) {
      Dlg_PushB1(d, code); unwrap(&d->res);
      Dlg_PushB1(d, n); unwrap(&d->res);
      return true;
   }
   return false;
}

static
bool CodeZX(struct compiler *d, i32 code) {
   ifw(i32 n = CodeZPG(d, "X"), n >= 0) {
      Dlg_PushB1(d, code); unwrap(&d->res);
      Dlg_PushB1(d, n); unwrap(&d->res);
      return true;
   }
   return false;
}

static
bool CodeZY(struct compiler *d, i32 code) {
   ifw(i32 n = CodeZPG(d, "Y"), n >= 0) {
      Dlg_PushB1(d, code); unwrap(&d->res);
      Dlg_PushB1(d, n); unwrap(&d->res);
      return true;
   }
   return false;
}

void Dlg_GetStmt_Asm(struct compiler *d) {
   struct token *tok;
   #define DCD(n, op, ty) \
      tok = tb_reget(&d->tb); \
      if(faststrcasechk(tok->textV, #op)) { \
         bool ret = Code##ty(d, DCD_##op##_##ty); \
         unwrap(&d->res); \
         if(ret) { \
            tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_semico, 0); \
            unwrap(&d->res); \
            return; \
         } \
      }
   #include "d_vm.h"
   tok = tb_reget(&d->tb);
   tb_err(&d->tb, &d->res, "no function found with this syntax", tok, _f);
   unwrap(&d->res);
}

/* EOF */
