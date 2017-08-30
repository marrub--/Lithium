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
};

// Static Functions ----------------------------------------------------------|

//
// RA_Take
//
static void RA_Take(int n)
{
   ACS_TakeInventory(StrParam("Lith_RA_Bullets%i", n),   999);
   ACS_TakeInventory(StrParam("Lith_RA_Energy%i", n),    999);
   ACS_TakeInventory(StrParam("Lith_RA_Fire%i", n),      999);
   ACS_TakeInventory(StrParam("Lith_RA_FireMagic%i", n), 999);
   ACS_TakeInventory(StrParam("Lith_RA_Magic%i", n),     999);
   ACS_TakeInventory(StrParam("Lith_RA_Melee%i", n),     999);
   ACS_TakeInventory(StrParam("Lith_RA_Shrapnel%i", n),  999);
   ACS_TakeInventory(StrParam("Lith_RA_Ice%i", n),       999);
}

//
// RA_Give
//
static void RA_Give(__str name, int n)
{
   ACS_GiveInventory(StrParam("Lith_RA_%S%i", name, n), 1);
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_RA_Give
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_RA_Give(int num)
{
   player_t *p = LocalPlayer;
   if(NoPlayer(p)) return;

   upgrade_t *upgr = p->getUpgr(UPGR_ReactArmor);

   if(!upgr->active)
      return;

   if(UData.activearmor != num + 1)
   {
      __str name = ArmorNames[num].full;

      UData.activearmor = num + 1;

      RA_Take(1);
      RA_Take(2);

      ACS_LocalAmbientSound("player/rarmor/mode", 127);

      p->logH(">>>>> Activating Armor->%S()", name);

      if(p->getUpgr(UPGR_ReactArmor2)->active)
         RA_Give(name, 2);
      else
         RA_Give(name, 1);
   }
}

//
// Deactivate
//
void Upgr_ReactArmor_Deactivate(player_t *p, upgrade_t *upgr)
{
   UData.activearmor = 0;

   RA_Take(1);
   RA_Take(2);
}

//
// Render
//
void Upgr_ReactArmor_Render(player_t *p, upgrade_t *upgr)
{
   if(UData.activearmor && p->getCVarI("lith_hud_showarmorind"))
   {
      DrawSpritePlain("lgfx/HUD/SplitLeft.png", hid_rarmorbg, 12.1, 170.2, TICSECOND);

      HudMessageF("LHUDFONT", "\Cn%S", ArmorNames[UData.activearmor - 1].abbr);
      HudMessagePlain(hid_rarmortxt, 32.1, 161.0, TICSECOND);
   }
}

// EOF

