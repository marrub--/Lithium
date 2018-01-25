// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Static Functions ----------------------------------------------------------|

//
// HUD_Ammo
//
static void HUD_Ammo(player_t *p, struct hud *h)
{
   HID(ammo1,      1);
   HID(ammo2,      1);
   HID(ammotype,   1);
   HID(ammotypebg, 1);
   HID(ammobg1,    1);
   HID(ammobg2,    1);
   HID(heat,       1);
   HID(heatbg,     1);

   invweapon_t const *wep = p->weapon.cur;

   __str typegfx = null;

   if(wep->ammotype & AT_NMag) {
      typegfx = "lgfx/HUD_C/MAG.png";
      HudMessageF("LHUDFONT", "%i/%i", wep->magmax - wep->magcur, wep->magmax);
      HudMessageParams(0, ammo1, CR_DARKRED, 242.1, 229.0, TS);
   }

   if(wep->ammotype & AT_Ammo && !(wep->info->flags & wf_magic))
   {
      int x = 0;

      if(wep->ammotype & AT_NMag) {
         DrawSpritePlain("lgfx/HUD_C/Back.png", ammobg2, 240.2, 238.2, TS);
         x = -58;
      }

      typegfx = "lgfx/HUD_C/AMMO.png";
      HudMessageF("LHUDFONT", "%i", wep->ammocur);
      HudMessageParams(0, ammo2, CR_DARKRED, x+242.1, 229.0, TS);
   }

   if(typegfx) {
      DrawSpritePlain("lgfx/HUD_C/SplitFront.png", ammobg1, 320.2, 238.2, TS);
      DrawSpritePlain(typegfx, ammotype, 316.2, 235.2, TS);
   }

   if(p->weapontype == weapon_c_smg)
   {
      DrawSpritePlain("lgfx/HUD_C/BarSmall.png", heatbg, 320.2, 205.2, TS);
      ACS_SetHudClipRect(320-63, 205-9, (ACS_CheckInventory("Lith_SMGHeat")/500.k) * 63, 9);
      DrawSpritePlain("lgfx/HUD_C/HeatBar.png", heat, 320.2, 205.2, TS);
      ACS_SetHudClipRect(0, 0, 0, 0);
   }
}

//
// HUD_Health
//
static void HUD_Health(player_t *p, struct hud *h)
{
   HID(hp,    1);
   HID(hptxt, 1);
   HID(hpbg,  1);

   DrawSpritePlain("lgfx/HUD_C/SplitBack.png", hpbg, 0.1, 239.2, TS);

   if(p->dead) HudMessageF("LHUDFONT", "---");
   else        HudMessageF("LHUDFONT", "%i", p->health);
   HudMessageParams(0, hp, CR_DARKRED, 21.1, 229.0, TS);

   DrawSpritePlain("lgfx/HUD_C/VIT.png", hptxt, 2.1, 237.2, TS);
}

//
// HUD_Armor
//
static void HUD_Armor(player_t *p, struct hud *h)
{
   HID(arm,    1);
   HID(armtxt, 1);
   HID(armbg,  1);

   static __str const armorgfx[armor_max] = {
      [armor_unknown] = "lgfx/HUD_C/ARM.png",
      [armor_none]    = "lgfx/HUD_C/ARM.png",
      [armor_bonus]   = "lgfx/HUD_C/ARM_Y.png",
      [armor_green]   = "lgfx/HUD_C/ARM_G.png",
      [armor_blue]    = "lgfx/HUD_C/ARM_B.png",
   };

   DrawSpritePlain("lgfx/HUD_C/SplitBack.png", armbg, 0.1, 220.2, TS);

   HudMessageF("LHUDFONT", "%i", p->armor);
   HudMessageParams(0, arm, CR_DARKRED, 21.1, 210.0, TS);

   DrawSpritePlain(armorgfx[p->armortype], armtxt, 3.1, 216.2, TS);
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

   HID(weaponbg, 1);

   if(p->getCVarI("lith_hud_showweapons"))
      DrawSpritePlain("lgfx/HUD_C/Bar.png", weaponbg, 320.2, 220.2, TS);

   // Status
   HUD_Ammo(p, h);
   HUD_Health(p, h);
   HUD_Armor(p, h);

   Lith_HUD_End(h);
}

// EOF

