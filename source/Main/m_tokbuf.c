// Copyright © 2017 Graham Sanderson, all rights reserved.
#include "lith_tokbuf.h"

#include <string.h>

// Extern Functions ----------------------------------------------------------|

//
// Lith_TBufProc
//
int Lith_TBufProc(struct token *tok, void *udata)
{
   switch(tok->type)
   {
   case tok_eof:    return tokproc_done;
   case tok_lnend:
   case tok_cmtblk:
   case tok_cmtlin: return tokproc_skip;
   }
   return tokproc_next;
}

//
// Lith_TBufProcL
//
int Lith_TBufProcL(struct token *tok, void *udata)
{
   switch(tok->type)
   {
   case tok_eof:    return tokproc_done;
   case tok_cmtblk:
   case tok_cmtlin: return tokproc_skip;
   }
   return tokproc_next;
}

//
// Lith_TBufCtor
//
void Lith_TBufCtor(struct tokbuf *tb)
{
   tb->orig.line = 1;
   tb->toks = Calloc(tb->bend, sizeof(struct token));
   if(!tb->tokProcess) tb->tokProcess = Lith_TBufProc;
}

//
// Lith_TBufDtor
//
void Lith_TBufDtor(struct tokbuf *tb)
{
   if(tb->toks) for(int i = 0; i < tb->bend; i++)
      Vec_Clear(tb->toks[i].text);

   Dalloc(tb->toks);
}

//
// Lith_TBufGet
//
struct token *Lith_TBufGet(struct tokbuf *tb)
{
   if(++tb->tpos < tb->tend)
      return &tb->toks[tb->tpos];

   // Free beginning of buffer.
   for(int i = 0; i < tb->bbeg; i++)
   {
      Vec_Clear(tb->toks[i].text);
      tb->toks[i] = (struct token){0};
   }

   // Move end of buffer to beginning.
   if(tb->tend) for(int i = tb->tend - tb->bbeg, j = 0; i < tb->tend; i++, j++)
   {
      tb->toks[j] = tb->toks[i];
      tb->toks[i] = (struct token){0};
   }

   // Get new tokens.
   for(tb->tpos = tb->tend = tb->bbeg; tb->tend < tb->bend; tb->tend++)
   {
   skip:
      Lith_ParseToken(tb->fp, &tb->toks[tb->tend], &tb->orig);

      switch(tb->tokProcess(&tb->toks[tb->tend], tb->udata)) {
      case tokproc_next: break;
      case tokproc_done: goto done;
      case tokproc_skip: goto skip;
      }
   }

done:
   return &tb->toks[tb->tpos];
}

//
// Lith_TBufPeek
//
struct token *Lith_TBufPeek(struct tokbuf *tb)
{
   Lith_TBufGet(tb);
   return Lith_TBufUnGet(tb);
}

//
// Lith_TBufUnGet
//
struct token *Lith_TBufUnGet(struct tokbuf *tb)
{
   return &tb->toks[tb->tpos--];
}

//
// Lith_TBufReGet
//
struct token *Lith_TBufReGet(struct tokbuf *tb)
{
   return &tb->toks[tb->tpos];
}

//
// Lith_TBufDrop
//
bool Lith_TBufDrop(struct tokbuf *tb, int t)
{
   if(Lith_TBufGet(tb)->type != t)
      {Lith_TBufUnGet(tb); return false;}
   else
      return true;
}

// EOF
