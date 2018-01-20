// Copyright © 2017 Graham Sanderson, all rights reserved.
#ifndef lith_token_h
#define lith_token_h

#include "lith_darray.h"

#include <stdio.h>

// Types ---------------------------------------------------------------------|

enum
{
   tok_null,   // No token

   // Text sequences
   tok_chrseq, // Arbitrary character sequence
   tok_identi, // Identifier
   tok_number, // Number
   tok_string, // String           < " >  <characters> < " >
   tok_charac, // Character String < ' >  <characters> < ' >
   tok_quote,  // Quote            < ` >  <characters> < \n >
   tok_cmtlin, // Line comment     < // > <characters> < \n >
   tok_cmtblk, // Block comment    < /* > <characters> < */ >

   // Special (post-process)
   tok_keywrd, // Keyword
   tok_numint, // Integer Literal
   tok_numflt, // Float Literal

   // Single characters
   tok_lnend,  // \n
   tok_semico, // ;
   tok_comma,  // ,
   tok_dollar, // $
   tok_bracko, // [
   tok_brackc, // ]
   tok_braceo, // {
   tok_bracec, // }
   tok_pareno, // (
   tok_parenc, // )

   // Paired operators
   tok_eq,     // =
   tok_eq2,    // ==
   tok_tern,   // ?
   tok_terneq, // ?=
   tok_div,    // /
   tok_diveq,  // /=
   tok_not,    // !
   tok_neq,    // !=
   tok_bnot,   // ~
   tok_bneq,   // ~=
   tok_mul,    // *
   tok_muleq,  // *=
   tok_at,     // @
   tok_at2,    // @@

   // Tupled operators
   tok_lt,     // <
   tok_lt2,    // <<
   tok_le,     // <=
   tok_gt,     // >
   tok_gt2,    // >>
   tok_ge,     // >=
   tok_or,     // |
   tok_or2,    // ||
   tok_oreq,   // |=
   tok_and,    // &
   tok_and2,   // &&
   tok_andeq,  // &=
   tok_add,    // +
   tok_add2,   // ++
   tok_addeq,  // +=
   tok_sub,    // -
   tok_sub2,   // --
   tok_subeq,  // -=
   tok_mod,    // %
   tok_mod2,   // %%
   tok_modeq,  // %=
   tok_xor,    // ^
   tok_xor2,   // ^^
   tok_xoreq,  // ^=
   tok_col,    // :
   tok_col2,   // ::
   tok_coleq,  // :=

   // Misc
   tok_dot,    // .
   tok_dot2,   // ..
   tok_dot3,   // ...
   tok_rarrow, // ->
   tok_eof,    // End of file

   tok_max
};

typedef struct origin_s
{
   int colu, line;
} origin_t;

typedef struct token_s
{
   int      type;
   origin_t orig;
   Vec_Decl(char, text);
} token_t;

// Extern Functions ----------------------------------------------------------|

void Lith_ParseToken(FILE *fp, token_t *tok, origin_t *orig);

#endif
