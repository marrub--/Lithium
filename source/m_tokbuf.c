/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Buffered token stream.
 *
 * ---------------------------------------------------------------------------|
 */

#include "m_tokbuf.h"
#include "m_str.h"

#include <string.h>

/* Extern Functions -------------------------------------------------------- */

stkcall i32 TBufProc(struct token *tok) {
   switch(tok->type) {
      case tok_eof:    return tokproc_done;
      case tok_lnend:
      case tok_cmment: return tokproc_skip;
   }

   return tokproc_next;
}

stkcall i32 TBufProcL(struct token *tok) {
   switch(tok->type) {
      case tok_eof:    return tokproc_done;
      case tok_cmment: return tokproc_skip;
   }

   return tokproc_next;
}

void TBufCtor(struct tokbuf *tb) {
   tb->orig.line = 1;
   tb->toks = Calloc(tb->bend, sizeof *tb->toks);

   if(!tb->tokProcess) tb->tokProcess = TBufProc;
}

void TBufDtor(struct tokbuf *tb) {
   if(tb->toks) for(i32 i = 0; i < tb->bend; i++) tb->toks[i].textC = 0;

   Dalloc(tb->toks);
}

struct token *TBufGet(struct tokbuf *tb) {
   if(++tb->tpos < tb->tend) return &tb->toks[tb->tpos];

   /* free beginning of buffer */
   for(i32 i = 0; i < tb->bbeg; i++) tb->toks[i].textC = 0;

   /* move end of buffer to beginning */
   if(tb->tend) {
      i32 s = tb->tend - tb->bbeg;

      for(i32 i = s, j = 0; i < tb->tend; i++, j++)
         tb->toks[j] = tb->toks[i];

      fastmemset(&tb->toks[s], 0, sizeof tb->toks[s] * (tb->tend - s));
   }

   /* get new tokens */
   for(tb->tpos = tb->tend = tb->bbeg; tb->tend < tb->bend; tb->tend++) {
   skip:
      TokParse(tb->fp, &tb->toks[tb->tend], &tb->orig);

      switch(tb->tokProcess(&tb->toks[tb->tend])) {
         case tokproc_next: break;
         case tokproc_done: goto done;
         case tokproc_skip: goto skip;
      }
   }

done:
   return &tb->toks[tb->tpos];
}

struct token *TBufPeek(struct tokbuf *tb) {
   TBufGet(tb);
   return TBufUnGet(tb);
}

stkcall struct token *TBufUnGet(struct tokbuf *tb) {
   return &tb->toks[tb->tpos--];
}

stkcall struct token *TBufReGet(struct tokbuf *tb) {
   return &tb->toks[tb->tpos];
}

stkcall struct token *TBufBack(struct tokbuf *tb, i32 n) {
   return &tb->toks[tb->tpos - n];
}

bool TBufDrop(struct tokbuf *tb, i32 t) {
   if(TBufGet(tb)->type != t) {
      TBufUnGet(tb);
      return false;
   } else {
      return true;
   }
}

/* EOF */
