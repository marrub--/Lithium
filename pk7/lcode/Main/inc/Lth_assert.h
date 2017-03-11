//-----------------------------------------------------------------------------
//
// Copyright © 2016-2017 Project Golan
//
// See "LICENSE.lithos3" for more information.
//
//-----------------------------------------------------------------------------
//
// Assertions.
//
//-----------------------------------------------------------------------------

#ifndef lithos3__Lth_assert_h
#define lithos3__Lth_assert_h

#include <stdio.h>

#ifdef NDEBUG
#define Lth_assert(expression) ((void)0)
#else
#define Lth_assert(expression) \
   if(!(expression)) \
      fprintf(stderr, "[lithos3] Assertion failed in %s (%s:%i): %s\n", \
         __func__, __FILE__, __LINE__, #expression); \
   else \
      ((void)0)
#endif

#endif//lithos3__Lth_assert_h
