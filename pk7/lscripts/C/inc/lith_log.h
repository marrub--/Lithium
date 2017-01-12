#ifndef LITH_LOG_H
#define LITH_LOG_H

#include "lith_lognames.h"

#define LOG_MAX 7

typedef struct logdata_s
{
   __str info;
   int time;
} logdata_t;

void Lith_Log(struct player_s *p, __str fmt, ...);
[[__call("ScriptS")]] void Lith_PlayerUpdateLog(struct player_s *p);
[[__call("ScriptS")]] void Lith_HUD_Log(struct player_s *p);

#endif

// EOF

