// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Dialogue compiler statements.                                            │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "d_compile.h"

noinit static char txtbuf[64];

static mem_size_t prefixed_text(struct compiler *d, cstr text) {
   char *p = txtbuf;
   faststrpcpy(&p, d->nam[d->pool].names[d->name]);
   if(d->page < DPAGE_NORMAL_MAX) {
      *p++ = '_';
      faststrpcpy(&p, d->nam[_name_pool_pages].names[d->page]);
   }
   faststrpcpy(&p, text);
   ++p;
   return (mem_size_t)(p - txtbuf);
}

static void stmt_cond(struct compiler *d) {
   struct token *tok = d->tb.expc(&d->res, d->tb.get(), tok_identi, tok_not, 0);
   unwrap(&d->res);

   bool bne = true;

   if(tok->type == tok_not) {
      bne = !bne;
      tok = d->tb.expc(&d->res, d->tb.get(), tok_identi, 0);
      unwrap(&d->res);
   }

   switch(Dlg_CondName(tok->textV)) {
   case _dlg_cond_item:
      bne = !bne;

      tok = d->tb.expc(&d->res, d->tb.get(), tok_string, 0);
      unwrap(&d->res);

      u32 s = Dlg_PushStr(d, tok->textV, tok->textC); unwrap(&d->res);
      Dlg_PushLdAdr(d, VAR_ADRL, s); unwrap(&d->res);
      Dlg_PushLdVA(d, ACT_LD_ITEM); unwrap(&d->res);
      break;
   case _dlg_cond_class:
      tok = d->tb.expc(&d->res, d->tb.get(), tok_identi, 0);
      unwrap(&d->res);

      Dlg_PushB1(d, DCD_LDA_AI); unwrap(&d->res);
      Dlg_PushB2(d, VAR_PCLASS); unwrap(&d->res);

      Dlg_PushB1(d, DCD_CMP_VI); unwrap(&d->res);
      i32 pcl = P_ClassToInt(tok->textV);
      if(pcl != -1) {
         Dlg_PushB1(d, pcl);
         unwrap(&d->res);
      } else {
         d->tb.err(&d->res, "%s invalid playerclass type", TokPrint(tok));
         unwrap(&d->res);
      }
      break;
   case _dlg_cond_var:
      d->tb.expc(&d->res, d->tb.get(), tok_mod, 0);
      unwrap(&d->res);

      tok = d->tb.expc(&d->res, d->tb.get(), tok_identi, 0);
      unwrap(&d->res);

      i32 n = Dlg_CheckNamePool(d, _name_pool_variables, tok->textV);
      if(n < 0) {
         d->tb.err(&d->res, "bad variable name '%s'", tok->textV);
         unwrap(&d->res);
      }

      Dlg_PushB1(d, DCD_LDA_AI);  unwrap(&d->res);
      Dlg_PushB2(d, VAR_END - n); unwrap(&d->res);

      Dlg_PushB1(d, DCD_CMP_VI); unwrap(&d->res);
      Dlg_PushB1(d, 1);          unwrap(&d->res);
      break;
   default:
      d->tb.err(&d->res, "invalid conditional type", TokPrint(tok));
      unwrap(&d->res);
   }

   Dlg_PushB1(d, 0); unwrap(&d->res); /* placeholder */
   Dlg_PushB1(d, 0); unwrap(&d->res); /* placeholder */
   u32 ptr = d->def.codeP;
   Dlg_PushB1(d, 0); unwrap(&d->res); /* placeholder */
   Dlg_PushB1(d, 0); unwrap(&d->res); /* placeholder */
   Dlg_PushB1(d, 0); unwrap(&d->res); /* placeholder */

   Dlg_GetStmt(d); unwrap(&d->res);

   bool use_else = false;
   u32  else_ptr;

   tok = d->tb.get();
   if(TokIsKw(tok, "else") || TokIsKw(tok, "ante")) {
      Dlg_PushB1(d, DCD_JMP_AI); unwrap(&d->res);
      Dlg_PushB2(d, 0); unwrap(&d->res);
      else_ptr = d->def.codeP;
      use_else = true;
   } else {
      d->tb.unget();
   }

   u32 rel = d->def.codeP - ptr;
   if(rel > 0x7F) {
      /* if A !~ B then jump +3 (continue) */
      Dlg_SetB1(d, ptr - 2, bne ? DCD_BEQ_RI : DCD_BNE_RI); unwrap(&d->res);
      Dlg_SetB1(d, ptr - 1, 3); unwrap(&d->res);
      /* else jump $<end> */
      Dlg_SetB1(d, ptr + 0, DCD_JMP_AI); unwrap(&d->res);
      Dlg_SetB2(d, ptr + 1, PRG_BEG + d->def.codeP); unwrap(&d->res);
   } else {
      /* if A ~ B then jump +<end>
       * else          continue
       */
      Dlg_SetB1(d, ptr - 2, bne ? DCD_BNE_RI : DCD_BEQ_RI); unwrap(&d->res);
      Dlg_SetB1(d, ptr - 1, rel); unwrap(&d->res);
      for(i32 i = 0; i < 3; i++) {
         Dlg_SetB1(d, ptr + i, DCD_NOP_NP); unwrap(&d->res);
      }
   }

   if(use_else) {
      Dlg_GetStmt(d); unwrap(&d->res);
      Dlg_SetB2(d, else_ptr - 2, PRG_BEG + d->def.codeP); unwrap(&d->res);
   }
}

