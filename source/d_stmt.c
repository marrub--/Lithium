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

static mem_size_t PrefixedText(struct compiler *d, cstr text) {
   char *p = txtbuf;
   if(d->name[0]) {
      faststrpcpy(&p, d->name);
      *p++ = '_';
   }
   faststrpcpy(&p, text);
   ++p;
   return (mem_size_t)(p - txtbuf);
}

static void PushFirmwareAction(struct compiler *d, i32 act) {
   Dlg_PushB1(d, DCD_LDA_VI); unwrap(&d->res);
   Dlg_PushB1(d, byte(act));  unwrap(&d->res);
   Dlg_PushB1(d, DCD_STA_AI); unwrap(&d->res);
   Dlg_PushB2(d, VAR_FRMAC);  unwrap(&d->res);
   Dlg_PushLdVA(d, ACT_WAIT); unwrap(&d->res);
}

static void Dlg_Stmt_Conditional(struct compiler *d) {
   struct token *tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_identi, tok_not, 0);
   unwrap(&d->res);
   bool bne = true;
   if(tok->type == tok_not) {
      bne = !bne;
      tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_identi, 0);
      unwrap(&d->res);
   }
   switch(Dlg_CondName(tok->textV)) {
   case _dlg_cond_item:
      bne = !bne;
      tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_string, 0);
      unwrap(&d->res);
      mem_size_t s = Dlg_PushStr(d, tok->textV, tok->textC); unwrap(&d->res);
      Dlg_PushLdAdr(d, VAR_ADRL, s); unwrap(&d->res);
      Dlg_PushLdVA(d, ACT_LD_ITEM); unwrap(&d->res);
      break;
   case _dlg_cond_class:
      tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_identi, 0);
      unwrap(&d->res);
      Dlg_PushB1(d, DCD_LDA_AI); unwrap(&d->res);
      Dlg_PushB2(d, VAR_PCLASS); unwrap(&d->res);
      Dlg_PushB1(d, DCD_CMP_VI); unwrap(&d->res);
      i32 pcl = P_ClassToInt(tok->textV);
      if(pcl != -1) {
         Dlg_PushB1(d, pcl);
         unwrap(&d->res);
      } else {
         tb_err(&d->tb, &d->res, "invalid playerclass type", tok, _f);
         unwrap(&d->res);
      }
      break;
   case _dlg_cond_deref:
      tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_identi, 0);
      unwrap(&d->res);
      struct compiler_var *v = Dlg_GetVar(d, tok->textV);
      if(!v) {
         tb_err(&d->tb, &d->res, "bad variable name", tok, _f);
         unwrap(&d->res);
      }
      Dlg_PushB1(d, DCD_LDA_AI); unwrap(&d->res);
      Dlg_PushB2(d, v->value);   unwrap(&d->res);
      Dlg_PushB1(d, DCD_CMP_VI); unwrap(&d->res);
      Dlg_PushB1(d, 1);          unwrap(&d->res);
      break;
   default:
      tb_err(&d->tb, &d->res, "invalid conditional type", tok, _f);
      unwrap(&d->res);
   }
   Dlg_PushB1(d, 0); unwrap(&d->res); /* placeholder */
   Dlg_PushB1(d, 0); unwrap(&d->res); /* placeholder */
   mem_size_t ptr = d->def.codeP;
   Dlg_PushB1(d, 0); unwrap(&d->res); /* placeholder */
   Dlg_PushB1(d, 0); unwrap(&d->res); /* placeholder */
   Dlg_PushB1(d, 0); unwrap(&d->res); /* placeholder */
   Dlg_GetStmt(d); unwrap(&d->res);
   bool       use_else = false;
   mem_size_t else_ptr;
   tok = tb_get(&d->tb);
   if(TokIsKw(tok, "else") || TokIsKw(tok, "anu")) {
      Dlg_PushB1(d, DCD_JMP_AI); unwrap(&d->res);
      Dlg_PushB2(d, 0); unwrap(&d->res);
      else_ptr = d->def.codeP;
      use_else = true;
   } else {
      tb_unget(&d->tb);
   }
   mem_size_t rel = d->def.codeP - ptr;
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

