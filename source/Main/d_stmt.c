/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Dialogue compiler statements.
 *
 * ---------------------------------------------------------------------------|
 */

#if LITHIUM
#include "d_compile.h"

/* Extern Functions -------------------------------------------------------- */

void Dlg_GetStmt_Text(struct compiler *d, struct token *tok, u32 act)
{
   Dlg_PushLdAdr(d, VAR_ADRL, Dlg_PushStr(d, tok->textV, tok->textC));
   Dlg_PushLdVA(d, act);
}

void Dlg_GetStmt_Cond(struct compiler *d)
{
   struct token *tok = d->tb.get();
   Expect(d, tok, tok_identi);

   u32 ins = DCD_BNE_RI;

   if(faststrcmp(tok->textV, "item") == 0) {
      tok = d->tb.get();
      Expect(d, tok, tok_string);

      ins = DCD_BEQ_RI;

      Dlg_PushLdAdr(d, VAR_ADRL, Dlg_PushStr(d, tok->textV, tok->textC));

      Dlg_PushLdVA(d, ACT_LD_ITEM);
   } else if(faststrcmp(tok->textV, "class") == 0) {
      tok = d->tb.get();
      Expect(d, tok, tok_identi);

      Dlg_PushB1(d, DCD_LDA_AI);
      Dlg_PushB2(d, VAR_PCLASS);

      Dlg_PushB1(d, DCD_CMP_VI);
      #define LITH_X(shr, lng) \
         if(faststrcmp(tok->textV, #shr) == 0) {Dlg_PushB1(d, shr); goto ok;}
      #include "p_player.h"
      ErrT(d, tok, "invalid playerclass type");
   } else {
      ErrT(d, tok, "invalid conditional type");
   }

ok:
   Dlg_PushB1(d, ins);
   Dlg_PushB1(d, 0); /* placeholder */
   u32 ptr = d->def.codeP;

   Dlg_GetStmt(d);

   u32 rel = d->def.codeP - ptr;
   if(rel > 0x7F) Err(d, "bad jump (too much code)");

   Dlg_SetB1(d, ptr - 1, rel);

   tok = d->tb.get();
   if(CheckKw(tok, "else")) {
      Dlg_PushB1(d, DCD_JMP_AI);
      Dlg_PushB2(d, 0);
      ptr = d->def.codeP;

      Dlg_GetStmt(d);

      Dlg_SetB2(d, ptr - 2, PRG_BEG + d->def.codeP);
   } else {
      d->tb.unget();
   }
}

void Dlg_GetStmt_Option(struct compiler *d)
{
   struct ptr2 adr;

   struct token *tok = d->tb.get();
   Expect2(d, tok, tok_identi, tok_string);

   Dlg_PushLdAdr(d, VAR_ADRL, Dlg_PushStr(d, tok->textV, tok->textC));
   adr = Dlg_PushLdAdr(d, VAR_RADRL, 0); /* placeholder */

   Dlg_PushLdVA(d, ACT_LD_OPT);

   Dlg_PushB1(d, DCD_JMP_AI);
   Dlg_PushB2(d, 0); /* placeholder */

   u32 ptr = d->def.codeP;
   u32 rel = PRG_BEG + ptr;

   Dlg_SetB1(d, adr.l, rel & 0xFF);
   Dlg_SetB1(d, adr.h, rel >> 8);

   Dlg_GetStmt(d);

   Dlg_SetB2(d, ptr - 2, PRG_BEG + d->def.codeP);
}

void Dlg_GetStmt_Page(struct compiler *d)
{
   struct token *tok = d->tb.get();
   Expect(d, tok, tok_number);
   Dlg_PushB1(d, DCD_JPG_VI);
   Dlg_PushB1(d, strtoi(tok->textV, nil, 0));
}

void Dlg_GetStmt_Str(struct compiler *d, u32 adr)
{
   struct token *tok = d->tb.get();
   Expect3(d, tok, tok_identi, tok_string, tok_number);

   Dlg_PushLdAdr(d, adr, Dlg_PushStr(d, tok->textV, tok->textC));
}

void Dlg_GetStmt_Script(struct compiler *d)
{
   struct token *tok = d->tb.get();
   Expect2(d, tok, tok_string, tok_number);

   u32 act;

   if(tok->type == tok_string) {
      act = ACT_SCRIPT_S;
      Dlg_PushLdAdr(d, VAR_ADRL, Dlg_PushStr(d, tok->textV, tok->textC));
   } else {
      u32 prm = strtoi(tok->textV, nil, 0);

      act = ACT_SCRIPT_I;

      Dlg_PushB1(d, DCD_LDA_VI);
      Dlg_PushB1(d, prm);

      Dlg_PushB1(d, DCD_STA_AI);
      Dlg_PushB2(d, VAR_SCP0);
   }

   for(u32 i = 0; i < 4 && d->tb.drop(tok_comma); i++) {
      tok = d->tb.get();
      Expect(d, tok, tok_number);

      u32 prm = strtoi(tok->textV, nil, 0);

      Dlg_PushB1(d, DCD_LDA_VI);
      Dlg_PushB1(d, prm);

      Dlg_PushB1(d, DCD_STA_AI);
      Dlg_PushB2(d, VAR_SCP1 + i);
   }

   Dlg_PushLdVA(d, act);
}

void Dlg_GetStmt_Block(struct compiler *d)
{
   while(!d->tb.drop(tok_bracec)) Dlg_GetStmt(d);
}

void Dlg_GetStmt_Concat(struct compiler *d)
{
   Dlg_PushB1(d, DCD_INC_AI);
   Dlg_PushB2(d, VAR_CONCAT);

   while(!d->tb.drop(tok_at2)) Dlg_GetStmt(d);

   Dlg_PushB1(d, DCD_DEC_AI);
   Dlg_PushB2(d, VAR_CONCAT);
}

script void Dlg_GetStmt(struct compiler *d)
{
   struct token *tok = d->tb.get();

   switch(tok->type) {
      case tok_braceo:
         Dlg_GetStmt_Block(d);
         break;
      case tok_at2:
         Dlg_GetStmt_Concat(d);
         break;
      case tok_identi:
         Dbg_Log(log_dlg, "%s: %s", __func__, tok->textV);

         if(faststrcmp(tok->textV, "if") == 0)
            Dlg_GetStmt_Cond(d);
         else if(faststrcmp(tok->textV, "option") == 0)
            Dlg_GetStmt_Option(d);
         else if(faststrcmp(tok->textV, "page") == 0)
            Dlg_GetStmt_Page(d);
         else if(faststrcmp(tok->textV, "name") == 0)
            Dlg_GetStmt_Str(d, VAR_NAMEL);
         else if(faststrcmp(tok->textV, "icon") == 0)
            Dlg_GetStmt_Str(d, VAR_ICONL);
         else if(faststrcmp(tok->textV, "remote") == 0)
            Dlg_GetStmt_Str(d, VAR_REMOTEL);
         else if(faststrcmp(tok->textV, "pict") == 0)
            Dlg_GetStmt_Str(d, VAR_PICTL);
         else if(faststrcmp(tok->textV, "script") == 0)
            Dlg_GetStmt_Script(d);
         else
            Dlg_GetStmt_Asm(d);

         break;
      case tok_quote:
         Dlg_GetStmt_Text(d, tok, ACT_TEXT_ADDI);
         break;
      case tok_hash:
         tok = d->tb.get();
         Expect2(d, tok, tok_identi, tok_string);
         Dlg_GetStmt_Text(d, tok, ACT_TEXT_ADDL);
         break;
      case tok_lt:
         while((tok = d->tb.get())->type != tok_gt) {
            Expect(d, tok, tok_number);
            Dlg_PushB1(d, strtoi(tok->textV, nil, 0));
         }
      case tok_semico:
         break;
      default:
         ErrT(d, tok, "invalid token in statement");
         break;
   }
}

#endif

/* EOF */
