// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// p_log.h: Action logging functions.

#ifndef p_log_h
#define p_log_h

#include "m_vec.h"

struct logmap
{
   str name;
   i32 lnum;

   Vec_Decl(struct logfdt, data);
};

struct logfdt
{
   str inf;
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

void Lith_LogB(struct player *p, i32 levl, char const *fmt, ...); // log to HUD and full log
void Lith_LogH(struct player *p, i32 levl, char const *fmt, ...); // log to HUD only
void Lith_LogF(struct player *p,           char const *fmt, ...); // log to full log only
void Lith_PlayerLogEntry(struct player *p);

#endif
