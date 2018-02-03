// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"
#include "lith_gui.h"

//
// Lith_CBITab_Statistics
//
void Lith_CBITab_Statistics(gui_state_t *g, player_t *p)
{
   int n = 0;

   #define Stat(name, f, x) \
      PrintTextStr(name); PrintText("CBIFONT", CR_WHITE, 23,1,  50 + 8*n,1); \
      PrintTextFmt(f, x); PrintText("CBIFONT", CR_WHITE, 300,2, 50 + 8*n,1); \
      n++

   PrintTextStr(p->name);
   PrintText("SMALLFNT", CR_LIGHTBLUE, 20,1, 40,1);

   Stat("Score Multiplier",    "%i%%", ceilk(p->scoremul * 100.0));
   Stat("Weapons Found",       "%i",   p->weaponsheld);
   Stat("Health Used",         "%li",  p->healthused);
   Stat("Health Sum",          "%li",  p->healthsum);
   Stat("Score Used",          "%lli", p->scoreused);
   Stat("Score Sum",           "%lli", p->scoresum);
   Stat("Secrets Found",       "%i",   world.secretsfound);
   Stat("Units Travelled",     "%imu", p->unitstravelled);
   Stat("Upgrades Owned",      "%i",   p->upgradesowned);
   Stat("Items Bought",        "%i",   p->itemsbought);
   Stat("Mail Received",       "%i",   p->bip.mailreceived);
   Stat("Seconds Elapsed",     "%li",  p->ticks / 35l);
   Stat("Spurious Explosions", "%i",   p->spuriousexplosions);
   Stat("Brouzouf Gained",     "%i",   p->brouzouf);
   Stat("Mail Truly Received", "%i",   p->bip.mailtrulyreceived);

   #undef Stat
}

// EOF
