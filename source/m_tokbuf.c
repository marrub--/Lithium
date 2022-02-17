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

#include "m_tokbuf.h"
#include "m_str.h"

#include <ACS_ZDoom.h>

enum {
   _bbeg = 14,
   _bend = 28,
};

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

void TBufCtor(struct tokbuf *tb, FILE *fp, cstr fname) {
   fastmemset(tb, 0, sizeof *tb);

   tb->fp        = fp;
   tb->fname     = fname;
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
   noinit static
   char errbuf[1024];

   va_list vl;
   mem_size_t len = faststrlen(tb->fname);

   errbuf[0] = '(';
   fastmemcpy(errbuf, tb->fname, len + 1);
   errbuf[len] = ')';

   va_start(vl, fmt);
   vsprintf(&errbuf[len + 1], fmt, vl);
   va_end(vl);

   res->some = true;
   res->err  = errbuf;
}

struct token *TBufExpc(struct tokbuf *tb, struct tbuf_err *res, struct token *tok, ...) {
   noinit static
   i32 tt[8];
   noinit static
   char ttstr[128];

   mem_size_t ttnum;
   va_list    args;

   va_start(args, tok);
   for(ttnum = 0; (tt[ttnum] = va_arg(args, i32)); ++ttnum) {
      if(tok->type == tt[ttnum]) {
         return tok;
      }
   }
   va_end(args);

   mem_size_t i = 0;
   ttstr[0] = '\0';

   faststrcat(ttstr, TokType(tt[i++]));
   if(ttnum > 1) {
      while(i < ttnum - 1) {
         faststrcat(ttstr, ", ");
         faststrcat(ttstr, TokType(tt[i++]));
      }
      if(ttnum > 2) {
         faststrcat(ttstr, ",");
      }
      faststrcat(ttstr, " or ");
      faststrcat(ttstr, TokType(tt[i++]));
   }
   tb->err(res, "%s expected %s", TokPrint(tok), ttstr);
   unwrap_cb();
}

void TBufExpDr(struct tokbuf *tb, struct tbuf_err *res, i32 t) {
   if(!tb->drop(t)) {
      struct token *tok = tb->reget();
      tb->err(res, "%s expected %s", TokPrint(tok), TokType(t));
   }
}

bool TBufKv(struct tokbuf *tb, struct tbuf_err *res, char *k, char *v) {
   struct token *tok = tb->expc(res, tb->get(), tok_semico, tok_identi, 0);
   unwrap(res);

   if(tok->type == tok_semico) {
      k[0] = v[0] = '\0';
      return false;
   } else {
      faststrcpy(k, tok->textV);
      tb->expdr(res, tok_col);
      unwrap(res);
      tok = tb->expc(res, tb->get(), tok_identi, tok_number, tok_string, 0);
      unwrap(res);
      faststrcpy(v, tok->textV);
      return true;
   }
}

/* EOF */
