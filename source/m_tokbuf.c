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

stkoff i32 tb_proc(struct token *tok) {
   switch(tok->type) {
   case tok_eof:    return tokproc_done;
   case tok_lnend:
   case tok_cmment: return tokproc_skip;
   }
   return tokproc_next;
}

stkoff i32 tb_procl(struct token *tok) {
   switch(tok->type) {
   case tok_eof:    return tokproc_done;
   case tok_cmment: return tokproc_skip;
   }
   return tokproc_next;
}

void tb_ctor(struct tokbuf *tb, FILE *fp, cstr fname) {
   fastmemset(tb, 0, sizeof *tb);
   tb->fp        = fp;
   tb->fname     = fname;
   tb->orig.line = 1;
   if(!tb->tok_process) tb->tok_process = tb_proc;
}

void tb_dtor(struct tokbuf *tb) {
   for(i32 i = 0; i < _bend; i++) {
      tb->toks[i].type = 0;
      tb->toks[i].textC = 0;
   }
}

struct token *tb_get(struct tokbuf *tb) {
   if(++tb->tpos < tb->tend) {
      return &tb->toks[tb->tpos];
   }
   /* free beginning of buffer */
   for(i32 i = 0; i < _bbeg; i++) {
      tb->toks[i].textC = 0;
   }
   /* move end of buffer to beginning */
   if(tb->tend) {
      i32 s = tb->tend - _bbeg;
      for(i32 i = s, j = 0; i < tb->tend; i++, j++) {
         tb->toks[j] = tb->toks[i];
      }
      fastmemset(&tb->toks[s], 0, sizeof tb->toks[s] * (tb->tend - s));
   }
   /* get new tokens */
   for(tb->tpos = tb->tend = _bbeg; tb->tend < _bend; tb->tend++) {
   skip:
      TokParse(tb->fp, &tb->toks[tb->tend], &tb->orig);
      switch(tb->tok_process(&tb->toks[tb->tend])) {
         case tokproc_next: break;
         case tokproc_done: goto done;
         case tokproc_skip: goto skip;
      }
   }
done:
   return &tb->toks[tb->tpos];
}

stkoff struct token *tb_unget(struct tokbuf *tb) {
   return &tb->toks[tb->tpos--];
}

stkoff struct token *tb_reget(struct tokbuf *tb) {
   return &tb->toks[tb->tpos];
}

stkoff struct token *tb_back(struct tokbuf *tb, i32 n) {
   return &tb->toks[tb->tpos - n];
}

struct token *tb_peek(struct tokbuf *tb) {
   tb_get(tb);
   return tb_unget(tb);
}

bool tb_drop(struct tokbuf *tb, i32 t) {
   if(tb_get(tb)->type != t) {
      tb_unget(tb);
      return false;
   } else {
      return true;
   }
}

void tb_err(struct tokbuf *tb, struct err *res, cstr fmt, struct token *tok, cstr func, ...) {
   noinit static char errbuf[1024];
   va_list vl;
   char *p = errbuf;
   if(tb->fname) {
      faststrpcpy(&p, tb->fname);
   }
   tok = tok |? tb_reget(tb);
   if(tok) {
      faststrpcpy(&p, TokPrint(tok));
   }
   if(func) {
      faststrpcpy(&p, func);
   }
   *p++ = ':';
   *p++ = ' ';
   va_start(vl, func);
   vsprintf(p, fmt, vl);
   va_end(vl);
   err_set(res, errbuf);
}

struct token *tb_expc(struct tokbuf *tb, struct err *res, struct token *tok, ...) {
   noinit static i32  tt[16];
   noinit static char ttstr[128];
   mem_size_t ttnum;
   va_list    vl;
   va_start(vl, tok);
   for(ttnum = 0; (tt[ttnum] = va_arg(vl, i32)); ++ttnum) {
      if(tok->type == tt[ttnum]) {
         return tok;
      }
   }
   va_end(vl);
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
   tb_err(tb, res, "expected %s", tok, nil, ttstr);
   unwrap_retn();
}

void tb_expdr(struct tokbuf *tb, struct err *res, i32 t) {
   if(!tb_drop(tb, t)) {
      struct token *tok = tb_reget(tb);
      tb_err(tb, res, "expected %s", tok, nil, TokType(t));
   }
}

bool tb_kv(struct tokbuf *tb, struct err *res, char *k, char *v) {
   struct token *tok = tb_expc(tb, res, tb_get(tb), tok_semico, tok_identi, 0);
   unwrap(res);

   if(tok->type == tok_semico) {
      k[0] = v[0] = '\0';
      return false;
   } else {
      faststrcpy(k, tok->textV);
      tb_expdr(tb, res, tok_col);
      unwrap(res);
      tok = tb_expc(tb, res, tb_get(tb), tok_identi, tok_number, tok_string, 0);
      unwrap(res);
      faststrcpy(v, tok->textV);
      return true;
   }
}

i32 tb_rflag(struct tokbuf *tb, struct err *res, char *s, tb_rflag_f fn) {
   i32 ret = 0;
   for(char *nx, *fl = faststrtok(s, &nx, ' '); fl; fl = faststrtok(nil, &nx, ' ')) {
      i32 parsed = fn(fl);
      if(parsed != -1) {
         set_bit(ret, parsed);
      } else {
         tb_err(tb, res, "invalid flag '%s'", nil, nil, fl);
         unwrap_retn();
      }
   }
   return ret;
}

/* EOF */
