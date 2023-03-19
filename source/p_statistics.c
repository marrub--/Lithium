// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Statistics tab.                                                          │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"
#include "gui.h"

void P_CBI_TabStatistics(struct gui_state *g) {
   i32 n = 0;

   #define Stat(name, f, x) \
   statement({ \
      PrintText_str(ns(lang(name)), sf_lmidfont, g->defcr, g->ox+17,1,  g->oy+27 + 8*n,1); \
      BeginPrintFmt(f, x); \
      PrintText(sf_smallfnt, g->defcr, g->ox+267,2, g->oy+27 + 8*n,1); \
      n++; \
   })

   Stat(sl_stat_scoremul,   "%i%%",      pl.scoremul);
   Stat(sl_stat_weapons,    "%i",        pl.weaponsheld);
   Stat(sl_stat_healthused, "%u",        pl.healthused);
   Stat(sl_stat_healthsum,  "%u",        pl.healthsum);
   Stat(sl_stat_scoreused,  "%" FMT_SCR, pl.scoreused);
   Stat(sl_stat_scoresum,   "%" FMT_SCR, pl.scoresum);
   Stat(sl_stat_secrets,    "%i",        wl.secretsfound);
   Stat(sl_stat_units,      "%imu",      pl.unitstravelled);
   Stat(sl_stat_upgrades,   "%i",        pl.upgradesowned);
   Stat(sl_stat_items,      "%i",        pl.itemsbought);
   Stat(sl_stat_mail,       "%i",        bip.mailreceived);
   Stat(sl_stat_time,       "%i",        pl.ticks / 35);
   if(get_bit(pcl_outcasts, pl.pclass)) {
      Stat(sl_stat_boom, "%i", pl.spuriousexplosions);
   }
   if(pl.pclass == pcl_marine) {
      Stat(sl_stat_brouzouf, "%i", pl.brouzouf);
   }
   Stat(sl_stat_truemail, "%i", bip.mailtrulyreceived);
}

/* EOF */
