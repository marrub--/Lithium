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

#include <ACS_ZDoom.h>
#include <string.h>

enum {
   _bbeg = 14,
   _bend = 28,
};

noinit
static char errbuf[1024];

i32 TBufProc(struct token *tok) {
   switch(tok->type) {
      case tok_eof:    return tokproc_done;
      case tok_lnend:
      case tok_cmment: return tokproc_skip;
   }

   return tokproc_next;
}

i32 TBufProcL(struct token *tok) {
   switch(tok->type) {
      case tok_eof:    return tokproc_done;
      case tok_cmment: return tokproc_skip;
   }

   return tokproc_next;
}

void TBufCtor(struct tokbuf *tb, FILE *fp) {
   fastmemset(tb, 0, sizeof *tb);

   tb->fp        = fp;
   tb->orig.line = 1;

   if(!tb->tokProcess) tb->tokProcess = TBufProc;
}

void TBufDtor(struct tokbuf *tb) {
   for(i32 i = 0; i < _bend; i++) {
      tb->toks[i].type = 0;
      tb->toks[i].textC = 0;
   }
}

struct token *TBufGet(struct tokbuf *tb) {
   if(++tb->tpos < tb->tend) return &tb->toks[tb->tpos];

   /* free beginning of buffer */
   for(i32 i = 0; i < _bbeg; i++) tb->toks[i].textC = 0;

   /* move end of buffer to beginning */
   if(tb->tend) {
      i32 s = tb->tend - _bbeg;

      for(i32 i = s, j = 0; i < tb->tend; i++, j++)
         tb->toks[j] = tb->toks[i];

      fastmemset(&tb->toks[s], 0, sizeof tb->toks[s] * (tb->tend - s));
   }

   /* get new tokens */
   for(tb->tpos = tb->tend = _bbeg; tb->tend < _bend; tb->tend++) {
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

struct token *TBufUnGet(struct tokbuf *tb) {
   return &tb->toks[tb->tpos--];
}

struct token *TBufReGet(struct tokbuf *tb) {
   return &tb->toks[tb->tpos];
}

struct token *TBufBack(struct tokbuf *tb, i32 n) {
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

void TBufErr(struct tokbuf *tb, struct tbuf_err *res, cstr fmt, ...) {
   va_list vl;

   va_start(vl, fmt);
   vsnprintf(errbuf, sizeof errbuf, fmt, vl);
   va_end(vl);

   res->some = true;
   res->err  = errbuf;
}

struct token *TBufExpc(struct tokbuf *tb, struct tbuf_err *res, struct token *tok, i32 t1) {
   if(tok->type != t1) {
      tb->err(res, "%s expected %s", TokPrint(tok), TokType(t1));
   }
   return tok;
}

struct token *TBufExpc2(struct tokbuf *tb, struct tbuf_err *res, struct token *tok, i32 t1, i32 t2) {
   if(tok->type != t1 && tok->type != t2) {
      tb->err(res, "%s expected %s or %s", TokPrint(tok), TokType(t1), TokType(t2));
   }
   return tok;
}

struct token *TBufExpc3(struct tokbuf *tb, struct tbuf_err *res, struct token *tok, i32 t1, i32 t2, i32 t3) {
   if(tok->type != t1 && tok->type != t2 && tok->type != t3) {
      tb->err(res, "%s expected %s, %s, or %s", TokPrint(tok), TokType(t1),
              TokType(t2), TokType(t3));
   }
   return tok;
}

void TBufExpDr(struct tokbuf *tb, struct tbuf_err *res, i32 t) {
   if(!tb->drop(t)) {
      struct token *tok = tb->reget();
      tb->err(res, "%s expected %s", TokPrint(tok), TokType(t));
   }
}

/* EOF */
