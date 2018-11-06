// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"
#include "lith_hud.h"

// Static Functions ----------------------------------------------------------|

static void HUD_Ammo(struct player *p)
{
   invweapon_t const *wep = p->weapon.cur;

   int type = 0;

   __str typegfx = null;

   if(wep->ammotype & AT_NMag) type |= 1;
   if(wep->ammotype & AT_Ammo && !(wep->info->flags & wf_magic)) type |= 2;

   if(type)
      PrintSprite(":HUD_C:SplitFront", 320,2, 238,2);

   if(type & 1)
   {
      typegfx = ":HUD_C:MAG";

      if(type & 2 && !wep->ammocur)
         PrintTextStr("\C[Lith_Red]OUT");
      else
         PrintTextFmt("\C[Lith_Red]%i/%i", wep->magmax - wep->magcur, wep->magmax);
      PrintText("lhudfont", 0, 242,1, 229,0);
   }

   if(type & 2)
   {
      typegfx = ":HUD_C:AMMO";

      int x = 0;

      if(type & 1)
      {
         PrintSprite(":HUD_C:Back", 240,2, 238,2);
         x = -58;
      }

      PrintTextFmt("\C[Lith_Red]%i", wep->ammocur);
      PrintText("lhudfont", 0, x+242,1, 229,0);
   }

   if(typegfx)
      PrintSprite(typegfx, 316,2, 235,2);

   if(p->weapontype == weapon_c_smg)
   {
      fixed heat = InvNum("Lith_SMGHeat")/500.0;
      PrintSprite(":HUD_C:BarSmall", 320,2, 205,2);
      SetClip(320-63, 205-9, heat * 63, 9);
      PrintSprite(":HUD_C:HeatBar", 320,2, 205,2);
      SetClip(0, 0, 0, 0);
   }
}

static void HUD_Health(struct player *p)
{
   PrintSprite(":HUD_C:SplitBack", 0,1, 239,2);
   PrintSprite(":HUD_C:VIT",       2,1, 237,2);

   PrintTextFmt("\C[Lith_Red]%i", p->health);
   PrintText("lhudfont", 0, 21,1, 229,0);
}

// Extern Functions ----------------------------------------------------------|

stkcall
void Upgr_HeadsUpDis2_Activate(struct player *p, upgrade_t *upgr)
{
   p->hudenabled = true;
}

stkcall
void Upgr_HeadsUpDis2_Deactivate(struct player *p, upgrade_t *upgr)
{
   p->hudenabled = false;
}

stkcall
void Upgr_HeadsUpDis2_Render(struct player *p, upgrade_t *upgr)
{
   if(p->indialogue) return;

   Lith_HUD_Log(p, CR_RED, 0, -10);
   Lith_HUD_KeyInd(p, 180, 21, true, 0.8);
   Lith_HUD_Score(p, "\C[Lith_Red]%S\Cnscr", p->score, "cnfont", "a", 160,0, 3,1);

   if(p->getCVarI("lith_hud_showweapons"))
      PrintSprite(":HUD_C:Bar", 320,2, 220,2);

   Lith_HUD_WeaponSlots(p, CR_DARKRED, CR_DARKGREEN, CR_BLUE, "g", 323, 219);

   // Status
   HUD_Ammo(p);
   HUD_Health(p);
}

// EOF

