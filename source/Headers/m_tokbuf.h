// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#ifndef m_tokbuf_h
#define m_tokbuf_h

#include "m_token.h"
#include "m_types.h"

#include <stdbool.h>

// Extern Functions ----------------------------------------------------------|

        void          Lith_TBufCtor (struct tokbuf *tb);
        void          Lith_TBufDtor (struct tokbuf *tb);
        struct token *Lith_TBufGet  (struct tokbuf *tb);
        struct token *Lith_TBufPeek (struct tokbuf *tb);
stkcall struct token *Lith_TBufUnGet(struct tokbuf *tb);
stkcall struct token *Lith_TBufReGet(struct tokbuf *tb);
        bool          Lith_TBufDrop (struct tokbuf *tb, i32 t);
stkcall i32           Lith_TBufProc (struct token *tok);
stkcall i32           Lith_TBufProcL(struct token *tok);

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

   i32 tpos, tend;
   i32 bbeg, bend;

   stkcall i32 (*tokProcess)(struct token *tok);
};

#endif
