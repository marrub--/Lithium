#ifndef LITH_LOG_H
#define LITH_LOG_H

#include "lith_lognames.h"
#include "lith_list.h"

#include <stdarg.h>

#define LOG_MAX 7

typedef struct logmap_s
{
   int levelnum;
   __str name;
   list_t link;
} logmap_t;

typedef struct logdata_s
{
   __str info;
   int time;
   int from;
   list_t link;
   list_t linkfull;
} logdata_t;

typedef struct loginfo_s
{
   list_t hud;
   list_t full;
   list_t maps;
} loginfo_t;

void Lith_Log(struct player_s *p, __str fmt, ...);  // log to hud and full log
void Lith_LogF(struct player_s *p, __str fmt, ...); // log to full log only
void Lith_LogH(struct player_s *p, __str fmt, ...); // log to hud only
logdata_t *Lith_LogV(struct player_s *p, __str fmt, va_list vl); // vararg
[[__call("ScriptS")]] void Lith_PlayerUpdateLog(struct player_s *p);
[[__call("ScriptS")]] void Lith_HUD_Log(struct player_s *p);
void Lith_PlayerLogEntry(struct player_s *p);

#endif

// EOF