static void Dlg_Stmt_Option(struct compiler *d) {
   struct ptr2 adr;
   struct token *tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_identi, tok_string, 0);
   unwrap(&d->res);
   mem_size_t s = Dlg_PushStr(d, txtbuf, PrefixedText(d, tok->textV));
   unwrap(&d->res);
   Dlg_PushLdAdr(d, VAR_ADRL, s); unwrap(&d->res);
   adr = Dlg_PushLdAdr(d, VAR_RADRL, 0); unwrap(&d->res); /* placeholder */
   Dlg_PushLdVA(d, ACT_LD_OPT); unwrap(&d->res);
   Dlg_PushB1(d, DCD_JMP_AI); unwrap(&d->res);
   Dlg_PushB2(d, 0); unwrap(&d->res); /* placeholder */
   mem_size_t ptr = d->def.codeP;
   mem_size_t rel = PRG_BEG + ptr;
   Dlg_SetB1(d, adr.l, byte(rel));      unwrap(&d->res);
   Dlg_SetB1(d, adr.h, byte(rel >> 8)); unwrap(&d->res);
   Dlg_GetStmt(d); unwrap(&d->res);
   Dlg_SetB2(d, ptr - 2, PRG_BEG + d->def.codeP); unwrap(&d->res);
}

static void Dlg_Stmt_JumpPage(struct compiler *d) {
   i32 n = Dlg_Evaluate(d, tok_semico);
   unwrap(&d->res);
   if(n >= DPAGE_MAX || n < 0) {
      tb_err(&d->tb, &d->res, "bad page number %i", nil, _f, n);
      unwrap(&d->res);
   }
   Dlg_PushB1(d, DCD_JPG_VI); unwrap(&d->res);
   Dlg_PushB1(d, n);          unwrap(&d->res);
}

optargs(1) static void Dlg_Stmt_StoreStr(struct compiler *d, mem_size_t adr, bool prefix) {
   struct token *tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_identi, tok_string, tok_number, 0);
   unwrap(&d->res);
   mem_size_t str_adr = Dlg_PushStr(d, !prefix ? tok->textV : txtbuf, !prefix ? tok->textC : PrefixedText(d, tok->textV));
   unwrap(&d->res);
   Dlg_PushLdAdr(d, adr, str_adr); unwrap(&d->res);
   tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_semico, 0);
   unwrap(&d->res);
}

static void Dlg_Stmt_Terminal(struct compiler *d, i32 act) {
   struct token *tok;
   if(act != TACT_INFO) {
      tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_identi, tok_string, tok_number, 0);
      unwrap(&d->res);
      mem_size_t s = Dlg_PushStr(d, tok->textV, tok->textC);
      unwrap(&d->res);
      Dlg_PushLdAdr(d, VAR_PICTL, s); unwrap(&d->res);
   }
   Dlg_GetStmt(d); unwrap(&d->res);
   PushFirmwareAction(d, act);
}

static void Dlg_Stmt_Finale(struct compiler *d, i32 act) {
   struct token *tok;
   if(act == FACT_CRAWL) {
      tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_identi, tok_string, 0);
      unwrap(&d->res);
      mem_size_t s = Dlg_PushStr(d, txtbuf, PrefixedText(d, tok->textV));
      unwrap(&d->res);
      Dlg_PushLdAdr(d, VAR_TXT1L, s); unwrap(&d->res);
   }
   tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_number, 0);
   unwrap(&d->res);
   Dlg_PushLdAdr(d, VAR_ADRL, word(faststrtoi32(tok->textV))); unwrap(&d->res);
   PushFirmwareAction(d, act);
}

static void Dlg_Stmt_ActionLoadNum(struct compiler *d, i32 act) {
   struct token *tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_number, 0);
   unwrap(&d->res);
   Dlg_PushLdAdr(d, VAR_ADRL, word(faststrtoi32(tok->textV))); unwrap(&d->res);
   Dlg_PushLdVA(d, act); unwrap(&d->res);
   tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_semico, 0);
   unwrap(&d->res);
}

static void Dlg_Stmt_Script(struct compiler *d) {
   struct token *tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_string, tok_number, 0);
   unwrap(&d->res);
   i32 act;
   if(tok->type == tok_string) {
      act = ACT_SCRIPT_S;
      mem_size_t s = Dlg_PushStr(d, tok->textV, tok->textC);
      unwrap(&d->res);
      Dlg_PushLdAdr(d, VAR_ADRL, s); unwrap(&d->res);
   } else {
      i32 prm = faststrtoi32(tok->textV);
      act = ACT_SCRIPT_I;
      Dlg_PushB1(d, DCD_LDA_VI); unwrap(&d->res);
      Dlg_PushB1(d, prm); unwrap(&d->res);
      Dlg_PushB1(d, DCD_STA_AI); unwrap(&d->res);
      Dlg_PushB2(d, VAR_SCP0); unwrap(&d->res);
   }
   bool do_continue = !tb_drop(&d->tb, tok_semico);
   for(i32 i = 0; i < 4; i++) {
      i32 prm = do_continue ? Dlg_Evaluate(d, tok_semico, tok_comma, &do_continue) : 0;
      unwrap(&d->res);
      Dlg_PushB1(d, DCD_LDA_VI); unwrap(&d->res);
      Dlg_PushB1(d, prm); unwrap(&d->res);
      Dlg_PushB1(d, DCD_STA_AI); unwrap(&d->res);
      Dlg_PushB2(d, VAR_SCP1 + i); unwrap(&d->res);
   }
   Dlg_PushLdVA(d, act); unwrap(&d->res);
}

