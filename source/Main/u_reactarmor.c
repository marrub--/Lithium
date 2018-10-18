// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

#define UData UData_ReactArmor(upgr)

// Static Objects ------------------------------------------------------------|

static struct {__str abbr, full;} const ArmorNames[] = {
   "BUL", "Bullets",
   "ENE", "Energy",
   "FIR", "Fire",
   "XMG", "FireMagic",
   "MAG", "Magic",
   "MEL", "Melee",
   "SHR", "Shrapnel",
   "ICE", "Ice",
   "HZD", "Hazard",
};

// Static Functions ----------------------------------------------------------|

static void RA_Take(int n)
{
   InvTake(StrParam("Lith_RA_Bullets%i", n),   999);
   InvTake(StrParam("Lith_RA_Energy%i", n),    999);
   InvTake(StrParam("Lith_RA_Fire%i", n),      999);
   InvTake(StrParam("Lith_RA_FireMagic%i", n), 999);
   InvTake(StrParam("Lith_RA_Magic%i", n),     999);
   InvTake(StrParam("Lith_RA_Melee%i", n),     999);
   InvTake(StrParam("Lith_RA_Shrapnel%i", n),  999);
   InvTake(StrParam("Lith_RA_Ice%i", n),       999);
}

static void RA_Give(__str name, int n)
{
   InvGive(StrParam("Lith_RA_%S%i", name, n), 1);
}

// Extern Functions ----------------------------------------------------------|

script ext("ACS")
void Lith_RA_Give(int num)
{
   withplayer(LocalPlayer)
   {
      if(!p->getUpgrActive(UPGR_ReactArmor))
         return;

      upgrade_t *upgr = p->getUpgr(UPGR_ReactArmor);

      if(UData.activearmor != num + 1)
      {
         __str name = ArmorNames[num].full;

         UData.activearmor = num + 1;

         RA_Take(1);
         RA_Take(2);

         ACS_LocalAmbientSound("player/rarmor/mode", 127);

         p->logH(3, "Activating Armor->%S()", name);

         if(p->getUpgrActive(UPGR_ReactArmor2)) RA_Give(name, 2);
         else                                   RA_Give(name, 1);
      }
   }
}

stkcall
void Upgr_ReactArmor_Deactivate(struct player *p, upgrade_t *upgr)
{
   UData.activearmor = 0;

   RA_Take(1);
   RA_Take(2);
}

stkcall
void Upgr_ReactArmor_Render(struct player *p, upgrade_t *upgr)
{
   if(UData.activearmor && p->getCVarI("lith_hud_showarmorind"))
   {
      PrintSprite(":HUD:SplitLeft", 12,1, 225,2);

      PrintTextStr(ArmorNames[UData.activearmor - 1].abbr);
      PrintText("lhudfont", CR_LIGHTBLUE, 32,1, 216,0);
   }
}

// EOF

