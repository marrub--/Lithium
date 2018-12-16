// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"
#include "lith_gui.h"

StrEntON

void Lith_CBITab_Statistics(gui_state_t *g, struct player *p)
{
   int n = 0;

   #define Stat(name, f, x) \
      PrintTextStr(name); PrintText("cbifont", CR_WHITE, 23,1,  50 + 8*n,1); \
      PrintTextFmt(f, x); PrintText("cbifont", CR_WHITE, 300,2, 50 + 8*n,1); \
      n++

   PrintTextStr(p->name);
   PrintText("smallfnt", CR_LIGHTBLUE, 20,1, 40,1);

   Stat(L(LANG "STAT_ScoreMul"),   "%i%%", ceilk(p->scoremul * 100.0));
   Stat(L(LANG "STAT_Weapons"),    "%i",   p->weaponsheld);
   Stat(L(LANG "STAT_HealthUsed"), "%u",   p->healthused);
   Stat(L(LANG "STAT_HealthSum"),  "%u",   p->healthsum);
   Stat(L(LANG "STAT_ScoreUsed"),  "%lli", p->scoreused);
   Stat(L(LANG "STAT_ScoreSum"),   "%lli", p->scoresum);
   Stat(L(LANG "STAT_Secrets"),    "%i",   world.secretsfound);
   Stat(L(LANG "STAT_Units"),      "%imu", p->unitstravelled);
   Stat(L(LANG "STAT_Upgrades"),   "%i",   p->upgradesowned);
   Stat(L(LANG "STAT_Items"),      "%i",   p->itemsbought);
   Stat(L(LANG "STAT_Mail"),       "%i",   p->bip.mailreceived);
   Stat(L(LANG "STAT_Time"),       "%li",  p->ticks / 35l);
   Stat(L(LANG "STAT_Boom"),       "%i",   p->spuriousexplosions);
   Stat(L(LANG "STAT_Brouzouf"),   "%i",   p->brouzouf);
   Stat(L(LANG "STAT_TrueMail"),   "%i",   p->bip.mailtrulyreceived);

   #undef Stat
}

// EOF