static void stmt_option(struct compiler *d) {
   struct ptr2 adr;

   struct token *tok = d->tb.expc(&d->res, d->tb.get(), tok_identi, tok_string, 0);
   unwrap(&d->res);

   u32 s = Dlg_PushStr(d, txtbuf, prefixed_text(d, tok->textV));
   unwrap(&d->res);
   Dlg_PushLdAdr(d, VAR_ADRL, s); unwrap(&d->res);
   adr = Dlg_PushLdAdr(d, VAR_RADRL, 0); unwrap(&d->res); /* placeholder */

   Dlg_PushLdVA(d, ACT_LD_OPT); unwrap(&d->res);

   Dlg_PushB1(d, DCD_JMP_AI); unwrap(&d->res);
   Dlg_PushB2(d, 0); unwrap(&d->res); /* placeholder */

   u32 ptr = d->def.codeP;
   u32 rel = PRG_BEG + ptr;

   Dlg_SetB1(d, adr.l, byte(rel));      unwrap(&d->res);
   Dlg_SetB1(d, adr.h, byte(rel >> 8)); unwrap(&d->res);

   Dlg_GetStmt(d); unwrap(&d->res);

   Dlg_SetB2(d, ptr - 2, PRG_BEG + d->def.codeP); unwrap(&d->res);
}

static void stmt_page(struct compiler *d) {
   struct token *tok = d->tb.expc(&d->res, d->tb.get(), tok_identi, 0);
   unwrap(&d->res);

   i32 n = Dlg_CheckNamePool(d, _name_pool_pages, tok->textV);
   if(n >= DPAGE_MAX || n < 0) {
      d->tb.err(&d->res, "bad page name '%s'", tok->textV);
      unwrap(&d->res);
   }

   Dlg_PushB1(d, DCD_JPG_VI); unwrap(&d->res);
   Dlg_PushB1(d, n); unwrap(&d->res);

   tok = d->tb.expc(&d->res, d->tb.get(), tok_semico, 0);
   unwrap(&d->res);
}

optargs(1) static void stmt_str(struct compiler *d, u32 adr, bool prefix) {
   struct token *tok =
      d->tb.expc(&d->res, d->tb.get(), tok_identi, tok_string, tok_number, 0);
   unwrap(&d->res);

   u32 s = Dlg_PushStr(
      d,
      !prefix ? tok->textV : txtbuf,
      !prefix ? tok->textC : prefixed_text(d, tok->textV)
   );
   unwrap(&d->res);
   Dlg_PushLdAdr(d, adr, s); unwrap(&d->res);

   tok = d->tb.expc(&d->res, d->tb.get(), tok_semico, 0);
   unwrap(&d->res);
}

