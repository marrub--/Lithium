// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"
#include "lith_gui.h"

StrEntOFF

void Lith_CBITab_Statistics(gui_state_t *g, struct player *p)
{
   int n = 0, l;
   char const *s;

   #define Stat(name, f, x) \
      s = name; \
      l = strlen(s); \
      PrintTextChr(s, l); PrintText(s_cbifont, CR_WHITE, 23,1,  50 + 8*n,1); \
      PrintTextFmt(f, x); PrintText(s_cbifont, CR_WHITE, 300,2, 50 + 8*n,1); \
      n++

   PrintTextStr(p->name);
   PrintText(s_smallfnt, CR_LIGHTBLUE, 20,1, 40,1);

   Stat(LC(LANG "STAT_ScoreMul"),   "%i%%", ceilk(p->scoremul * 100.0));
   Stat(LC(LANG "STAT_Weapons"),    "%i",   p->weaponsheld);
   Stat(LC(LANG "STAT_HealthUsed"), "%u",   p->healthused);
   Stat(LC(LANG "STAT_HealthSum"),  "%u",   p->healthsum);
   Stat(LC(LANG "STAT_ScoreUsed"),  "%lli", p->scoreused);
   Stat(LC(LANG "STAT_ScoreSum"),   "%lli", p->scoresum);
   Stat(LC(LANG "STAT_Secrets"),    "%i",   world.secretsfound);
   Stat(LC(LANG "STAT_Units"),      "%imu", p->unitstravelled);
   Stat(LC(LANG "STAT_Upgrades"),   "%i",   p->upgradesowned);
   Stat(LC(LANG "STAT_Items"),      "%i",   p->itemsbought);
   Stat(LC(LANG "STAT_Mail"),       "%i",   p->bip.mailreceived);
   Stat(LC(LANG "STAT_Time"),       "%li",  p->ticks / 35l);
   Stat(LC(LANG "STAT_Boom"),       "%i",   p->spuriousexplosions);
   Stat(LC(LANG "STAT_Brouzouf"),   "%i",   p->brouzouf);
   Stat(LC(LANG "STAT_TrueMail"),   "%i",   p->bip.mailtrulyreceived);
}

// EOF
