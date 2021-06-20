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

#define Slot(name, x, y) \
   statement({ \
      Str(st_path, s":UI:" name); \
      PrintSprite(st_path, g->ox+287-x*48,2, g->oy+48*y-33,1); \
   })

#define CPU(num) \
   statement({ \
      Str(st_path, s":UI:CPU" #num); \
      PrintSprite(st_path, g->ox-13,1, g->oy-13,1); \
   })

/* Static Functions -------------------------------------------------------- */

static
void CBITab_Marine(struct gui_state *g)
{
   i32 ram;
   cstr name;

        Upgr(hasupgr2) {CPU(1); ram = 150; name = LC(LANG "CBI_CPU3");}
   else Upgr(hasupgr1) {CPU(2); ram = 100; name = LC(LANG "CBI_CPU2");}
   else                {CPU(3); ram =  50; name = LC(LANG "CBI_CPU1");}

   PrintTextChS(name);
   PrintText(sf_smallfnt, g->defcr, g->ox+7,1, g->oy+47,1);

   InfoStart;

   Info(LC(LANG "CBI_PERF"), cbiperf);
   Info(LC(LANG "CBI_PUSE"), pl.cbi.pruse);
   Info(LC(LANG "CBI_WRAM"), ram);

   InfoSep;

   Info(LC(LANG "CBI_INTERFACES"));
   Upgr(armorinter) Info("> %s", LC(LANG "CBI_ArmorInter"));
   Upgr(weapninter) Info("> %s", LC(LANG "CBI_WeapnInter"));
   Upgr(weapninte2) Info("> %s", LC(LANG "CBI_WeapnInte2"));
   Upgr(rdistinter) Info("> %s", LC(LANG "CBI_RDistInter"));

   Upgr(armorinter) Slot("ArmorInter", 0, 1);
   Upgr(weapninter) Slot("WeapnInter", 0, 2);
   Upgr(weapninte2) Slot("WeapnInte2", 0, 3);
   Upgr(rdistinter) Slot("RDistInter", 0, 4);
}

static
void CBITab_CyberMage(struct gui_state *g)
{
   cstr name = LC(LANG "CBI_CPU4");

   CPU(2);
   PrintTextChS(name);
   PrintText(sf_smallfnt, g->defcr, g->ox+7,1, g->oy+47,1);

   InfoStart;

   Info(LC(LANG "CBI_PERF"), 34);
   Info(LC(LANG "CBI_PUSE"), pl.cbi.pruse);
   Info(LC(LANG "CBI_WRAM"), 19);

   InfoSep;

   Info(LC(LANG "CBI_INTERFACES"));
                      Info("> %s", LC(LANG "CBI_Slot1Spell"));
                      Info("> %s", LC(LANG "CBI_Slot2Spell"));
   Upgr(c_slot3spell) Info("> %s", LC(LANG "CBI_Slot3Spell"));
   Upgr(c_slot4spell) Info("> %s", LC(LANG "CBI_Slot4Spell"));
   Upgr(c_slot5spell) Info("> %s", LC(LANG "CBI_Slot5Spell"));
   Upgr(c_slot6spell) Info("> %s", LC(LANG "CBI_Slot6Spell"));
   Upgr(c_slot7spell) Info("> %s", LC(LANG "CBI_Slot7Spell"));
   Upgr(c_rdistinter) Info("> %s", LC(LANG "CBI_RDistInter"));

                      Slot("Slot1Spell", 0, 1);
                      Slot("Slot2Spell", 0, 2);
   Upgr(c_slot3spell) Slot("Slot3Spell", 0, 3);
   Upgr(c_slot4spell) Slot("Slot4Spell", 0, 4);
   Upgr(c_slot5spell) Slot("Slot5Spell", 1, 1);
   Upgr(c_slot6spell) Slot("Slot6Spell", 1, 2);
   Upgr(c_slot7spell) Slot("Slot7Spell", 1, 3);
   Upgr(c_rdistinter) Slot("RDistInter", 1, 4);
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
