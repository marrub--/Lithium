//----------------------------------------------------------------------------
//
// Copyright (c) 2017 Project Golan
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//----------------------------------------------------------------------------
//
// Lithium.h
//
// API for modifying Lithium.
//
//----------------------------------------------------------------------------

#ifndef source__Main__api__Lithium_H
#define source__Main__api__Lithium_H

#include <stdfix.h>
#include <stdbool.h>

typedef long long int score_t;

typedef struct shopdef_s
{
   // Public Data
   __str name;
   __str bipunlock;
   score_t cost;
   
   // Private Data
   void (*shopBuy)   (struct player_s *p, struct shopdef_s const *def, void *obj);
   bool (*shopCanBuy)(struct player_s *p, struct shopdef_s const *def, void *obj);
   void (*shopGive)  (int tid,            struct shopdef_s const *def, void *obj);
} shopdef_t;

#include "LithUpgrades.h"

#endif//source__Main__api__Lithium_H

