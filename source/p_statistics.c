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

   #define Stat(name, ...) \
   statement({ \
      PrintText_str(name, sf_lmidfont, g->defcr, g->ox+17,1, g->oy+27 + 8*n,1); \
      ACS_BeginPrint(); \
      (__VA_ARGS__); \
      PrintText(sf_smallfnt, g->defcr, g->ox+267,2, g->oy+27 + 8*n,1); \
      n++; \
   })

   Stat(sl_stat_scoremul,   _p(pl.scoremul), _c('%'));
   Stat(sl_stat_weapons,    _p(pl.weaponsheld));
   Stat(sl_stat_healthused, _p(pl.healthused));
   Stat(sl_stat_healthsum,  _p(pl.healthsum));
   Stat(sl_stat_scoreused,  _p(scoresep(pl.scoreused)));
   Stat(sl_stat_scoresum,   _p(scoresep(pl.scoresum)));
   Stat(sl_stat_secrets,    _p(wl.secretsfound));
   Stat(sl_stat_units,      _p(pl.unitstravelled), _l("mu"));
   Stat(sl_stat_upgrades,   _p(pl.upgradesowned));
   Stat(sl_stat_items,      _p(pl.itemsbought));
   Stat(sl_stat_mail,       _p(bip.mailreceived));
   Stat(sl_stat_time,       _p(wl.realtime / 35.0k));
   if(get_bit(pcl_outcasts, pl.pclass)) {
      Stat(sl_stat_boom, _p(pl.spuriousexplosions));
   }
   if(pl.pclass == pcl_marine) {
      Stat(sl_stat_brouzouf, _p(pl.brouzouf));
   }
   Stat(sl_stat_truemail, _p(bip.mailtrulyreceived));
}

/* EOF */
