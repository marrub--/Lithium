/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * ReactArmor upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

#define UData p->upgrdata.reactarmor

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
   InvTake(StrParam(OBJ "RA_Bullets%i", n),   INT32_MAX);
   InvTake(StrParam(OBJ "RA_Energy%i", n),    INT32_MAX);
   InvTake(StrParam(OBJ "RA_Fire%i", n),      INT32_MAX);
   InvTake(StrParam(OBJ "RA_FireMagic%i", n), INT32_MAX);
   InvTake(StrParam(OBJ "RA_Magic%i", n),     INT32_MAX);
   InvTake(StrParam(OBJ "RA_Melee%i", n),     INT32_MAX);
   InvTake(StrParam(OBJ "RA_Shrapnel%i", n),  INT32_MAX);
   InvTake(StrParam(OBJ "RA_Ice%i", n),       INT32_MAX);
}

static void RA_Give(cstr name, i32 n)
{
   InvGive(StrParam(OBJ "RA_%s%i", name, n), 1);
}

/* Extern Functions -------------------------------------------------------- */

void Upgr_ReactArmor_Deactivate(struct player *p, struct upgrade *upgr)
{
   UData.activearmor = 0;

   RA_Take(1);
   RA_Take(2);
}

void Upgr_ReactArmor_Render(struct player *p, struct upgrade *upgr)
{
   if(UData.activearmor && p->getCVarI(sc_hud_showarmorind))
   {
      PrintSprite(sp_HUD_SplitLeft, 12,1, 225,2);

      PrintTextChr(ArmorNames[UData.activearmor - 1].abbr, 3);
      PrintTextX(s_bigupper, CR_LIGHTBLUE, 32,1, 216,0, ptf_no_utf);
   }
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "RA_Give")
void Sc_GiveRA(i32 num)
{
   with_player(LocalPlayer)
   {
      if(!get_bit(p->upgrades[UPGR_ReactArmor].flags, _ug_active)) return;

      if(UData.activearmor != num + 1)
      {
         cstr name = ArmorNames[num].full;

         UData.activearmor = num + 1;

         RA_Take(1);
         RA_Take(2);

         ACS_LocalAmbientSound(ss_player_rarmor_mode, 127);

         p->logH(3, LC(LANG "LOG_ActiveArmor"), name);

         if(get_bit(p->upgrades[UPGR_ReactArmor2].flags, _ug_active))
            RA_Give(name, 2);
         else
            RA_Give(name, 1);
      }
   }
}

/* EOF */
