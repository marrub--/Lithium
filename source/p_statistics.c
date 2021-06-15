/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Statistics tab.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "w_world.h"
#include "gui.h"

void P_CBI_TabStatistics(struct gui_state *g)
{
   i32 n = 0;

   #define Stat(name, f, x) \
   statement({ \
      PrintTextChS(name); \
      PrintText(sf_lmidfont, g->defcr, g->ox+17,1,  g->oy+27 + 8*n,1); \
      PrintTextFmt(f, x); \
      PrintText(sf_smallfnt, g->defcr, g->ox+267,2, g->oy+27 + 8*n,1); \
      n++; \
   })

   Stat(LC(LANG "STAT_ScoreMul"),   "%i%%", ceilk(pl.scoremul * 100.0k));
   Stat(LC(LANG "STAT_Weapons"),    "%i",   pl.weaponsheld);
   Stat(LC(LANG "STAT_HealthUsed"), "%u",   pl.healthused);
   Stat(LC(LANG "STAT_HealthSum"),  "%u",   pl.healthsum);
   Stat(LC(LANG "STAT_ScoreUsed"),  "%lli", pl.scoreused);
   Stat(LC(LANG "STAT_ScoreSum"),   "%lli", pl.scoresum);
   Stat(LC(LANG "STAT_Secrets"),    "%i",   secretsfound);
   Stat(LC(LANG "STAT_Units"),      "%imu", pl.unitstravelled);
   Stat(LC(LANG "STAT_Upgrades"),   "%i",   pl.upgradesowned);
   Stat(LC(LANG "STAT_Items"),      "%i",   pl.itemsbought);
   Stat(LC(LANG "STAT_Mail"),       "%i",   pl.bip.mailreceived);
   Stat(LC(LANG "STAT_Time"),       "%li",  pl.ticks / 35L);
   Stat(LC(LANG "STAT_Boom"),       "%i",   pl.spuriousexplosions);
   Stat(LC(LANG "STAT_Brouzouf"),   "%i",   pl.brouzouf);
   Stat(LC(LANG "STAT_TrueMail"),   "%i",   pl.bip.mailtrulyreceived);
}

/* EOF */
