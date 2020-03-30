/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Token-based text parsing.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef m_token_h
#define m_token_h

#include "m_vec.h"

#include <stdio.h>

#define TokStr(tok) l_strndup((tok)->textV, (tok)->textC)

/* Types ------------------------------------------------------------------- */

enum
{
   tok_null,   /* No token */

   /* Text sequences */
   tok_chrseq, /* Arbitrary character sequence */
   tok_identi, /* Identifier */
   tok_number, /* Number */
   tok_string, /* String */
   tok_charac, /* Character String */
   tok_quote,  /* Quote */
   tok_cmment, /* Comment */

   /* Special (post-process) */
   tok_keywrd, /* Keyword */
   tok_numint, /* Integer Literal */
   tok_numflt, /* Float Literal */

   /* Single characters */
   tok_lnend,  /* \n */
   tok_semico, /* ; */
   tok_comma,  /* , */
   tok_hash,   /* # */
   tok_bracko, /* [ */
   tok_brackc, /* ] */
   tok_braceo, /* { */
   tok_bracec, /* } */
   tok_pareno, /* ( */
   tok_parenc, /* ) */

   /* Paired operators */
   tok_eq,     /* = */
   tok_eq2,    /* == */
   tok_tern,   /* ? */
   tok_terneq, /* ?= */
   tok_div,    /* / */
   tok_diveq,  /* /= */
   tok_not,    /* ! */
   tok_neq,    /* != */
   tok_bnot,   /* ~ */
   tok_bneq,   /* ~= */
   tok_mul,    /* * */
   tok_muleq,  /* *= */
   tok_at,     /* @ */
   tok_at2,    /* @@ */

   /* Tupled operators */
   tok_lt,     /* < */
   tok_lt2,    /* << */
   tok_le,     /* <= */
   tok_gt,     /* > */
   tok_gt2,    /* >> */
   tok_ge,     /* >= */
   tok_or,     /* | */
   tok_or2,    /* || */
   tok_oreq,   /* |= */
   tok_and,    /* & */
   tok_and2,   /* && */
   tok_andeq,  /* &= */
   tok_add,    /* + */
   tok_add2,   /* ++ */
   tok_addeq,  /* += */
   tok_sub,    /* - */
   tok_sub2,   /* -- */
   tok_subeq,  /* -= */
   tok_mod,    /* % */
   tok_mod2,   /* %% */
   tok_modeq,  /* %= */
   tok_xor,    /* ^ */
   tok_xor2,   /* ^^ */
   tok_xoreq,  /* ^= */
   tok_col,    /* : */
   tok_col2,   /* :: */
   tok_coleq,  /* := */

   /* Misc */
   tok_dot,    /* . */
   tok_dot2,   /* .. */
   tok_dot3,   /* ... */
   tok_rarrow, /* -> */
   tok_eof,    /* End of file */

   tok_max
};

struct origin
{
   i32 colu, line;
};

struct token
{
   i32 type;
   struct origin orig;
   Vec_Decl(char, text);
};

/* Extern Functions -------------------------------------------------------- */

void TokParse(FILE *fp, struct token *tok, struct origin *orig);

#endif
