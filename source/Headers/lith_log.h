// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifndef lith_log_h
#define lith_log_h

#include "lith_darray.h"

struct logmap
{
   __str name;
   int   lnum;

   Vec_Decl(struct logfdt, data);
};

struct logfdt
{
   __str info;
};

struct logdat
{
   anonymous
   struct logfdt fdta;
   u32           time;
   u32           ftim;
};

struct loginfo
{
   struct logdat hudV[7];
   size_t        hudC;

   Vec_Decl(struct logmap, maps);

   struct logmap *curmap;
   u32            curtime;
};

void Lith_LogB(struct player *p, int levl, __str fmt, ...); // log to HUD and full log
void Lith_LogH(struct player *p, int levl, __str fmt, ...); // log to HUD only
void Lith_LogF(struct player *p,           __str fmt, ...); // log to full log only
void Lith_PlayerLogEntry(struct player *p);

#endif

// EOF

