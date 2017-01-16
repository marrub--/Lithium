#ifndef LITH_LOG_H
#define LITH_LOG_H

#include "lith_lognames.h"
#include "lith_list.h"

#define LOG_MAX 7

typedef struct logdata_s
{
   __str info;
   int time;
   list_t link;
} logdata_t;

void Lith_Log(struct player_s *p, __str fmt, ...);
[[__call("ScriptS")]] void Lith_PlayerUpdateLog(struct player_s *p);
[[__call("ScriptS")]] void Lith_HUD_Log(struct player_s *p);

#endif

// EOF

