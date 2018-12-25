// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"
#include "lith_world.h"
#include "lith_monster.h"

#include <math.h>

StrEntON

#define UData UData_VitalScan(upgr)

// Extern Functions ----------------------------------------------------------|

script
void Upgr_VitalScan_Update(struct player *p, upgrade_t *upgr)
{
   ACS_SetActivator(0, AAPTR_PLAYER_GETTARGET);

   bool six = ACS_StrCmp(ACS_GetActorClass(0), "RLDeVileSix", 11) == 0;

   bool validtarget =
      six ||
      ACS_CheckFlag(0, s_COUNTKILL) ||
      ACS_PlayerNumber() != -1;

   if(GetPropI(0, APROP_Health) <= 0)
      UData = (struct upgr_data_VitalScan){};
   else if(validtarget)
   {
      bool legendary = world.legendoom && InvNum("LDLegendaryMonsterToken");
      bool henshin   = world.legendoom && InvNum("LDLegendaryMonsterTransformed");

      bool phantom = InvNum(OBJ "IsPhantom");

      bool freak  = ACS_CheckFlag(0, "INVULNERABLE") || ACS_CheckFlag(0, "NODAMAGE");
      bool boss   = ACS_CheckFlag(0, "BOSS");
      bool shadow = ACS_CheckFlag(0, "SHADOW");

      int chp = GetPropI(0, APROP_Health);
      int shp = GetPropI(0, APROP_SpawnHealth);

      int id = Lith_UniqueID();
      #if LITHIUM
      dmon_t const *const m = DmonSelf();
      #endif

      int ot = UData.target;

      if(UData.target != id)
      {
         memset(&UData, 0, sizeof UData);
         UData.oldhealth = UData.health;
      }

      UData.oldtarget = ot;
      UData.target    = id;

      bool healthset = false;

      if((freak || boss) && !phantom)
      {
         extern __str Lith_RandomName(int id);

         UData.tagstr = Lith_RandomName(freak ? 0 : id);

         if(p->getCVarI(sCVAR "scanner_bar"))
         {
            UData.oldhealth = UData.health = ACS_Random(0, 666666);
            UData.maxhealth = ACS_Random(0, 666666);
            healthset = true;
         }
      }
      else
      {
         char color = p->getCVarI(sCVAR "scanner_color") & 0x7F;

              if(six)     UData.tagstr = "\Cg6";
         else if(henshin) UData.tagstr = StrParam(c"\CgLegendary\C%c %tS", color, 0);
         else if(phantom) UData.tagstr = StrParam(c"\Cg%tS", 0);
         else             UData.tagstr = StrParam(c"\C%c%tS", color, 0);
      }

      if(!healthset)
      {
         UData.oldhealth = UData.health;
         UData.health    = chp;
         #if LITHIUM
         UData.maxhealth = m ? m->maxhealth : shp;
         #else
         UData.maxhealth = shp;
         #endif
      }

      #if LITHIUM
      if(m && shp && m->maxhealth)
      {
         int level  = shadow ? m->level - ACS_Random(-5, 5) : m->level;
         int nsplit = min(m->maxhealth / (fixed)shp, 7);
         int split  = ceilk(chp / (fixed)m->maxhealth * nsplit);
         int splith = m->maxhealth / (fixed)nsplit;
         UData.tagstr    = StrParam(c"%S lv.%i", UData.tagstr, level);
         UData.rank      = m->rank;
         UData.splitfrac = (chp - (splith * (split - 1))) / (fixed)splith;
         UData.split     = minmax(split, 1, 7);
      }
      else
      #endif
      {
         UData.rank = 0;
         UData.splitfrac = chp / (fixed)shp;
         UData.split = 1;
      }

      UData.freak  = six || freak || phantom || boss;
      UData.cangle = ACS_VectorAngle(p->x - GetX(0), p->y - GetY(0)) * tau;

      // Hit indicator
      if(UData.hdtime != 0) UData.hdtime--;
      else                  UData.hdelta = 0;

      if(UData.health < UData.oldhealth) {
         UData.hdelta = UData.oldhealth - UData.health;
         UData.hdtime = 30;
      }
   }
}

stkcall
void Upgr_VitalScan_Render(struct player *p, upgrade_t *upgr)
{
   if(!p->hudenabled || !UData.target) return;

   if(UData.hdtime == 30) SetFade(fid_vscan, 10, 0.1);

   int ox = p->getCVarI(sCVAR "scanner_xoffs");
   int oy = p->getCVarI(sCVAR "scanner_yoffs");

   if(p->getCVarI(sCVAR "scanner_slide"))
   {
      fixed64 diff = p->yawf - (fixed64)UData.cangle;
      fixed ds = ACS_Sin(diff / tau) * tau;
      fixed dc = ACS_Cos(diff / tau) * tau;
      UData.oangle = lerplk(UData.oangle, atan2f(ds, dc), 0.1);
      ox += UData.oangle * 64;
   }

   // Rank
   if(UData.rank) for(int i = 1; i <= UData.rank; i++)
      PrintSprite(StrParam(c":UI:Rank%i", i), 100+ox + i*6,1, 216+oy,1);

   // Hit indicator
   if(UData.hdelta && CheckFade(fid_vscan)) {
      PrintTextFmt(c"-%i", UData.hdelta);
      PrintTextFX(s_cbifont, CR_RED, 160+ox,4, 235+oy,2, fid_vscan);
   }

   // Tag and health
   bool  afnt = p->getCVarI(sCVAR "scanner_altfont");
   __str font = afnt ? s_chfont : s_cbifont;

   PrintTextStr(UData.tagstr);
   PrintTextX(font, CR_WHITE, 160+ox,4, 216+oy,2);

   if(UData.maxhealth) PrintTextFmt(c"%u/%u", UData.health, UData.maxhealth);
   else                PrintTextFmt(c"%uhp",  UData.health);
   PrintTextX(UData.freak ? s_alienfont : font, CR_WHITE, 160+ox,4, 225+oy,2);

   // Health bar
   if(p->getCVarI(sCVAR "scanner_bar"))
   {
      int y = afnt ? 201 : 205;
      SetClip(120 + ox, y + oy, 80 * UData.splitfrac, 2);
      PrintSprite(StrParam(c":UI:HealthBar%i", UData.split), 120+ox,1, y+oy,1);
      ClearClip();

      if(UData.split > 1)
         PrintSprite(StrParam(c":UI:HealthBar%i", UData.split - 1), 120+ox,1, y+oy,1);
   }
}

// EOF
