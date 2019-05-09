/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * ReactArmor upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

#define UData UData_ReactArmor(upgr)

/* Static Objects ---------------------------------------------------------- */

static struct {cstr abbr, full;} const ArmorNames[] = {
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

/* Static Functions -------------------------------------------------------- */

static void RA_Take(i32 n)
{
   InvTake(StrParam(OBJ "RA_Bullets%i", n),   INT_MAX);
   InvTake(StrParam(OBJ "RA_Energy%i", n),    INT_MAX);
   InvTake(StrParam(OBJ "RA_Fire%i", n),      INT_MAX);
   InvTake(StrParam(OBJ "RA_FireMagic%i", n), INT_MAX);
   InvTake(StrParam(OBJ "RA_Magic%i", n),     INT_MAX);
   InvTake(StrParam(OBJ "RA_Melee%i", n),     INT_MAX);
   InvTake(StrParam(OBJ "RA_Shrapnel%i", n),  INT_MAX);
   InvTake(StrParam(OBJ "RA_Ice%i", n),       INT_MAX);
}

static void RA_Give(cstr name, i32 n)
{
   InvGive(StrParam(OBJ "RA_%s%i", name, n), 1);
}

/* Extern Functions -------------------------------------------------------- */

stkcall
void Upgr_ReactArmor_Deactivate(struct player *p, struct upgrade *upgr)
{
   UData.activearmor = 0;

   RA_Take(1);
   RA_Take(2);
}

stkcall
void Upgr_ReactArmor_Render(struct player *p, struct upgrade *upgr)
{
   if(UData.activearmor && p->getCVarI(sc_hud_showarmorind))
   {
      PrintSprite(sp_HUD_SplitLeft, 12,1, 225,2);

      PrintTextChr(ArmorNames[UData.activearmor - 1].abbr, 3);
      PrintTextX(s_lhudfont, CR_LIGHTBLUE, 32,1, 216,0);
   }
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr("Lith_RA_Give")
void Sc_GiveRA(i32 num)
{
   with_player(LocalPlayer)
   {
      if(!p->getUpgrActive(UPGR_ReactArmor))
         return;

      struct upgrade *upgr = p->getUpgr(UPGR_ReactArmor);

      if(UData.activearmor != num + 1)
      {
         cstr name = ArmorNames[num].full;

         UData.activearmor = num + 1;

         RA_Take(1);
         RA_Take(2);

         ACS_LocalAmbientSound(ss_player_rarmor_mode, 127);

         p->logH(3, "Activating Armor->%s()", name); /* TODO */

         if(p->getUpgrActive(UPGR_ReactArmor2)) RA_Give(name, 2);
         else                                   RA_Give(name, 1);
      }
   }
}

/* EOF */
