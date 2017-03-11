//-----------------------------------------------------------------------------
//
// Copyright © 2016-2017 Project Golan, David Hill
//
// Code by David Hill has been relicensed with consent.
//
// See "LICENSE.lithos3" for more information.
//
//-----------------------------------------------------------------------------
//
// Token stream.
//
//-----------------------------------------------------------------------------

#define _GNU_SOURCE

#include "Lth.h"


// Static Functions ----------------------------------------------------------|

static int Lth_TokenFileStream_getc(struct Lth_TokenStreamBuf *cookie);
static int Lth_TokenFileStream_bumpc(struct Lth_TokenStreamBuf *cookie);


// Type Definitions ----------------------------------------------------------|

//
// Lth_TokenStreamBuf
//
typedef struct Lth_TokenStreamBuf
{
   char  *beg, *itr, *end;
   size_t len;
   
   __prop getc  {call: Lth_TokenFileStream_getc(this)}
   __prop bumpc {call: Lth_TokenFileStream_bumpc(this)}
} Lth_TokenStreamBuf;


// Static Functions ----------------------------------------------------------|

//
// Lth_TokenFileStream_getc
//
static int Lth_TokenFileStream_getc(Lth_TokenStreamBuf *cookie)
{
   if(cookie->itr == cookie->end) return EOF;
   return *cookie->itr;
}

//
// Lth_TokenFileStream_bumpc
//
static int Lth_TokenFileStream_bumpc(Lth_TokenStreamBuf *cookie)
{
   if(cookie->itr == cookie->end) return EOF;
   return *cookie->itr++;
}

//
// Lth_TokenFileStream_read
//
static ssize_t Lth_TokenFileStream_read(void *cookie_, char *buf, size_t size)
{
   Lth_TokenStreamBuf *cookie = cookie_;
   size_t              avail  = cookie->end - cookie->itr;

   if(size > avail)
      size = avail;

   if(size == 0) return 0;

   for(ssize_t i = 0; i < size;)
   {
      if(cookie->getc() == '/')
      {
         cookie->bumpc();
         if(cookie->getc() == '*')
         {
            for(cookie->bumpc(); !(cookie->bumpc() == '*' && cookie->getc() == '/');)
               if(cookie->getc() == EOF)
                  return i;
            buf[i++] = ' ';
         }
         else if(cookie->getc() == '/')
         {
            do if(cookie->bumpc() == EOF) return i;
            while(cookie->getc() != '\n');
            buf[i++] = ' ';
         }
         else
            buf[i++] = '/';
      }
      else if(cookie->getc() == EOF)
         return i;
      else
         buf[i++] = cookie->bumpc();
   }

   return size;
}

//
// Lth_TokenFileStream_close
//
static int Lth_TokenFileStream_close(void *cookie)
{
   free(cookie);
   return 0;
}

//
// Lth_TokenFileStreamOpen
//
static FILE *Lth_TokenFileStreamOpen(void *data, size_t size)
{
   cookie_io_functions_t io_funcs = {
      .read  = Lth_TokenFileStream_read,
      .write = NULL,
      .seek  = NULL,
      .close = Lth_TokenFileStream_close
   };

   Lth_TokenStreamBuf *cookie = calloc(1, sizeof(Lth_TokenStreamBuf));

   cookie->beg = data;
   cookie->itr = data;
   cookie->end = cookie->beg + size;

   return fopencookie(cookie, "r", io_funcs);
}

//
// Lth_TokenStreamGrab
//
static void Lth_TokenStreamGrab(Lth_TokenStream *stream)
{
   if(feof(stream->fp)) return;

   do Lth_TokenGet(stream->fp, &stream->tokbuf);
   while((stream->skipspc && stream->tokbuf.type == Lth_TOK_Space) ||
         (stream->skipeol && stream->tokbuf.type == Lth_TOK_LnEnd));

   stream->filled = true;
}


// Extern Functions ----------------------------------------------------------|

//
// Lth_TokenStreamOpen
//
Lth_TokenStream *Lth_TokenStreamOpen(void *data, size_t size)
{
   Lth_TokenStream *stream = calloc(1, sizeof(Lth_TokenStream));

   stream->fp = Lth_TokenFileStreamOpen(data, size);

   if(stream->fp == NULL)
   {
      free(stream);
      return NULL;
   }

   stream->skipeol = stream->skipspc = true;

   return stream;
}

//
// Lth_TokenStreamClose
//
void Lth_TokenStreamClose(Lth_TokenStream *stream)
{
   if(stream == NULL) return;

   Lth_TokenCleanup(stream->tokbuf);
   fclose(stream->fp);
   free(stream);
}

//
// Lth_TokenStreamBump
//
Lth_Token const *Lth_TokenStreamBump(Lth_TokenStream *stream)
{
   Lth_Token const *tok = Lth_TokenStreamPeek(stream);
   return stream->filled = false, tok;
}

//
// Lth_TokenStreamPeek
//
Lth_Token const *Lth_TokenStreamPeek(Lth_TokenStream *stream)
{
   static Lth_Token const eof = { .type = Lth_TOK_EOF };

   if(!stream->filled && (Lth_TokenStreamGrab(stream), !stream->filled))
      return &eof;

   return &stream->tokbuf;
}

//
// Lth_TokenStreamDrop
//
bool Lth_TokenStreamDrop(Lth_TokenStream *stream, Lth_TokenType tt)
{
   Lth_Token const *tok = Lth_TokenStreamPeek(stream);

   if(tok->type == tt)
      return Lth_TokenStreamBump(stream), true;

   return false;
}

// EOF
