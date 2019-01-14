// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// m_token.c: Token-based text parsing.

#include "m_token.h"
#include "m_vec.h"
#include "m_char.h"
#include "m_file.h"

#define textNext() Vec_Grow(tok->text, 1), Vec_Next(tok->text)

#define tokText(fn) \
   do { \
      for(i32 pc = 0; fn(ch); pc = ch, getch()) { \
         advLine(); \
         textNext() = ch; \
      } \
      textNext() = '\0'; \
      unget(); \
   } while(0)

#define InComment(ch) ((ch) != '\n' && !FEOF(fp))

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

void TokParse(FILE *fp, struct token *tok, struct origin *orig)
{
   if(!tok || !fp || !orig) return;

begin:;
   i32 ch;

   Vec_Clear(tok->text);

   getch();
   if(FEOF(fp) || ch == EOF) {
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
      else if(IsDigit(ch)) {unget(); ch = '-'; break;}
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

         for(i32 lvl = 1; lvl && !FEOF(fp); getch())
         {
            if(ch == '/') {
               if(getch() == '*') {lvl++; continue;}
               else               {unget(); ch = '/';}
            } else if(ch == '*') {
               if(getch() == '/') {lvl--; continue;}
               else               {unget(); ch = '*';}
            }

            advLine();
            textNext() = ch;
         }

         unget();
      }
      else
         {unget(); tok1(tok_div);}
      return;

   case '.':
      if(getch() == '.')
         tok2('.', tok_dot2, tok_dot3);
      else if(IsDigit(ch))
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
   string:
      for(i32 i = 0, beg = ch; getch() != beg && !FEOF(fp);)
         textNext() = ch;
      textNext() = '\0';
      return;
   }

   if(IsBlank(ch))
   {
      while(ch = getch(), IsBlank(ch));
      unget();
      goto begin;
   }
   else if(IsDigit(ch) || ch == '.' || ch == '-')
   {
      tok1(tok_number);
      textNext() = ch;
      getch();
      tokText(IsNumId);
   }
   else if(IsIdent(ch))
   {
      tok1(tok_identi);
      tokText(IsIdent);
   }
   else
   {
      tok1(tok_chrseq);
      textNext() = ch;
      textNext() = '\0';
   }
}

// EOF
