// Copyright © 2018 Alison Sanderson, all rights reserved.
#ifndef LITH_MEMORY_H
#define LITH_MEMORY_H

#if LITH_MEMORY_DEBUG
#include <stdio.h>
#pragma GDCC STRENT_LITERAL ON
#define Salloc(t)    (printf(c"%S:%i: Salloc\n", __FILE__, __LINE__), calloc(1, sizeof(t)))
#define Malloc(s)    (printf(c"%S:%i: Malloc\n", __FILE__, __LINE__), malloc(s))
#define Calloc(n, s) (printf(c"%S:%i: Calloc\n", __FILE__, __LINE__), calloc(n, s))
#define Ralloc(p, s) (printf(c"%S:%i: Ralloc\n", __FILE__, __LINE__), realloc(p, s))
#define Dalloc(p)    (printf(c"%S:%i: Dalloc\n", __FILE__, __LINE__), free(p))
#else
#define Salloc(t)    calloc(1, sizeof(t))
#define Malloc(s)    malloc(s)
#define Calloc(n, s) calloc(n, s)
#define Ralloc(p, s) realloc(p, s)
#define Dalloc(p)    free(p)
#endif

#endif
