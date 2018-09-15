// Copyright © 2017 Graham Sanderson, all rights reserved.
// vim: columns=120
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

#define Upgr(name) if(world.cbiupgr[cupg_##name])

#define InfoStart int y  = 70
#define InfoSep       y += 20

#define Info(...) \
   do { \
      PrintTextFmt(__VA_ARGS__); \
      PrintText("CBIFONT", CR_WHITE, 23,1, y,1); \
      y += 10; \
   } while(0)

#define Slot(name, x, y) \
   PrintSprite(":UI:" name "", 300-x*48,2, 48*y-20,1)

#define CPU(num) \
   PrintSprite(":UI:CPU" #num "", 0,1, 0,1)

// Static Functions ----------------------------------------------------------|

//
// CBITab_Marine
//
static void CBITab_Marine(gui_state_t *g, struct player *p)
{
   int   ram;
   __str name;

   Upgr(hasupgr2)
      {CPU(1); ram = 150; name = Language("LITH_TXT_INFO_TITLE_CBIUpgr2");}
   else Upgr(hasupgr1)
      {CPU(2); ram = 100; name = Language("LITH_TXT_INFO_TITLE_CBIUpgr1");}
   else
      {CPU(3); ram =  50; name = "OFMD Spec. Nodea 541 Basic CPU";}

   PrintTextStr(name);
   PrintText("CBIFONT", CR_WHITE, 20,1, 60,1);

   InfoStart;

   Info("Performance: %i\CbPerf", world.cbiperf);
   Info("In use: %i\CbPerf", p->cbi.pruse);
   Info("RAM: %iTiB", ram);

   InfoSep;

   Upgr(armorinter) Info("Has Armor Interface");
   Upgr(weapninter) Info("Has Weapon Modification Device");
   Upgr(weapninte2) Info("Has Weapon Refactoring Device");
   Upgr(rdistinter) Info("Has Reality Distortion Interface");

   Upgr(armorinter) Slot("ArmorInter", 0, 1);
   Upgr(weapninter) Slot("WeapnInter", 0, 2);
   Upgr(weapninte2) Slot("WeapnInte2", 0, 3);
   Upgr(rdistinter) Slot("RDistInter", 0, 4);
}

//
// CBITab_CyberMage
//
static void CBITab_CyberMage(gui_state_t *g, struct player *p)
{
   CPU(2);
   PrintTextStr("AOF 5900001 Rev7 CPU");
   PrintText("CBIFONT", CR_WHITE, 20,1, 60,1);

   InfoStart;

   Info("Performance: 34\CbPerf");
   Info("In use: %i\CbPerf", p->cbi.pruse);
   Info("RAM: 19TiB");

   InfoSep;

                      Info("\CiBlade Spell Driver \Cbinstalled.");
                      Info("\CiDelear Spell Driver \Cbinstalled.");
   Upgr(c_slot3spell) Info("\CiFeuer Spell Driver \Cbinstalled.");
   Upgr(c_slot4spell) Info("\CiRomyetya Spell Driver \Cbinstalled.");
   Upgr(c_slot5spell) Info("\CiHulgyon Spell Driver \Cbinstalled.");
   Upgr(c_slot6spell) Info("\CiHosh'danma Spell Driver \Cbinstalled.");
   Upgr(c_slot7spell) Info("\CgCercle de la Mort Spell Driver \Cbinstalled.");
   Upgr(c_rdistinter) Info("Reality Distortion Interface \Cbinstalled.");

                      Slot("Slot1Spell", 0, 1);
                      Slot("Slot2Spell", 0, 2);
   Upgr(c_slot3spell) Slot("Slot3Spell", 0, 3);
   Upgr(c_slot4spell) Slot("Slot4Spell", 0, 4);
   Upgr(c_slot5spell) Slot("Slot5Spell", 1, 1);
   Upgr(c_slot6spell) Slot("Slot6Spell", 1, 2);
   Upgr(c_slot7spell) Slot("Slot7Spell", 1, 3);
   Upgr(c_rdistinter) Slot("RDistInter", 1, 4);
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_CBITab_CBI
//
void Lith_CBITab_CBI(gui_state_t *g, struct player *p)
{
   switch(p->pclass)
   {
   case pcl_marine:    CBITab_Marine   (g, p); break;
   case pcl_cybermage: CBITab_CyberMage(g, p); break;
   }
}


// EOF
