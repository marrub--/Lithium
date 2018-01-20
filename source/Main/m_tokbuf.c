// Copyright © 2017 Graham Sanderson, all rights reserved.
#include "lith_tokbuf.h"

#include <string.h>

// Static Functions ----------------------------------------------------------|

//
// TBufProc
//
static int TBufProc(token_t *tok, void *udata)
{
   switch(tok->type) {
   default:         return tokproc_next;
   case tok_eof:    return tokproc_done;
   case tok_lnend:
   case tok_cmtblk:
   case tok_cmtlin: return tokproc_skip;
   }
}

//
// TBufPrint
//
static void TBufPrint(tokbuf_t *tb)
{
   for(int i = 0; i < tb->tend; i++)
      printf("[%i]%i: %p %s%s\n", i, tb->toks[i].type, tb->toks[i].textV,
         tb->toks[i].textV, i == tb->tpos ? " <-- cursor is here" : "");

   printf("---\n");
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_TBufCtor
//
void Lith_TBufCtor(tokbuf_t *tb)
{
   tb->orig.line = 1;
   tb->toks = calloc(tb->bend, sizeof(token_t));
   if(!tb->tokProcess) tb->tokProcess = TBufProc;
}

//
// Lith_TBufDtor
//
void Lith_TBufDtor(tokbuf_t *tb)
{
   if(tb->toks)
      for(int i = 0; i < tb->bend; i++)
         Vec_Clear(tb->toks[i].text);

   free(tb->toks);
}

//
// Lith_TBufGet
//
token_t *Lith_TBufGet(tokbuf_t *tb)
{
   if(++tb->tpos < tb->tend)
      return &tb->toks[tb->tpos];

   // Free beginning of buffer.
   for(int i = 0; i < tb->bbeg; i++) {
      Vec_Clear(tb->toks[i].text);
      tb->toks[i] = (token_t){0};
   }

   // Move end of buffer to beginning.
   if(tb->tend) for(int i = tb->tend - tb->bbeg, j = 0; i < tb->tend; i++, j++)
   {
      tb->toks[j] = tb->toks[i];
      tb->toks[i] = (token_t){0};
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
token_t *Lith_TBufPeek(tokbuf_t *tb)
{
   Lith_TBufGet(tb);
   return Lith_TBufUnGet(tb);
}

//
// Lith_TBufUnGet
//
token_t *Lith_TBufUnGet(tokbuf_t *tb)
{
   return &tb->toks[tb->tpos--];
}

//
// Lith_TBufReGet
//
token_t *Lith_TBufReGet(tokbuf_t *tb)
{
   return &tb->toks[tb->tpos];
}

//
// Lith_TBufDrop
//
bool Lith_TBufDrop(tokbuf_t *tb, int t)
{
   if(Lith_TBufGet(tb)->type != t)
      {Lith_TBufUnGet(tb); return false;}
   else
      return true;
}

// EOF
