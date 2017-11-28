// Copyright © 2017 Graham Sanderson, all rights reserved.
#include "lith_token.h"
#include "lith_darray.h"

#include <ctype.h>

#define GetPredStr(fn) \
   do { \
      for(; fn(ch); ch = fgetc(fp)) { \
         Vec_Grow(tok->text, 1); \
         Vec_Next(tok->text) = ch; \
      } \
      Vec_Grow(tok->text, 1); \
      Vec_Next(tok->text) = '\0'; \
      ungetc(ch, fp); \
   } while(0)

#define IsIdenti(ch) (isalnum(ch) || (ch) == '_')
#define IsNum(ch)    (isalnum(ch) || (ch) == '_' || (ch) == '.')

// Extern Functions ----------------------------------------------------------|

//
// Lith_ParseToken
//
void Lith_ParseToken(FILE *fp, token_t *tok)
{
   if(!tok) return;

   Vec_Clear(tok->text);

   if(!fp || feof(fp)) {
      tok->type = tok_eof;
      return;
   }

begin:;
   int ch;
   switch((ch = fgetc(fp)))
   {
   case EOF:
   case '\0': tok->type = tok_eof;    return;
   case '\r': if((ch = fgetc(fp)) != '\n') ungetc(ch, fp);
   case '\n': tok->type = tok_lnend;  return;
   case ';':  tok->type = tok_semico; return;
   case ',':  tok->type = tok_comma;  return;
   case '$':  tok->type = tok_dollar; return;
   case '[':  tok->type = tok_bracko; return;
   case ']':  tok->type = tok_brackc; return;
   case '{':  tok->type = tok_braceo; return;
   case '}':  tok->type = tok_bracec; return;
   case '(':  tok->type = tok_pareno; return;
   case ')':  tok->type = tok_parenc; return;
   #define TokOp2(c, c2, t, t2) \
   case c: \
      if((ch = fgetc(fp)) == c2) tok->type = t2; \
      else {ungetc(ch, fp); tok->type = t;} \
      return
   TokOp2('=', '=', tok_eq,   tok_eq2);
   TokOp2('?', '=', tok_tern, tok_terneq);
   TokOp2('!', '=', tok_not,  tok_neq);
   TokOp2('~', '=', tok_bnot, tok_bneq);
   TokOp2('*', '=', tok_mul,  tok_muleq);
   TokOp2('@', '@', tok_at,   tok_at2);
   #undef TokOp2
   #define TokOp3(c, t, t2, teq) \
   case c: \
           if((ch = fgetc(fp)) == c) tok->type = t2; \
      else if(ch == '=')             tok->type = teq; \
      else {ungetc(ch, fp); tok->type = t;} \
      return
   TokOp3('<', tok_lt,  tok_lt2,  tok_le);
   TokOp3('>', tok_gt,  tok_gt2,  tok_ge);
   TokOp3('|', tok_or,  tok_or2,  tok_oreq);
   TokOp3('&', tok_and, tok_and2, tok_andeq);
   TokOp3('+', tok_add, tok_add2, tok_addeq);
   TokOp3('%', tok_mod, tok_mod2, tok_modeq);
   TokOp3('^', tok_xor, tok_xor2, tok_xoreq);
   TokOp3(':', tok_col, tok_col2, tok_coleq);
   #undef TokOp3
   case '-':
           if((ch = fgetc(fp)) == '-') tok->type = tok_sub2;
      else if(ch == '=')               tok->type = tok_subeq;
      else if(ch == '>')               tok->type = tok_rarrow;
      else if(isdigit(ch)) {ungetc(ch, fp); ch = '-'; break;}
      else {ungetc(ch, fp); tok->type = tok_sub;}
      return;
   case '/':
      if((ch = fgetc(fp)) == '=')
         tok->type = tok_diveq;
      else if(ch == '/')
      {
         #define InComment(ch) ((ch) != '\n' && !feof(fp))
         ch = fgetc(fp);
         GetPredStr(InComment);
         tok->type = tok_cmtlin;
         #undef InComment
      }
      else
         {ungetc(ch, fp); tok->type = tok_div;}
      return;
   case '.':
      if((ch = fgetc(fp)) == '.')
      {
         if((ch = fgetc(fp)) == '.')
            tok->type = tok_dot3;
         else
            {ungetc(ch, fp); tok->type = tok_dot2;}
      }
      else if(isdigit(ch))
         {ungetc(ch, fp); break;}
      else
         {ungetc(ch, fp); tok->type = tok_dot;}
      return;
   case 0xFFFFFFE2:
      if((ch = fgetc(fp)) == 0xFFFFFF80)
      {
         if((ch = fgetc(fp)) == 0xFFFFFF9C)
         {
            #define InQuote(ch) ((ch) != '\n' && !feof(fp))
            ch = fgetc(fp);
            GetPredStr(InQuote);
            tok->type = tok_quote;
            #undef InQuote
            return;
         }
         else
            ungetc('\x9c', fp);
      }
      ungetc('\x80', fp);
      break;
   case '\'': tok->type = tok_charac; goto string;
   case '"':  tok->type = tok_string; goto string;
   string: {
      int i, beg;
      for(i = 0, beg = ch; (ch = fgetc(fp)) != beg && !feof(fp);) {
         Vec_Grow(tok->text, 1);
         Vec_Next(tok->text) = ch;
      }
      Vec_Grow(tok->text, 1);
      Vec_Next(tok->text) = '\0';
      return;
   }
   }

   if(isblank(ch))
   {
      while(isblank(ch = fgetc(fp)));
      ungetc(ch, fp);
      goto begin;
   }
   else if(isdigit(ch) || ch == '.' || ch == '-')
   {
      Vec_Grow(tok->text, 1);
      Vec_Next(tok->text) = ch;
      ch = fgetc(fp);
      GetPredStr(IsNum);
      tok->type = tok_number;
   }
   else if(IsIdenti(ch))
   {
      GetPredStr(IsIdenti);
      tok->type = tok_identi;
   }
   else
   {
      Vec_Grow(tok->text, 2);
      Vec_Next(tok->text) = ch;
      Vec_Next(tok->text) = '\0';
      tok->type = tok_chrseq;
   }
}

// EOF
