// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Dialogue compiler utilities.                                             │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "d_compile.h"

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

/* EOF */
