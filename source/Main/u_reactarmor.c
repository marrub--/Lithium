// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

StrEntOFF

#define UData UData_ReactArmor(upgr)

// Static Objects ------------------------------------------------------------|

static struct {char const *abbr, *full;} const ArmorNames[] = {
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
   InvTake(StrParam(OBJ "RA_Bullets%i", n),   999);
   InvTake(StrParam(OBJ "RA_Energy%i", n),    999);
   InvTake(StrParam(OBJ "RA_Fire%i", n),      999);
   InvTake(StrParam(OBJ "RA_FireMagic%i", n), 999);
   InvTake(StrParam(OBJ "RA_Magic%i", n),     999);
   InvTake(StrParam(OBJ "RA_Melee%i", n),     999);
   InvTake(StrParam(OBJ "RA_Shrapnel%i", n),  999);
   InvTake(StrParam(OBJ "RA_Ice%i", n),       999);
}

static void RA_Give(char const *name, int n)
{
   InvGive(StrParam(OBJ "RA_%s%i", name, n), 1);
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
         char const *name = ArmorNames[num].full;

         UData.activearmor = num + 1;

         RA_Take(1);
         RA_Take(2);

         ACS_LocalAmbientSound(s"player/rarmor/mode", 127);

         p->logH(3, s"Activating Armor->%s()", name);

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
   if(UData.activearmor && p->getCVarI(sCVAR "hud_showarmorind"))
   {
      PrintSprite(s":HUD:SplitLeft", 12,1, 225,2);

      PrintTextChr(ArmorNames[UData.activearmor - 1].abbr, 3);
      PrintTextX(s"lhudfont", CR_LIGHTBLUE, 32,1, 216,0);
   }
}

// EOF

