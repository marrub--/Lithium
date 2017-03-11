//-----------------------------------------------------------------------------
//
// Copyright © 2016-2017 Project Golan
//
// See "LICENSE.lithos3" for more information.
//
//-----------------------------------------------------------------------------
//
// Token stream.
//
//-----------------------------------------------------------------------------

#ifndef lithos3__Lth_tokenstream_h
#define lithos3__Lth_tokenstream_h

#include "Lth_token.h"

#include <stdio.h>


// Type Definitions ----------------------------------------------------------|

//
// Lth_TokenStream
//
// internal data
//    fp
//    tokbuf
//    filled
//
// read-write
//    skipeol: Skip line end tokens.   Defaults to true.
//    skipspc: Skip whitespace tokens. Defaults to true.
//
typedef struct Lth_TokenStream
{
   FILE     *fp;
   Lth_Token tokbuf;
   bool      filled;

   bool skipeol;
   bool skipspc;
} Lth_TokenStream;


// Extern Functions ----------------------------------------------------------|

Lth_TokenStream *Lth_TokenStreamOpen(void *data, size_t size);
void Lth_TokenStreamClose(Lth_TokenStream *stream);

Lth_Token const *Lth_TokenStreamBump(Lth_TokenStream *stream);
Lth_Token const *Lth_TokenStreamPeek(Lth_TokenStream *stream);
bool             Lth_TokenStreamDrop(Lth_TokenStream *stream, Lth_TokenType tt);

#endif//lithos3__Lth_tokenstream_h
