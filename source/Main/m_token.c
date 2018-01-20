// Copyright © 2017 Graham Sanderson, all rights reserved.
#include "lith_token.h"
#include "lith_darray.h"

#include <ctype.h>

#define tokText(fn) \
   do { \
      for(int pc = 0; fn(ch); pc = ch, getch()) { \
         advLine(); \
         Vec_Grow(tok->text, 1); \
         Vec_Next(tok->text) = ch; \
      } \
      Vec_Grow(tok->text, 1); \
      Vec_Next(tok->text) = '\0'; \
      unget(); \
   } while(0)

#define IsIdenti(ch) (isalnum(ch) || (ch) == '_')
#define IsNum(ch)    (isalnum(ch) || (ch) == '_' || (ch) == '.')

#define InComment(ch) ((ch) != '\n' && !feof(fp))

#define unget() ((orig->colu -= 1), ungetc(ch, fp))
#define getch() ((orig->colu += 1), ch = fgetc(fp))

#define advLine() \
   switch(ch) { \
   case '\r': if(getch() != '\n') unget(); \
   case '\n': orig->colu = 0; ++orig->line; \
   }

//#define tok1(tt1) (printf(#tt1 "\n"), tok->type = tt1)
#define tok1(tt1) (tok->orig = *orig, tok->type = tt1)
#define tok2(c2, tt1, tt2) \
   if(getch() == c2) tok1(tt2); \
   else    (unget(), tok1(tt1))
#define tok3(c2, c3, tt1, tt2, tt3) \
        if(getch() == c3) tok1(tt3); \
   else if(   ch   == c2) tok1(tt2); \
   else         (unget(), tok1(tt1))

// Extern Functions ----------------------------------------------------------|

//
// Lith_ParseToken
//
void Lith_ParseToken(FILE *fp, token_t *tok, origin_t *orig)
{
   if(!tok || !fp || !orig) return;

begin:;
   int ch;

   Vec_Clear(tok->text);

   getch();
   if(feof(fp) || ch == EOF) {
      unget();
      tok1(tok_eof);
      return;
   }

   switch(ch)
   {
   // Whitespace
   case '\r': case '\n': tok1(tok_lnend); advLine(); return;

   // 1-op tokens
   case ';': tok1(tok_semico); return;
   case ',': tok1(tok_comma ); return;
   case '$': tok1(tok_dollar); return;
   case '[': tok1(tok_bracko); return;
   case ']': tok1(tok_brackc); return;
   case '{': tok1(tok_braceo); return;
   case '}': tok1(tok_bracec); return;
   case '(': tok1(tok_pareno); return;
   case ')': tok1(tok_parenc); return;

   // 2-op tokens
   case '=': tok2('=', tok_eq,   tok_eq2   ); return;
   case '?': tok2('=', tok_tern, tok_terneq); return;
   case '!': tok2('=', tok_not,  tok_neq   ); return;
   case '~': tok2('=', tok_bnot, tok_bneq  ); return;
   case '*': tok2('=', tok_mul,  tok_muleq ); return;
   case '@': tok2('@', tok_at,   tok_at2   ); return;

   // 3-op tokens
   case '<': tok3('<', '=', tok_lt,  tok_lt2,  tok_le   ); return;
   case '>': tok3('>', '=', tok_gt,  tok_gt2,  tok_ge   ); return;
   case '|': tok3('|', '=', tok_or,  tok_or2,  tok_oreq ); return;
   case '&': tok3('&', '=', tok_and, tok_and2, tok_andeq); return;
   case '+': tok3('+', '=', tok_add, tok_add2, tok_addeq); return;
   case '%': tok3('%', '=', tok_mod, tok_mod2, tok_modeq); return;
   case '^': tok3('^', '=', tok_xor, tok_xor2, tok_xoreq); return;
   case ':': tok3(':', '=', tok_col, tok_col2, tok_coleq); return;

   case '-':
           if(getch() == '-')        tok1(tok_sub2);
      else if(ch == '=')             tok1(tok_subeq);
      else if(ch == '>')             tok1(tok_rarrow);
      else if(isdigit(ch)) {unget(); ch = '-'; break;}
      else                 {unget(); tok1(tok_sub);}
      return;

   case '/':
      if(getch() == '=')
         tok1(tok_diveq);
      else if(ch == '/')
      {
         tok1(tok_cmtlin);
         getch();
         tokText(InComment);
      }
      else if(ch == '*')
      {
         tok1(tok_cmtblk);
         getch();

         for(int lvl = 1; lvl && !feof(fp); getch())
         {
            if(ch == '/') {
               if(getch() == '*') {lvl++; continue;}
               else               {unget(); ch = '/';}
            } else if(ch == '*') {
               if(getch() == '/') {lvl--; continue;}
               else               {unget(); ch = '*';}
            }

            advLine();

            Vec_Grow(tok->text, 1);
            Vec_Next(tok->text) = ch;
         }

         unget();
      }
      else
         {unget(); tok1(tok_div);}
      return;

   case '.':
      if(getch() == '.')
         tok2('.', tok_dot2, tok_dot3);
      else if(isdigit(ch))
         {unget(); break;}
      else
         {unget(); tok1(tok_dot);}
      return;

   case '`':
      getch();
      tokText(InComment);
      tok1(tok_quote);
      return;

   case '\'': tok1(tok_charac); goto string;
   case '"':  tok1(tok_string); goto string;
   string: {
      int i, beg;
      for(i = 0, beg = ch; getch() != beg && !feof(fp);) {
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
      while(isblank(getch()));
      unget();
      goto begin;
   }
   else if(isdigit(ch) || ch == '.' || ch == '-')
   {
      tok1(tok_number);
      Vec_Grow(tok->text, 1);
      Vec_Next(tok->text) = ch;
      getch();
      tokText(IsNum);
   }
   else if(IsIdenti(ch))
   {
      tok1(tok_identi);
      tokText(IsIdenti);
   }
   else
   {
      tok1(tok_chrseq);
      Vec_Grow(tok->text, 2);
      Vec_Next(tok->text) = ch;
      Vec_Next(tok->text) = '\0';
   }
}

// EOF
