// Copyright © 2017 Graham Sanderson, all rights reserved.
#ifndef lith_tokbuf_h
#define lith_tokbuf_h

#include "lith_token.h"

#include <stdbool.h>

// Extern Functions ----------------------------------------------------------|

struct tokbuf_s;
void     Lith_TBufCtor (struct tokbuf_s *tb);
void     Lith_TBufDtor (struct tokbuf_s *tb);
token_t *Lith_TBufGet  (struct tokbuf_s *tb);
token_t *Lith_TBufPeek (struct tokbuf_s *tb);
token_t *Lith_TBufUnGet(struct tokbuf_s *tb);
token_t *Lith_TBufReGet(struct tokbuf_s *tb);
bool     Lith_TBufDrop (struct tokbuf_s *tb, int t);

// Types ---------------------------------------------------------------------|

enum
{
   tokproc_next,
   tokproc_done,
   tokproc_skip
};

typedef struct tokbuf_s
{
   __prop ctor  {call: Lith_TBufCtor (this)}
   __prop dtor  {call: Lith_TBufDtor (this)}
   __prop get   {call: Lith_TBufGet  (this)}
   __prop peek  {call: Lith_TBufPeek (this)}
   __prop unget {call: Lith_TBufUnGet(this)}
   __prop reget {call: Lith_TBufReGet(this)}
   __prop drop  {call: Lith_TBufDrop (this)}

   origin_t orig;
   FILE    *fp;
   token_t *toks;
   int      tpos, tend;
   int      bbeg, bend;
   void    *udata;

   int (*tokProcess)(token_t *tok, void *udata);
} tokbuf_t;

#endif