static void Dlg_Stmt_Block(struct compiler *d) {
   while(!tb_drop(&d->tb, tok_bracec)) {
      unwrap(&d->res);
      Dlg_GetStmt(d); unwrap(&d->res);
   }
}

script void Dlg_GetStmt(struct compiler *d) {
   struct token *tok = tb_get(&d->tb);
   switch(tok->type) {
   case tok_braceo: Dlg_Stmt_Block(d); unwrap(&d->res); break;
   case tok_identi: {
      Dbg_Log(log_gsinfo, _l(_f), _l(": "), _p((cstr)tok->textV));
      switch(Dlg_StmtName(tok->textV)) {
      /* conditionals */
      case _dlg_stmt_if:     Dlg_Stmt_Conditional(d); break;
      case _dlg_stmt_option: Dlg_Stmt_Option(d); break;
      /* terminals */
      case _dlg_stmt_logon:  Dlg_Stmt_Terminal(d, TACT_LOGON);  break;
      case _dlg_stmt_logoff: Dlg_Stmt_Terminal(d, TACT_LOGOFF); break;
      case _dlg_stmt_pict:   Dlg_Stmt_Terminal(d, TACT_PICT);   break;
      case _dlg_stmt_info:   Dlg_Stmt_Terminal(d, TACT_INFO);   break;
      /* finales */
      case _dlg_stmt_fade_in:  Dlg_Stmt_Finale(d, FACT_FADE_IN);  break;
      case _dlg_stmt_fade_out: Dlg_Stmt_Finale(d, FACT_FADE_OUT); break;
      case _dlg_stmt_hold:     Dlg_Stmt_Finale(d, FACT_WAIT);     break;
      case _dlg_stmt_mus_fade: Dlg_Stmt_Finale(d, FACT_MUS_FADE); break;
      case _dlg_stmt_crawl:    Dlg_Stmt_Finale(d, FACT_CRAWL);    break;
      /* general */
      case _dlg_stmt_jump_page: Dlg_Stmt_JumpPage(d); break;
      case _dlg_stmt_script:    Dlg_Stmt_Script(d); break;
      case _dlg_stmt_teleport_interlevel:
         Dlg_Stmt_ActionLoadNum(d, ACT_TELEPORT_INTERLEVEL);
         break;
      case _dlg_stmt_teleport_intralevel:
         Dlg_Stmt_ActionLoadNum(d, ACT_TELEPORT_INTRALEVEL);
         break;
      case _dlg_stmt_wait: Dlg_PushLdVA(d, ACT_WAIT); break;
      /* strings */
      case _dlg_stmt_icon:   Dlg_Stmt_StoreStr(d, VAR_ICONL);       break;
      case _dlg_stmt_image:  Dlg_Stmt_StoreStr(d, VAR_PICTL);       break;
      case _dlg_stmt_music:  Dlg_Stmt_StoreStr(d, VAR_MUSICL);      break;
      case _dlg_stmt_name:   Dlg_Stmt_StoreStr(d, VAR_NAMEL);       break;
      case _dlg_stmt_remote: Dlg_Stmt_StoreStr(d, VAR_REMOTEL);     break;
      case _dlg_stmt_text:   Dlg_Stmt_StoreStr(d, VAR_TXT1L, true); break;
      case _dlg_stmt_text2:  Dlg_Stmt_StoreStr(d, VAR_TXT2L, true); break;
      /* lastly, try an assembly statement */
      default: Dlg_Stmt_Asm(d); break;
      }
      unwrap(&d->res);
      break;
   }
   case tok_lt:
      while((tok = tb_expc(&d->tb, &d->res, tb_get(&d->tb), tok_number, tok_gt, 0))->type != tok_gt) {
         unwrap(&d->res);
         Dlg_PushB1(d, faststrtoi32(tok->textV)); unwrap(&d->res);
      }
      break;
   case tok_semico:
      break;
   default:
      tb_err(&d->tb, &d->res, "invalid token in statement", tok, _f);
      unwrap(&d->res);
      break;
   }
}

/* EOF */
