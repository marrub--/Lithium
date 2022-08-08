// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Buffered token stream.                                                   │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef m_tokbuf_h
#define m_tokbuf_h

#include "m_token.h"
#include "m_types.h"
#include "m_flow.h"

#include <stdbool.h>
#include <stdarg.h>

enum {
   tokproc_next,
   tokproc_done,
   tokproc_skip
};

stkcall funcdef i32 (*tb_rflag_f  )(cstr s);
stkcall funcdef i32 (*tb_process_f)(struct token *tok);

struct tokbuf {
   struct origin orig;
   FILE         *fp;
   cstr          fname;
   struct token  toks[32];
   i32           tpos, tend;
   tb_process_f  tok_process;
};

stkcall i32           tb_proc (struct token *tok);
stkcall i32           tb_procl(struct token *tok);
void                  tb_ctor (struct tokbuf *tb, FILE *fp, cstr fname);
void                  tb_dtor (struct tokbuf *tb);
struct token         *tb_get  (struct tokbuf *tb);
stkcall struct token *tb_unget(struct tokbuf *tb);
stkcall struct token *tb_reget(struct tokbuf *tb);
stkcall struct token *tb_back (struct tokbuf *tb, i32 n);
struct token         *tb_peek (struct tokbuf *tb);
bool                  tb_drop (struct tokbuf *tb, i32 t);
void                  tb_err  (struct tokbuf *tb, struct err *res, cstr fmt, struct token *tok, cstr func, ...);
struct token         *tb_expc (struct tokbuf *tb, struct err *res, struct token *tok, ...);
void                  tb_expdr(struct tokbuf *tb, struct err *res, i32 t);
bool                  tb_kv   (struct tokbuf *tb, struct err *res, char *k, char *v);
i32                   tb_rflag(struct tokbuf *tb, struct err *res, char *s, tb_rflag_f fn);

#endif
