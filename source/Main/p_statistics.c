// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"
#include "lith_gui.h"

void Lith_CBITab_Statistics(gui_state_t *g, struct player *p)
{
   int n = 0;

   #define Stat(name, f, x) \
      PrintTextStr(name); PrintText("cbifont", CR_WHITE, 23,1,  50 + 8*n,1); \
      PrintTextFmt(f, x); PrintText("cbifont", CR_WHITE, 300,2, 50 + 8*n,1); \
      n++

   PrintTextStr(p->name);
   PrintText("SMALLFNT", CR_LIGHTBLUE, 20,1, 40,1);

   Stat(L("LITH_STAT_ScoreMul"),   "%i%%", ceilk(p->scoremul * 100.0));
   Stat(L("LITH_STAT_Weapons"),    "%i",   p->weaponsheld);
   Stat(L("LITH_STAT_HealthUsed"), "%u",   p->healthused);
   Stat(L("LITH_STAT_HealthSum"),  "%u",   p->healthsum);
   Stat(L("LITH_STAT_ScoreUsed"),  "%lli", p->scoreused);
   Stat(L("LITH_STAT_ScoreSum"),   "%lli", p->scoresum);
   Stat(L("LITH_STAT_Secrets"),    "%i",   world.secretsfound);
   Stat(L("LITH_STAT_Units"),      "%imu", p->unitstravelled);
   Stat(L("LITH_STAT_Upgrades"),   "%i",   p->upgradesowned);
   Stat(L("LITH_STAT_Items"),      "%i",   p->itemsbought);
   Stat(L("LITH_STAT_Mail"),       "%i",   p->bip.mailreceived);
   Stat(L("LITH_STAT_Time"),       "%li",  p->ticks / 35l);
   Stat(L("LITH_STAT_Boom"),       "%i",   p->spuriousexplosions);
   Stat(L("LITH_STAT_Brouzouf"),   "%i",   p->brouzouf);
   Stat(L("LITH_STAT_TrueMail"),   "%i",   p->bip.mailtrulyreceived);

   #undef Stat
}

// EOF
