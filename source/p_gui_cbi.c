/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * CBI tab GUI.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "w_world.h"

#define Upgr(name) if(cbiupgr[cupg_##name])

#define InfoStart i32 y  = 57
#define InfoSep       y += 20

#define Info(...) \
   statement({ \
      PrintTextFmt(__VA_ARGS__); \
      PrintText(sf_smallfnt, g->defcr, g->ox+10,1, g->oy+y,1); \
      y += 10; \
   })

#define Slot(name, x, y) PrintSprite(name, g->ox+287-x*48,2, g->oy+48*y-33,1)
#define CPU(num)         PrintSprite(sp_UI_CPU##num, g->ox-13,1, g->oy-13,1)

/* Static Functions -------------------------------------------------------- */

static
void CBITab_Marine(struct gui_state *g) {
   i32 ram;
   str name;

        Upgr(hasupgr2) {CPU(1); ram = 150; name = ns(lang(sl_cbi_cpu3));}
   else Upgr(hasupgr1) {CPU(2); ram = 100; name = ns(lang(sl_cbi_cpu2));}
   else                {CPU(3); ram =  50; name = ns(lang(sl_cbi_cpu1));}

   PrintTextChS(name);
   PrintText_str(name, sf_smallfnt, g->defcr, g->ox+7,1, g->oy+47,1);

   InfoStart;

   Info(tmpstr(lang(sl_cbi_perf)), cbiperf);
   Info(tmpstr(lang(sl_cbi_puse)), pl.cbi.pruse);
   Info(tmpstr(lang(sl_cbi_wram)), ram);

   InfoSep;

   Info(tmpstr(lang(sl_cbi_interfaces)));
   Upgr(armorinter) Info("> %S", ns(lang(sl_cbi_armorinter)));
   Upgr(weapninter) Info("> %S", ns(lang(sl_cbi_weapninter)));
   Upgr(weapninte2) Info("> %S", ns(lang(sl_cbi_weapninte2)));
   Upgr(rdistinter) Info("> %S", ns(lang(sl_cbi_rdistinter)));

   Upgr(armorinter) Slot(sp_UI_ArmorInter, 0, 1);
   Upgr(weapninter) Slot(sp_UI_WeapnInter, 0, 2);
   Upgr(weapninte2) Slot(sp_UI_WeapnInte2, 0, 3);
   Upgr(rdistinter) Slot(sp_UI_RDistInter, 0, 4);
}

static
void CBITab_CyberMage(struct gui_state *g)
{
   CPU(2);
   PrintText_str(ns(lang(sl_cbi_cpu4)), sf_smallfnt, g->defcr, g->ox+7,1, g->oy+47,1);

   InfoStart;

   Info(tmpstr(lang(sl_cbi_perf)), 34);
   Info(tmpstr(lang(sl_cbi_puse)), pl.cbi.pruse);
   Info(tmpstr(lang(sl_cbi_wram)), 19);

   InfoSep;

   Info(tmpstr(lang(sl_cbi_interfaces)));
                      Info("> %S", ns(lang(sl_cbi_slot1spell)));
                      Info("> %S", ns(lang(sl_cbi_slot2spell)));
   Upgr(c_slot3spell) Info("> %S", ns(lang(sl_cbi_slot3spell)));
   Upgr(c_slot4spell) Info("> %S", ns(lang(sl_cbi_slot4spell)));
   Upgr(c_slot5spell) Info("> %S", ns(lang(sl_cbi_slot5spell)));
   Upgr(c_slot6spell) Info("> %S", ns(lang(sl_cbi_slot6spell)));
   Upgr(c_slot7spell) Info("> %S", ns(lang(sl_cbi_slot7spell)));
   Upgr(c_rdistinter) Info("> %S", ns(lang(sl_cbi_rdistinter)));

                      Slot(sp_UI_Slot1Spell, 0, 1);
                      Slot(sp_UI_Slot2Spell, 0, 2);
   Upgr(c_slot3spell) Slot(sp_UI_Slot3Spell, 0, 3);
   Upgr(c_slot4spell) Slot(sp_UI_Slot4Spell, 0, 4);
   Upgr(c_slot5spell) Slot(sp_UI_Slot5Spell, 1, 1);
   Upgr(c_slot6spell) Slot(sp_UI_Slot6Spell, 1, 2);
   Upgr(c_slot7spell) Slot(sp_UI_Slot7Spell, 1, 3);
   Upgr(c_rdistinter) Slot(sp_UI_RDistInter, 1, 4);
}

/* Extern Functions -------------------------------------------------------- */

void P_CBI_TabCBI(struct gui_state *g)
{
   switch(pl.pclass) {
   case pcl_marine:    CBITab_Marine   (g); break;
   case pcl_cybermage: CBITab_CyberMage(g); break;
   }
}

/* EOF */
