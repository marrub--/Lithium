/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Token-based text parsing.
 *
 * ---------------------------------------------------------------------------|
 */

#include "m_token.h"
#include "m_vec.h"
#include "m_char.h"
#include "m_file.h"
#include "m_str.h"
#include <ACS_ZDoom.h>

#define textNext() tok->textV[tok->textC++]

#define tokText(fn) \
   statement({ \
      for(i32 pc = 0; fn(ch); pc = ch, getch()) { \
         advLine(); \
         textNext() = ch; \
      } \
      unget(); \
   })

#define untilEOL(ch) ((ch) != '\n' && !FEOF(fp))

#define unget() ((orig->colu -= 1), ungetc(ch, fp))
#define getch() ((orig->colu += 1), ch = fgetc(fp))

#define advLine() \
   switch(ch) { \
   case '\r': if(getch() != '\n') unget(); \
   case '\n': orig->colu = 0; ++orig->line; \
   }

#define tok1(tt1) (tok->orig = *orig, tok->type = tt1)
#define tok2(c2, tt1, tt2) \
   if(getch() == c2) tok1(tt2); \
   else              (unget(), tok1(tt1))
#define tok3(c2, c3, tt1, tt2, tt3) \
        if(getch() == c3) tok1(tt3); \
   else if(ch      == c2) tok1(tt2); \
   else                   (unget(), tok1(tt1))

/* Extern Functions -------------------------------------------------------- */

void TokParse(FILE *fp, struct token *tok, struct origin *orig) {
   if(!tok || !fp || !orig) return;

begin:
   i32 ch;

   tok->textC = 0;

   getch();
   if(FEOF(fp) || ch == EOF) {
      unget();
      tok1(tok_eof);
      goto done;
   }

   switch(ch) {
   /* Whitespace */
   case '\r': case '\n': tok1(tok_lnend); advLine(); goto done;

   /* 1-op tokens */
   case ';': tok1(tok_semico); goto done;
   case ',': tok1(tok_comma ); goto done;
   case '#': tok1(tok_hash  ); goto done;
   case '[': tok1(tok_bracko); goto done;
   case ']': tok1(tok_brackc); goto done;
   case '{': tok1(tok_braceo); goto done;
   case '}': tok1(tok_bracec); goto done;
   case '(': tok1(tok_pareno); goto done;
   case ')': tok1(tok_parenc); goto done;

   /* 2-op tokens */
   case '=': tok2('=', tok_eq,   tok_eq2   ); goto done;
   case '?': tok2('=', tok_tern, tok_terneq); goto done;
   case '!': tok2('=', tok_not,  tok_neq   ); goto done;
   case '~': tok2('=', tok_bnot, tok_bneq  ); goto done;
   case '*': tok2('=', tok_mul,  tok_muleq ); goto done;
   case '@': tok2('@', tok_at,   tok_at2   ); goto done;

   /* 3-op tokens */
   case '<': tok3('<', '=', tok_lt,  tok_lt2,  tok_le   ); goto done;
   case '>': tok3('>', '=', tok_gt,  tok_gt2,  tok_ge   ); goto done;
   case '|': tok3('|', '=', tok_or,  tok_or2,  tok_oreq ); goto done;
   case '&': tok3('&', '=', tok_and, tok_and2, tok_andeq); goto done;
   case '+': tok3('+', '=', tok_add, tok_add2, tok_addeq); goto done;
   case '%': tok3('%', '=', tok_mod, tok_mod2, tok_modeq); goto done;
   case '^': tok3('^', '=', tok_xor, tok_xor2, tok_xoreq); goto done;
   case ':': tok3(':', '=', tok_col, tok_col2, tok_coleq); goto done;

   case '-':
           if(getch() == '-') tok1(tok_sub2);
      else if(ch == '=')      tok1(tok_subeq);
      else if(ch == '>')      tok1(tok_rarrow);
      else if(IsDigit(ch))    {unget(); ch = '-'; break;}
      else                    {unget(); tok1(tok_sub);}
      goto done;

   case '/':
      if(getch() == '=') {
         tok1(tok_diveq);
      } else if(ch == '*') {
         tok1(tok_cmment);
         getch();

         for(i32 lvl = 1; lvl && !FEOF(fp); getch()) {
            if(ch == '/') {
               if(getch() == '*') {lvl++; continue;}
               else               {unget(); ch = '/';}
            } else if(ch == '*') {
               if(getch() == '/') {lvl--; continue;}
               else               {unget(); ch = '*';}
            }

            advLine();
         }

         unget();
      } else {
         unget();
         tok1(tok_div);
      }
      goto done;

   case '.':
      if(getch() == '.')
         tok2('.', tok_dot2, tok_dot3);
      else if(IsDigit(ch))
         {unget(); break;}
      else
         {unget(); tok1(tok_dot);}
      goto done;

   case '`':
      getch();
      tokText(untilEOL);
      tok1(tok_quote);
      goto done;

   case '\'': tok1(tok_charac); goto string;
   case '"':  tok1(tok_string); goto string;
   string:
      for(i32 i = 0, beg = ch; getch() != beg && !FEOF(fp);)
         textNext() = ch;
      goto done;
   }

   if(IsBlank(ch)) {
      while(ch = getch(), IsBlank(ch));
      unget();
      goto begin;
   } else if(IsDigit(ch) || ch == '.' || ch == '-' || ch == '$') {
      tok1(tok_number);

      if(ch == '$') {
         textNext() = '0';
         textNext() = 'x';
      } else {
         textNext() = ch;
      }

      getch();
      tokText(IsNumId);
   } else if(IsIdent(ch)) {
      tok1(tok_identi);
      tokText(IsIdent);
   } else {
      tok1(tok_chrseq);
      textNext() = ch;
   }

done:
   textNext() = '\0';
}

