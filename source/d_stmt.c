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

/* Extern Functions -------------------------------------------------------- */

void Dlg_GetStmt_Cond(struct compiler *d) {
   struct token *tok = d->tb.expc(&d->res, d->tb.get(), tok_identi, 0);
   unwrap(&d->res);

   bool bne = true;

   switch(Dlg_CondName(tok->textV)) {
   case _dlg_cond_item:
      tok = d->tb.expc(&d->res, d->tb.get(), tok_string, 0);
      unwrap(&d->res);

      bne = false;

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
   if(TokIsKw(tok, "else")) {
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

void Dlg_GetStmt_Option(struct compiler *d) {
   struct ptr2 adr;

   struct token *tok = d->tb.expc(&d->res, d->tb.get(), tok_identi, tok_string, 0);
   unwrap(&d->res);

   u32 s = Dlg_PushStr(d, tok->textV, tok->textC);
   unwrap(&d->res);
   Dlg_PushLdAdr(d, VAR_ADRL, s); unwrap(&d->res);
   adr = Dlg_PushLdAdr(d, VAR_RADRL, 0); unwrap(&d->res); /* placeholder */

   Dlg_PushLdVA(d, ACT_LD_OPT); unwrap(&d->res);

   Dlg_PushB1(d, DCD_JMP_AI); unwrap(&d->res);
   Dlg_PushB2(d, 0); unwrap(&d->res); /* placeholder */

   u32 ptr = d->def.codeP;
   u32 rel = PRG_BEG + ptr;

   Dlg_SetB1(d, adr.l, rel & 0xFF); unwrap(&d->res);
   Dlg_SetB1(d, adr.h, rel >> 8); unwrap(&d->res);

   Dlg_GetStmt(d); unwrap(&d->res);

   Dlg_SetB2(d, ptr - 2, PRG_BEG + d->def.codeP); unwrap(&d->res);
}

void Dlg_GetStmt_Page(struct compiler *d) {
   struct token *tok = d->tb.expc(&d->res, d->tb.get(), tok_number, 0);
   unwrap(&d->res);
   Dlg_PushB1(d, DCD_JPG_VI); unwrap(&d->res);
   Dlg_PushB1(d, faststrtoi32(tok->textV)); unwrap(&d->res);

   tok = d->tb.expc(&d->res, d->tb.get(), tok_semico, 0);
   unwrap(&d->res);
}

void Dlg_GetStmt_Str(struct compiler *d, u32 adr) {
   struct token *tok =
      d->tb.expc(&d->res, d->tb.get(), tok_identi, tok_string, tok_number, 0);
   unwrap(&d->res);
   u32 s = Dlg_PushStr(d, tok->textV, tok->textC);
   unwrap(&d->res);
   Dlg_PushLdAdr(d, adr, s); unwrap(&d->res);

   tok = d->tb.expc(&d->res, d->tb.get(), tok_semico, 0);
   unwrap(&d->res);
}

void Dlg_GetStmt_Terminal(struct compiler *d, u32 act) {
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
   Dlg_PushB1(d, act & 0xFF);     unwrap(&d->res);
   Dlg_PushB1(d, DCD_STA_AI);     unwrap(&d->res);
   Dlg_PushB2(d, VAR_TACT);       unwrap(&d->res);
   Dlg_PushLdVA(d, ACT_TRM_WAIT); unwrap(&d->res);
}

void Dlg_GetStmt_Finale(struct compiler *d, u32 act) {
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
   Dlg_PushLdAdr(d, VAR_ADRL, faststrtoi32(tok->textV) & 0xFFFF); unwrap(&d->res);

   Dlg_PushB1(d, DCD_LDA_VI);     unwrap(&d->res);
   Dlg_PushB1(d, act & 0xFF);     unwrap(&d->res);
   Dlg_PushB1(d, DCD_STA_AI);     unwrap(&d->res);
   Dlg_PushB2(d, VAR_FACT);       unwrap(&d->res);
   Dlg_PushLdVA(d, ACT_FIN_WAIT); unwrap(&d->res);
}

void Dlg_GetStmt_Num(struct compiler *d, u32 act) {
   struct token *tok = d->tb.expc(&d->res, d->tb.get(), tok_number, 0);
   unwrap(&d->res);

   Dlg_PushLdAdr(d, VAR_ADRL, faststrtoi32(tok->textV) & 0xFFFF); unwrap(&d->res);
   Dlg_PushLdVA(d, act); unwrap(&d->res);

   tok = d->tb.expc(&d->res, d->tb.get(), tok_semico, 0);
   unwrap(&d->res);
}

void Dlg_GetStmt_Script(struct compiler *d) {
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

void Dlg_GetStmt_Block(struct compiler *d) {
   while(!d->tb.drop(tok_bracec)) {
      Dlg_GetStmt(d); unwrap(&d->res);
   }
}

script void Dlg_GetStmt(struct compiler *d) {
   struct token *tok = d->tb.get();

   switch(tok->type) {
   case tok_braceo:
      Dlg_GetStmt_Block(d);
      unwrap(&d->res);
      break;
   case tok_identi: {
      Dbg_Log(log_dlg, "%s: %s", __func__, tok->textV);

      switch(Dlg_StmtName(tok->textV)) {
      /* conditionals */
      case _dlg_stmt_if:     Dlg_GetStmt_Cond(d);   break;
      case _dlg_stmt_option: Dlg_GetStmt_Option(d); break;

      /* terminals */
      case _dlg_stmt_logon:  Dlg_GetStmt_Terminal(d, TACT_LOGON);  break;
      case _dlg_stmt_logoff: Dlg_GetStmt_Terminal(d, TACT_LOGOFF); break;
      case _dlg_stmt_pict:   Dlg_GetStmt_Terminal(d, TACT_PICT);   break;
      case _dlg_stmt_info:   Dlg_GetStmt_Terminal(d, TACT_INFO);   break;

      /* finales */
      case _dlg_stmt_fade_in:  Dlg_GetStmt_Finale(d, FACT_FADE_IN);  break;
      case _dlg_stmt_fade_out: Dlg_GetStmt_Finale(d, FACT_FADE_OUT); break;
      case _dlg_stmt_wait:     Dlg_GetStmt_Finale(d, FACT_WAIT);     break;
      case _dlg_stmt_mus_fade: Dlg_GetStmt_Finale(d, FACT_MUS_FADE); break;
      case _dlg_stmt_crawl:    Dlg_GetStmt_Finale(d, FACT_CRAWL);    break;

      /* general */
      case _dlg_stmt_page:   Dlg_GetStmt_Page(d);   break;
      case _dlg_stmt_script: Dlg_GetStmt_Script(d); break;
      case _dlg_stmt_teleport_interlevel:
         Dlg_GetStmt_Num(d, ACT_TELEPORT_INTERLEVEL);
         break;
      case _dlg_stmt_teleport_intralevel:
         Dlg_GetStmt_Num(d, ACT_TELEPORT_INTRALEVEL);
         break;

      /* strings */
      case _dlg_stmt_name:   Dlg_GetStmt_Str(d, VAR_NAMEL);   break;
      case _dlg_stmt_icon:   Dlg_GetStmt_Str(d, VAR_ICONL);   break;
      case _dlg_stmt_remote: Dlg_GetStmt_Str(d, VAR_REMOTEL); break;
      case _dlg_stmt_text:   Dlg_GetStmt_Str(d, VAR_TEXTL);   break;
      case _dlg_stmt_music:  Dlg_GetStmt_Str(d, VAR_MUSICL);  break;
      case _dlg_stmt_image:  Dlg_GetStmt_Str(d, VAR_PICTL);   break;

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