static void stmt_terminal(struct compiler *d, u32 act) {
   struct token *tok;
   if(act != TACT_INFO) {
      tok = d->tb.expc(&d->res, d->tb.get(), tok_identi, tok_string, tok_number, 0);
      unwrap(&d->res);
      u32 s = Dlg_PushStr(d, tok->textV, tok->textC);
      unwrap(&d->res);
      Dlg_PushLdAdr(d, VAR_PICTL, s); unwrap(&d->res);
   }

   Dlg_GetStmt(d); unwrap(&d->res);

   Dlg_PushB1(d, DCD_LDA_VI);     unwrap(&d->res);
   Dlg_PushB1(d, byte(act));      unwrap(&d->res);
   Dlg_PushB1(d, DCD_STA_AI);     unwrap(&d->res);
   Dlg_PushB2(d, VAR_TACT);       unwrap(&d->res);
   Dlg_PushLdVA(d, ACT_TRM_WAIT); unwrap(&d->res);
}

static void stmt_finale(struct compiler *d, u32 act) {
   struct token *tok;
   if(act == FACT_CRAWL) {
      tok = d->tb.expc(&d->res, d->tb.get(), tok_identi, tok_string, 0);
      unwrap(&d->res);
      u32 s = Dlg_PushStr(d, tok->textV, tok->textC);
      unwrap(&d->res);
      Dlg_PushLdAdr(d, VAR_TEXTL, s); unwrap(&d->res);
   }

   tok = d->tb.expc(&d->res, d->tb.get(), tok_number, 0);
   unwrap(&d->res);
   Dlg_PushLdAdr(d, VAR_ADRL, word(faststrtoi32(tok->textV))); unwrap(&d->res);

   Dlg_PushB1(d, DCD_LDA_VI);     unwrap(&d->res);
   Dlg_PushB1(d, byte(act));      unwrap(&d->res);
   Dlg_PushB1(d, DCD_STA_AI);     unwrap(&d->res);
   Dlg_PushB2(d, VAR_FACT);       unwrap(&d->res);
   Dlg_PushLdVA(d, ACT_FIN_WAIT); unwrap(&d->res);
}

static void stmt_num(struct compiler *d, u32 act) {
   struct token *tok = d->tb.expc(&d->res, d->tb.get(), tok_number, 0);
   unwrap(&d->res);

   Dlg_PushLdAdr(d, VAR_ADRL, word(faststrtoi32(tok->textV))); unwrap(&d->res);
   Dlg_PushLdVA(d, act); unwrap(&d->res);

   tok = d->tb.expc(&d->res, d->tb.get(), tok_semico, 0);
   unwrap(&d->res);
}

static void stmt_script(struct compiler *d) {
   struct token *tok = d->tb.expc(&d->res, d->tb.get(), tok_string, tok_number, 0);
   unwrap(&d->res);

   u32 act;

   if(tok->type == tok_string) {
      act = ACT_SCRIPT_S;
      u32 s = Dlg_PushStr(d, tok->textV, tok->textC);
      unwrap(&d->res);
      Dlg_PushLdAdr(d, VAR_ADRL, s); unwrap(&d->res);
   } else {
      u32 prm = faststrtoi32(tok->textV);

      act = ACT_SCRIPT_I;

      Dlg_PushB1(d, DCD_LDA_VI); unwrap(&d->res);
      Dlg_PushB1(d, prm); unwrap(&d->res);

      Dlg_PushB1(d, DCD_STA_AI); unwrap(&d->res);
      Dlg_PushB2(d, VAR_SCP0); unwrap(&d->res);
   }

   for(u32 i = 0; i < 4 && d->tb.drop(tok_comma); i++) {
      tok = d->tb.expc(&d->res, d->tb.get(), tok_number, 0);
      unwrap(&d->res);

      u32 prm = faststrtoi32(tok->textV);

      Dlg_PushB1(d, DCD_LDA_VI); unwrap(&d->res);
      Dlg_PushB1(d, prm); unwrap(&d->res);

      Dlg_PushB1(d, DCD_STA_AI); unwrap(&d->res);
      Dlg_PushB2(d, VAR_SCP1 + i); unwrap(&d->res);
   }

   Dlg_PushLdVA(d, act); unwrap(&d->res);

   tok = d->tb.expc(&d->res, d->tb.get(), tok_semico, 0);
   unwrap(&d->res);
}

