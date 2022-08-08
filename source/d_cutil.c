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

void Dlg_PushB1(struct compiler *d, u32 b) {
   mem_size_t pc = d->def.codeP++;

   if(pc + 1 > PRG_END - PRG_BEG) {
      tb_err(&d->tb, &d->res, "PRG segment overflow", nil, _f);
      unwrap(&d->res);
   }

   if(pc + 1 > d->def.codeC * 4) {
      d->def.codeC += 64;
      d->def.codeV = Talloc(d->def.codeV, d->def.codeC, _tag_dlgv);
   }

   if(b > 0xFF) {
      tb_err(&d->tb, &d->res, "byte error (overflow) %u", nil, _f, b);
      unwrap(&d->res);
   }

   Cps_SetC(d->def.codeV, pc, b);
}

void Dlg_PushB2(struct compiler *d, u32 word) {
   Dlg_PushB1(d, byte(word));      unwrap(&d->res);
   Dlg_PushB1(d, byte(word >> 8)); unwrap(&d->res);
}

void Dlg_PushLdVA(struct compiler *d, u32 action) {
   Dlg_PushB1(d, DCD_LDV_VI); unwrap(&d->res);
   Dlg_PushB1(d, action);     unwrap(&d->res);
}

struct ptr2 Dlg_PushLdAdr(struct compiler *d, u32 at, u32 set) {
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

void Dlg_SetB1(struct compiler *d, u32 ptr, u32 b) {
   if(b > 0xFF) {
      tb_err(&d->tb, &d->res, "byte error (overflow) %u", nil, _f, b);
      unwrap(&d->res);
   }

   Cps_SetC(d->def.codeV, ptr, b);
}

void Dlg_SetB2(struct compiler *d, u32 ptr, u32 word) {
   Dlg_SetB1(d, ptr + 0, byte(word));      unwrap(&d->res);
   Dlg_SetB1(d, ptr + 1, byte(word >> 8)); unwrap(&d->res);
}

u32 Dlg_PushStr(struct compiler *d, cstr s, u32 l) {
   u32  p = d->def.stabP;
   u32 vl = Cps_Size(p + l) - d->def.stabC;

   if(p + l > STR_END - STR_BEG) {
      tb_err(&d->tb, &d->res, "STR segment overflow", nil, _f);
      unwrap(&d->res);
   }

   Dbg_Log(log_dlg,
           _l(__func__), _l(": ("), _p(l), _c(' '), _p(vl), _l(") '"), _p(s),
           _c('\''));

   d->def.stabC += vl;
   d->def.stabP += l;
   d->def.stabV  = Talloc(d->def.stabV, d->def.stabC, _tag_dlgv);

   for(u32 i = 0; i < l; i++) Cps_SetC(d->def.stabV, p + i, s[i]);

   return STR_BEG + p;
}

void Dlg_GetNamePool(struct compiler *d, i32 which) {
   struct name_pool *pool = &d->nam[which];

   tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_braceo, 0); unwrap(&d->res);

   while(!tb_drop(&d->tb, tok_bracec)) {
      struct token *tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_identi, 0);
      unwrap(&d->res);

      char *s = Malloc(tok->textC + 1, _tag_dlgc);
      faststrcpy(s, tok->textV);

      pool->names = Talloc(pool->names, pool->num_names + 1, _tag_dlgc);
      pool->names[pool->num_names++] = s;

      tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_comma, 0); unwrap(&d->res);
   }
}

i32 Dlg_CheckNamePool(struct compiler *d, i32 which, cstr check) {
   struct name_pool *pool = &d->nam[which];

   for(mem_size_t i = 0; i < pool->num_names; ++i) {
      if(faststrchk(pool->names[i], check)) {
         return i;
      }
   }

   return -1;
}

void Dlg_ClearNamePool(struct compiler *d, i32 which) {
   struct name_pool *pool = &d->nam[which];
   for(i32 i = 0; i < pool->num_names; ++i) {
      Dalloc(pool->names[i]);
   }
   Dalloc(pool->names);
   pool->names     = nil;
   pool->num_names = 0;
}

/* EOF */
