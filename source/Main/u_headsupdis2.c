// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Static Functions ----------------------------------------------------------|

//
// HUD_Ammo
//
static void HUD_Ammo(player_t *p, struct hud *h)
{
   invweapon_t const *wep = p->weapon.cur;

   int type = 0;

   __str typegfx = null;

   if(wep->ammotype & AT_NMag) type |= 1;
   if(wep->ammotype & AT_Ammo && !(wep->info->flags & wf_magic)) type |= 2;

   if(type)
      PrintSprite("lgfx/HUD_C/SplitFront.png", 320,2, 238,2);

   if(type & 1)
   {
      typegfx = "lgfx/HUD_C/MAG.png";

      if(type & 2 && !wep->ammocur)
         PrintTextFmt("OUT");
      else
         PrintTextFmt("%i/%i", wep->magmax - wep->magcur, wep->magmax);
      PrintText("LHUDFONT", CR_DARKRED, 242,1, 229,0);
   }

   if(type & 2)
   {
      typegfx = "lgfx/HUD_C/AMMO.png";

      int x = 0;

      if(type & 1)
      {
         PrintSprite("lgfx/HUD_C/Back.png", 240,2, 238,2);
         x = -58;
      }

      PrintTextFmt("%i", wep->ammocur);
      PrintText("LHUDFONT", CR_DARKRED, x+242,1, 229,0);
   }

   if(typegfx)
      PrintSprite(typegfx, 316,2, 235,2);

   if(p->weapontype == weapon_c_smg)
   {
      fixed heat = ACS_CheckInventory("Lith_SMGHeat")/500.0;
      PrintSprite("lgfx/HUD_C/BarSmall.png", 320,2, 205,2);
      SetClip(320-63, 205-9, heat * 63, 9);
      PrintSprite("lgfx/HUD_C/HeatBar.png", 320,2, 205,2);
      SetClip(0, 0, 0, 0);
   }
}

//
// HUD_Health
//
static void HUD_Health(player_t *p, struct hud *h)
{
   PrintSprite("lgfx/HUD_C/SplitBack.png", 0,1, 239,2);
   PrintSprite("lgfx/HUD_C/VIT.png",       2,1, 237,2);

   if(p->dead) PrintTextFmt("---");
   else        PrintTextFmt("%i", p->health);
   PrintText("LHUDFONT", CR_DARKRED, 21,1, 229,0);
}

// Extern Functions ----------------------------------------------------------|

//
// Activate
//
void Upgr_HeadsUpDis2_Activate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = true;

   p->hud.p   = p;
   p->hud.beg = hid_base_hud;
   p->hud.end = hid_end_hud;
}

//
// Disable
//
void Upgr_HeadsUpDis2_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = false;

   Lith_HUD_Clear(&p->hud);
}

//
// Render
//
void Upgr_HeadsUpDis2_Render(player_t *p, upgrade_t *upgr)
{
   struct hud *h = &p->hud;

   Lith_HUD_Begin(h);

   Lith_HUD_Log(h, CR_RED, 0, -10);
   Lith_HUD_KeyInd(h, 180, 21, true, 0.8);
   Lith_HUD_Score(h, "%S\Cnscr", p->score, "CNFONT", "a", 160.0, 3.1);

   Lith_HUD_WeaponSlots(h, CR_DARKRED, CR_DARKGREEN, CR_BLUE, "g", 323, 219);

   if(p->getCVarI("lith_hud_showweapons"))
      PrintSprite("lgfx/HUD_C/Bar.png", 320,2, 220,2);

   // Status
   HUD_Ammo(p, h);
   HUD_Health(p, h);

   Lith_HUD_End(h);
}

// EOF