static void stmt_block(struct compiler *d) {
   while(!d->tb.drop(tok_bracec)) {
      Dlg_GetStmt(d); unwrap(&d->res);
   }
}

script void Dlg_GetStmt(struct compiler *d) {
   struct token *tok = d->tb.get();

   switch(tok->type) {
   case tok_braceo:
      stmt_block(d);
      unwrap(&d->res);
      break;
   case tok_identi: {
      Dbg_Log(log_dlg, _l(__func__), _l(": "), _p((cstr)tok->textV));

      switch(Dlg_StmtName(tok->textV)) {
      /* conditionals */
      case _dlg_stmt_if:     stmt_cond(d);   break;
      case _dlg_stmt_option: stmt_option(d); break;

      /* terminals */
      case _dlg_stmt_logon:  stmt_terminal(d, TACT_LOGON);  break;
      case _dlg_stmt_logoff: stmt_terminal(d, TACT_LOGOFF); break;
      case _dlg_stmt_pict:   stmt_terminal(d, TACT_PICT);   break;
      case _dlg_stmt_info:   stmt_terminal(d, TACT_INFO);   break;

      /* finales */
      case _dlg_stmt_fade_in:  stmt_finale(d, FACT_FADE_IN);  break;
      case _dlg_stmt_fade_out: stmt_finale(d, FACT_FADE_OUT); break;
      case _dlg_stmt_wait:     stmt_finale(d, FACT_WAIT);     break;
      case _dlg_stmt_mus_fade: stmt_finale(d, FACT_MUS_FADE); break;
      case _dlg_stmt_crawl:    stmt_finale(d, FACT_CRAWL);    break;

      /* general */
      case _dlg_stmt_page:   stmt_page(d);   break;
      case _dlg_stmt_script: stmt_script(d); break;
      case _dlg_stmt_teleport_interlevel:
         stmt_num(d, ACT_TELEPORT_INTERLEVEL);
         break;
      case _dlg_stmt_teleport_intralevel:
         stmt_num(d, ACT_TELEPORT_INTRALEVEL);
         break;

      /* strings */
      case _dlg_stmt_icon:   stmt_str(d, VAR_ICONL);       break;
      case _dlg_stmt_image:  stmt_str(d, VAR_PICTL);       break;
      case _dlg_stmt_music:  stmt_str(d, VAR_MUSICL);      break;
      case _dlg_stmt_name:   stmt_str(d, VAR_NAMEL);       break;
      case _dlg_stmt_remote: stmt_str(d, VAR_REMOTEL);     break;
      case _dlg_stmt_text:   stmt_str(d, VAR_TEXTL, true); break;

      default:
         Dlg_GetStmt_Asm(d);
         break;
      }
      unwrap(&d->res);
      break;
   }
   case tok_lt:
      while((tok = d->tb.expc(&d->res, d->tb.get(), tok_number, tok_gt, 0))->type != tok_gt) {
         unwrap(&d->res);
         Dlg_PushB1(d, faststrtoi32(tok->textV)); unwrap(&d->res);
      }
   case tok_semico:
      break;
   default:
      d->tb.err(&d->res, "%s invalid token in statement", TokPrint(tok));
      unwrap(&d->res);
      break;
   }
}

/* EOF */
