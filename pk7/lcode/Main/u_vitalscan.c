// Required for sincos(3).
#define _GNU_SOURCE
#include "lith_upgrades_common.h"
#include "lith_world.h"
#include "lith_monster.h"

#define UData UData_VitalScan(upgr)


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Update
//
[[__call("ScriptS")]]
void Upgr_VitalScan_Update(player_t *p, upgrade_t *upgr)
{
   ACS_SetActivator(0, AAPTR_PLAYER_GETTARGET);

   bool six = ACS_StrCmp(ACS_GetActorClass(0), "RLDeVileSix", 11) == 0;

   bool validtarget =
      six ||
      ACS_CheckFlag(0, "COUNTKILL") ||
      ACS_PlayerNumber() != -1;

   if(ACS_GetActorProperty(0, APROP_Health) <= 0) {
      UData.target = UData.oldtarget = 0;
   } else if(validtarget) {
      bool legendary = world.legendoom && ACS_CheckInventory("LDLegendaryMonsterToken");
      bool henshin   = world.legendoom && ACS_CheckInventory("LDLegendaryMonsterTransformed");

      bool phantom = ACS_CheckInventory("Lith_IsPhantom");

      bool freak  = ACS_CheckFlag(0, "INVULNERABLE") || ACS_CheckFlag(0, "NODAMAGE");
      bool boss   = ACS_CheckFlag(0, "BOSS");
      bool shadow = ACS_CheckFlag(0, "SHADOW");

      int chp = ACS_GetActorProperty(0, APROP_Health);
      int shp = ACS_GetActorProperty(0, APROP_SpawnHealth);

      int id = Lith_UniqueID();
      dmon_t *m = DmonPtr();

      if((freak || boss) && !phantom) {
         extern __str Lith_RandomName(int id);

         UData.tagstr    = Lith_RandomName(freak ? 0 : id);
         UData.oldhealth = UData.health = ACS_Random(0, 666666);
         UData.maxhealth = ACS_Random(0, 666666);
      } else {
         char color = p->getCVarI("lith_scanner_color") & 0x7F;

              if(six)     UData.tagstr = "\Cg6";
         else if(henshin) UData.tagstr = StrParam("\CgLegendary\C%c %tS", color, 0);
         else if(phantom) UData.tagstr = StrParam("\Cg%tS", 0);
         else             UData.tagstr = StrParam("\C%c%tS", color, 0);

         UData.oldhealth = UData.health;
         UData.health    = chp;

         if(m)
            UData.maxhealth = m->maxhealth;
         else
            UData.maxhealth = shp;
      }

      if(m) {
         int level  = shadow ? m->level - ACS_Random(-5, 5) : m->level;
         int nsplit = min(m->maxhealth / (fixed)shp, 7);
         int split  = ceilk((chp / (fixed)m->maxhealth) * nsplit);
         int splith = m->maxhealth / (fixed)nsplit;
         UData.tagstr    = StrParam("%S lv.%i", UData.tagstr, level);
         UData.rank      = m->rank;
         UData.splitfrac = (chp - (splith * (split - 1))) / (fixed)splith;
         UData.split     = minmax(split, 1, 7);
      } else {
         UData.splitfrac = chp / (fixed)shp;
         UData.split = 1;
      }

      UData.freak  = six || freak || phantom || boss;
      UData.cangle = atan2f(p->y - ACS_GetActorY(0), p->x - ACS_GetActorX(0));

      if((UData.oldtarget = UData.target) != (UData.target = id))
         UData.oldhealth = UData.health;
   }
}

//
// Render
//
void Upgr_VitalScan_Render(player_t *p, upgrade_t *upgr)
{
   if(!p->hudenabled || !UData.target)
      return;

   int ox = p->getCVarI("lith_scanner_xoffs");
   int oy = p->getCVarI("lith_scanner_yoffs");

   if(p->getCVarI("lith_scanner_slide")) {
      float diff = p->yawf - UData.cangle;
      float ds, dc;
      sincosf(diff, &ds, &dc);
      UData.oangle = lerpf(UData.oangle, atan2f(ds, dc), 0.1);
      ox += UData.oangle * 64;
   }

   // Rank
   if(UData.rank) {
      for(int i = 1; i <= UData.rank; i++) {
         DrawSpriteFade(StrParam("lgfx/UI/Rank%i.png", i),
            hid_vscrankS - (i - 1), 100.1 + ox + (i * 6), 180.1 + oy, 0.1, 0.1);
      }
   }

   // Hit indicator
   if(UData.health < UData.oldhealth) {
      int delta = UData.oldhealth - UData.health;

      HudMessageF("CBIFONT", "-%i", delta);
      HudMessageParams(HUDMSG_FADEOUT, hid_vschitS, CR_RED, 160.4 + ox, 196.2 + oy, 0.1, 0.4);

      for(int i = 1; i < 5 && delta >= 100 * i; i++) {
         HudMessageF("CBIFONT", "-%i", delta);
         HudMessageParams(HUDMSG_FADEOUT|HUDMSG_ADDBLEND, hid_vschitS - i, CR_RED, 160.4 + ox, 196.2 + oy, 0.1, 0.4);
      }
   }

   // Tag and health
   bool  afnt = p->getCVarI("lith_scanner_altfont");
   __str font = afnt ? "SMALLFONT" : "CBIFONT";

   HudMessageF(font, "%S", UData.tagstr);
   HudMessageParams(HUDMSG_FADEOUT, hid_vsctag, CR_WHITE, 160.4 + ox, 180.2 + oy, 0.1, 0.4);

   HudMessageF(UData.freak ? "ALIENFONT" : font, "%i/%i", UData.health, UData.maxhealth);
   HudMessageParams(HUDMSG_FADEOUT, hid_vschp, CR_WHITE, 160.4 + ox, 188.2 + oy, 0.1, 0.4);

   // Health bar
   if(p->getCVarI("lith_scanner_bar")) {
      int y = afnt ? 168 : 171;
      ACS_SetHudClipRect(120 + ox, y + oy, 80 * UData.splitfrac, 2);
      DrawSpritePlain(StrParam("lgfx/UI/HealthBar%i.png", UData.split), hid_vscbar, 120.1+ox, y+.1+oy, 0.1);
      ACS_SetHudClipRect(0, 0, 0, 0);

      if(UData.split > 1)
         DrawSpritePlain(StrParam("lgfx/UI/HealthBar%i.png", UData.split - 1), hid_vscbarn, 120.1+ox, y+.1+oy, 0.1);
   }
}

// EOF

