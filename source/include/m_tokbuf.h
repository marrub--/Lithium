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

#ifndef m_tokbuf_h
#define m_tokbuf_h

#include "m_token.h"
#include "m_types.h"

#include <stdbool.h>
#include <stdarg.h>

i32           TBufProc (struct token *tok);
i32           TBufProcL(struct token *tok);
void          TBufCtor (struct tokbuf *tb, FILE *fp);
void          TBufDtor (struct tokbuf *tb);
struct token *TBufGet  (struct tokbuf *tb);
struct token *TBufPeek (struct tokbuf *tb);
struct token *TBufUnGet(struct tokbuf *tb);
struct token *TBufReGet(struct tokbuf *tb);
struct token *TBufBack (struct tokbuf *tb, i32 n);
bool          TBufDrop (struct tokbuf *tb, i32 t);
void          TBufErr  (struct tokbuf *tb, struct tbuf_err *res, cstr fmt, ...);
struct token *TBufExpc (struct tokbuf *tb, struct tbuf_err *res, struct token *tok, i32 t1);
struct token *TBufExpc2(struct tokbuf *tb, struct tbuf_err *res, struct token *tok, i32 t1, i32 t2);
struct token *TBufExpc3(struct tokbuf *tb, struct tbuf_err *res, struct token *tok, i32 t1, i32 t2, i32 t3);
void          TBufExpDr(struct tokbuf *tb, struct tbuf_err *res, i32 t);

enum {
   tokproc_next,
   tokproc_done,
   tokproc_skip
};

struct tbuf_err {
   bool some;
   cstr err;
};

struct tokbuf {
   __prop get   {operator(): TBufGet  (this)}
   __prop peek  {operator(): TBufPeek (this)}
   __prop unget {operator(): TBufUnGet(this)}
   __prop reget {operator(): TBufReGet(this)}
   __prop back  {operator(): TBufBack (this)}
   __prop drop  {operator(): TBufDrop (this)}
   __prop err   {operator(): TBufErr  (this)}
   __prop expc  {operator(): TBufExpc (this)}
   __prop expc2 {operator(): TBufExpc2(this)}
   __prop expc3 {operator(): TBufExpc3(this)}
   __prop expdr {operator(): TBufExpDr(this)}

   struct origin orig;

   FILE *fp;

   struct token toks[32];

   i32 tpos, tend;

   i32 (*tokProcess)(struct token *tok);
};

#endif
