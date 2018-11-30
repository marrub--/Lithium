// Copyright © 2017 Alison Sanderson, all rights reserved.
#ifndef lith_tokbuf_h
#define lith_tokbuf_h

#include "lith_token.h"
#include "lith_types.h"

#include <stdbool.h>

// Extern Functions ----------------------------------------------------------|

        void          Lith_TBufCtor (struct tokbuf *tb);
        void          Lith_TBufDtor (struct tokbuf *tb);
        struct token *Lith_TBufGet  (struct tokbuf *tb);
        struct token *Lith_TBufPeek (struct tokbuf *tb);
stkcall struct token *Lith_TBufUnGet(struct tokbuf *tb);
stkcall struct token *Lith_TBufReGet(struct tokbuf *tb);
        bool          Lith_TBufDrop (struct tokbuf *tb, int t);
stkcall int           Lith_TBufProc (struct token *tok);
stkcall int           Lith_TBufProcL(struct token *tok);

// Types ---------------------------------------------------------------------|

enum
{
   tokproc_next,
   tokproc_done,
   tokproc_skip
};

struct tokbuf
{
   __prop ctor  {call: Lith_TBufCtor (this)}
   __prop dtor  {call: Lith_TBufDtor (this)}
   __prop get   {call: Lith_TBufGet  (this)}
   __prop peek  {call: Lith_TBufPeek (this)}
   __prop unget {call: Lith_TBufUnGet(this)}
   __prop reget {call: Lith_TBufReGet(this)}
   __prop drop  {call: Lith_TBufDrop (this)}

   struct origin orig;

   FILE *fp;

   struct token *toks;

   int tpos, tend;
   int bbeg, bend;

   stkcall int (*tokProcess)(struct token *tok);
};

#endif
