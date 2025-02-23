// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Dialogue compiler utilities.                                             │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "d_compile.h"

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

void Dlg_PushB1(struct compiler *d, i32 b) {
   mem_size_t pc = d->def.codeP++;
   if(pc + 1 > PRG_END - PRG_BEG) {
      tb_err(&d->tb, &d->res, "PRG segment overflow", nil, _f);
      unwrap(&d->res);
   }
   if(pc + 1 > d->def.codeC * 4) {
      d->def.codeC += 64;
      d->def.codeV = Talloc(d->def.codeV, d->def.codeC, _tag_dlgv);
   }
   if(b > 0xFF || b < 0) {
      tb_err(&d->tb, &d->res, "byte error %u", nil, _f, b);
      unwrap(&d->res);
   }
   Cps_SetC(d->def.codeV, pc, b);
}

void Dlg_PushB2(struct compiler *d, i32 word) {
   Dlg_PushB1(d, byte(word));      unwrap(&d->res);
   Dlg_PushB1(d, byte(word >> 8)); unwrap(&d->res);
}

void Dlg_PushLdVA(struct compiler *d, i32 action) {
   Dlg_PushB1(d, DCD_LDV_VI); unwrap(&d->res);
   Dlg_PushB1(d, action);     unwrap(&d->res);
}

struct ptr2 Dlg_PushLdAdr(struct compiler *d, mem_size_t at, i32 set) {
   struct ptr2 adr;
   Dlg_PushB1(d, DCD_LDA_VI); unwrap(&d->res);
   Dlg_PushB1(d, byte(set));  unwrap(&d->res);
   adr.l = d->def.codeP - 1;
   Dlg_PushB1(d, DCD_STA_AI); unwrap(&d->res);
   Dlg_PushB2(d, at);         unwrap(&d->res);
   Dlg_PushB1(d, DCD_LDA_VI); unwrap(&d->res);
   Dlg_PushB1(d, set >> 8);   unwrap(&d->res);
   adr.h = d->def.codeP - 1;
   Dlg_PushB1(d, DCD_STA_AI); unwrap(&d->res);
   Dlg_PushB2(d, at + 1);     unwrap(&d->res);
   return adr;
}

void Dlg_SetB1(struct compiler *d, mem_size_t ptr, i32 b) {
   if(b > 0xFF || b < 0) {
      tb_err(&d->tb, &d->res, "byte error %u", nil, _f, b);
      unwrap(&d->res);
   }
   Cps_SetC(d->def.codeV, ptr, b);
}

void Dlg_SetB2(struct compiler *d, mem_size_t ptr, i32 word) {
   Dlg_SetB1(d, ptr + 0, byte(word));      unwrap(&d->res);
   Dlg_SetB1(d, ptr + 1, byte(word >> 8)); unwrap(&d->res);
}

mem_size_t Dlg_PushStr(struct compiler *d, cstr s, mem_size_t l) {
   mem_size_t p  = d->def.stabP;
   mem_size_t vl = Cps_Size(p + l) - d->def.stabC;

   if(p + l > STR_END - STR_BEG) {
      tb_err(&d->tb, &d->res, "STR segment overflow", nil, _f);
      unwrap(&d->res);
   }

   Dbg_Log(log_gsinfo,
           _l(_f), _l(": ("), _p(l), _c(' '), _p(vl), _l(") '"), _p(s),
           _c('\''));

   d->def.stabC += vl;
   d->def.stabP += l;
   d->def.stabV  = Talloc(d->def.stabV, d->def.stabC, _tag_dlgv);

   for(mem_size_t i = 0; i < l; i++) Cps_SetC(d->def.stabV, p + i, s[i]);

   return STR_BEG + p;
}

#define varmap_t_GetKey(o)    ((o)->name)
#define varmap_t_GetNext(o)   (&(o)->next)
#define varmap_t_GetPrev(o)   (&(o)->prev)
#define varmap_t_HashKey(k)   (faststrhash(k))
#define varmap_t_HashObj(o)   (varmap_t_HashKey((o)->name))
#define varmap_t_KeyCmp(l, r) (faststrcmp(l, r))
GDCC_HashMap_Defn(varmap_t, cstr, struct compiler_var)

struct compiler_var *Dlg_GetVar(struct compiler *d, cstr check) {
   return d->vars->find(check);
}

void Dlg_SetVar(struct compiler *d, struct compiler_var *var) {
   d->vars->insert(var);
}

i32 Dlg_Evaluate(struct compiler *d, i32 end_token, i32 continue_token, bool *do_continue) {
   enum {_stack_top = 8};
   static i32 stack[_stack_top], top;
   top = _stack_top;
   struct token *tok = nil;
   for(bool not_first = false;
       !tok || (tok->type != end_token && tok->type != continue_token);
       not_first = true)
   {
      tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb),
                    tok_number, tok_identi,
                    tok_add, tok_sub, tok_add2, tok_sub2,
                    tok_div, tok_mod, tok_mul,
                    not_first ? end_token      : 0,
                    not_first ? continue_token : 0,
                    0);
      unwrap(&d->res);
      switch(tok->type) {
      underflow:
         tb_err(&d->tb, &d->res, "stack underflow", tok, _f);
         unwrap(&d->res);
      overflow:
         tb_err(&d->tb, &d->res, "stack overflow", tok, _f);
         unwrap(&d->res);
      case tok_number:
         if(top == 0) goto overflow;
         stack[--top] = faststrtoi32(tok->textV);
         break;
      case tok_identi:
         if(top == 0) goto overflow;
         struct compiler_var *var = Dlg_GetVar(d, tok->textV);
         if(var) {
            stack[--top] = var->value;
         } else {
            tb_err(&d->tb, &d->res, "unknown variable", tok, _f);
            unwrap(&d->res);
         }
         break;
      #define binary_op(op) \
         if(top > _stack_top - 2) goto underflow; \
         stack[top + 1] = stack[top + 1] op stack[top]; \
         ++top
      case tok_add: binary_op(+); break;
      case tok_sub: binary_op(-); break;
      case tok_div: binary_op(/); break;
      case tok_mod: binary_op(%); break;
      case tok_mul: binary_op(*); break;
      #define unary_op(op) \
         if(top > _stack_top - 1) goto underflow; \
         op stack[top]
      case tok_add2: unary_op(++); break;
      case tok_sub2: unary_op(--); break;
      }
   }
   if(top >= _stack_top || top < 0) {
      tb_err(&d->tb, &d->res, "stack error", tok, _f);
      unwrap(&d->res);
   }
   if(tok && continue_token && tok->type != continue_token) {
      *do_continue = false;
   }
   return stack[top];
}

/* EOF */