cstr TokPrint(struct token *tok) {
   static char pbuf[256];
   snprintf(pbuf, sizeof pbuf, "[%i:%i](%s/%i '%.*s':%i:%p)",
            tok->orig.line, tok->orig.colu, TokType(tok->type), tok->type,
            tok->textC, tok->textV, tok->textC, tok->textV);
   return pbuf;
}

bool TokIsKw(struct token *tok, cstr kw) {
   return tok->type == tok_identi && faststrcmp(tok->textV, kw) == 0;
}

cstr TokType(i32 type) {
   switch(type) {
   case tok_null:   return "null-token";
   case tok_chrseq: return "character-sequence";
   case tok_identi: return "identifier";
   case tok_number: return "number";
   case tok_string: return "string-double";
   case tok_charac: return "string-single";
   case tok_quote:  return "string-quote";
   case tok_cmment: return "comment";
   case tok_lnend:  return "new-line";
   case tok_semico: return "`;'";
   case tok_comma:  return "`,'";
   case tok_hash:   return "`#'";
   case tok_bracko: return "`['";
   case tok_brackc: return "`]'";
   case tok_braceo: return "`{'";
   case tok_bracec: return "`}'";
   case tok_pareno: return "`('";
   case tok_parenc: return "`)'";
   case tok_eq:     return "`='";
   case tok_eq2:    return "`=='";
   case tok_tern:   return "`?'";
   case tok_terneq: return "`?='";
   case tok_div:    return "`/'";
   case tok_diveq:  return "`/='";
   case tok_not:    return "`!'";
   case tok_neq:    return "`!='";
   case tok_bnot:   return "`~'";
   case tok_bneq:   return "`~='";
   case tok_mul:    return "`*'";
   case tok_muleq:  return "`*='";
   case tok_at:     return "`@'";
   case tok_at2:    return "`@@'";
   case tok_lt:     return "`<'";
   case tok_lt2:    return "`<<'";
   case tok_le:     return "`<='";
   case tok_gt:     return "`>'";
   case tok_gt2:    return "`>>'";
   case tok_ge:     return "`>='";
   case tok_or:     return "`|'";
   case tok_or2:    return "`||'";
   case tok_oreq:   return "`|='";
   case tok_and:    return "`&'";
   case tok_and2:   return "`&&'";
   case tok_andeq:  return "`&='";
   case tok_add:    return "`+'";
   case tok_add2:   return "`++'";
   case tok_addeq:  return "`+='";
   case tok_sub:    return "`-'";
   case tok_sub2:   return "`--'";
   case tok_subeq:  return "`-='";
   case tok_mod:    return "`%'";
   case tok_mod2:   return "`%%'";
   case tok_modeq:  return "`%='";
   case tok_xor:    return "`^'";
   case tok_xor2:   return "`^^'";
   case tok_xoreq:  return "`^='";
   case tok_col:    return "`:'";
   case tok_col2:   return "`::'";
   case tok_coleq:  return "`:='";
   case tok_dot:    return "`.'";
   case tok_dot2:   return "`..'";
   case tok_dot3:   return "`...'";
   case tok_rarrow: return "`->'";
   case tok_eof:    return "end-of-file";
   }
   return "invalid-token";
}

/* EOF */
