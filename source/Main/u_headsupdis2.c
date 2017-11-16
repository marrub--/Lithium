// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Static Functions ----------------------------------------------------------|

//
// HUD_Weapons
//
static void HUD_Weapons(player_t *p)
{
   DrawSpritePlain("lgfx/HUD_C/Bar.png", hid_weaponbg, 320.2, 220.2, TICSECOND);

   static int const ncolor[] = {
      CR_DARKRED,
      CR_DARKGREEN,
      CR_BLUE
   };

   Lith_HUD_DrawWeaponSlots(p, ncolor, countof(ncolor), 'g', 323, 219);
}

//
// HUD_Ammo
//
static void HUD_Ammo(player_t *p)
{
   invweapon_t  const *wep  = p->weapon.cur;
   weaponinfo_t const *info = wep->info;

   __str typegfx = null;

   if(wep->ammotype & AT_NMag)
   {
      int max = ACS_GetMaxInventory(0, wep->magclass);
      int cur = ACS_CheckInventory(wep->magclass);

      typegfx = "lgfx/HUD_C/MAG.png";
      HudMessageF("LHUDFONT", "%i/%i", max - cur, max);
      HudMessageParams(0, hid_ammo1, CR_DARKRED, 242.1, 229.0, TICSECOND);
   }

   if(wep->ammotype & AT_Ammo && !(wep->info->flags & wf_magic))
   {
      int x = 0;

      if(wep->ammotype & AT_NMag) {
         DrawSpritePlain("lgfx/HUD_C/Back.png", hid_ammobg2, 240.2, 238.2, TICSECOND);
         x = -58;
      }

      typegfx = "lgfx/HUD_C/AMMO.png";
      HudMessageF("LHUDFONT", "%i", ACS_CheckInventory(wep->ammoclass));
      HudMessageParams(0, hid_ammo2, CR_DARKRED, x+242.1, 229.0, TICSECOND);
   }

   if(typegfx) {
      DrawSpritePlain("lgfx/HUD_C/SplitFront.png", hid_ammobg1, 320.2, 238.2, TICSECOND);
      DrawSpritePlain(typegfx, hid_ammotype, 316.2, 235.2, TICSECOND);
   }

   if(p->weapontype == weapon_c_smg)
   {
      DrawSpritePlain("lgfx/HUD_C/BarSmall.png", hid_rifleheatbg, 320.2, 205.2, TICSECOND);
      ACS_SetHudClipRect(320-63, 205-9, (ACS_CheckInventory("Lith_SMGHeat")/500.k) * 63, 9);
      DrawSpritePlain("lgfx/HUD_C/HeatBar.png", hid_rifleheat, 320.2, 205.2, TICSECOND);
      ACS_SetHudClipRect(0, 0, 0, 0);
   }
}

//
// HUD_Health
//
static void HUD_Health(player_t *p)
{
   DrawSpritePlain("lgfx/HUD_C/SplitBack.png", hid_healthbg, 0.1, 239.2, TICSECOND);

   if(p->dead) HudMessageF("LHUDFONT", "---");
   else        HudMessageF("LHUDFONT", "%i", p->health);
   HudMessageParams(0, hid_health, CR_DARKRED, 21.1, 229.0, TICSECOND);

   DrawSpritePlain("lgfx/HUD_C/VIT.png", hid_healthtxt, 2.1, 237.2, TICSECOND);
}

//
// HUD_Armor
//
static void HUD_Armor(player_t *p)
{
   static __str const armorgfx[armor_max] = {
      [armor_unknown] = "lgfx/HUD_C/ARM.png",
      [armor_none]    = "lgfx/HUD_C/ARM.png",
      [armor_bonus]   = "lgfx/HUD_C/ARM_Y.png",
      [armor_green]   = "lgfx/HUD_C/ARM_G.png",
      [armor_blue]    = "lgfx/HUD_C/ARM_B.png",
   };

   DrawSpritePlain("lgfx/HUD_C/SplitBack.png", hid_armorbg, 0.1, 220.2, TICSECOND);

   HudMessageF("LHUDFONT", "%i", p->armor);
   HudMessageParams(0, hid_armor, CR_DARKRED, 21.1, 210.0, TICSECOND);

   DrawSpritePlain(armorgfx[p->armortype], hid_armortxt, 3.1, 216.2, TICSECOND);
}

//
// HUD_Score
//
static void HUD_Score(player_t *p)
{
   HudMessageF("CNFONT", "\Ca%S\Cnscr", Lith_ScoreSep(p->score));
   HudMessageParams(HUDMSG_PLAIN, hid_score, CR_WHITE, 160.0, 3.1, 0.1);
}

//
// HUD_KeyInd
//
static void HUD_KeyInd(player_t *p)
{
   if(p->keys.redskull)    DrawSpriteAlpha("H_KS1", hid_key_redskull,    180.2, 11.1, 0.1, 0.8);
   if(p->keys.yellowskull) DrawSpriteAlpha("H_KS2", hid_key_yellowskull, 170.2, 11.1, 0.1, 0.8);
   if(p->keys.blueskull)   DrawSpriteAlpha("H_KS3", hid_key_blueskull,   160.2, 10.1, 0.1, 0.8);
   if(p->keys.redcard)     DrawSpriteAlpha("H_KC1", hid_key_red,         150.2, 10.1, 0.1, 0.8);
   if(p->keys.yellowcard)  DrawSpriteAlpha("H_KC2", hid_key_yellow,      140.2, 11.1, 0.1, 0.8);
   if(p->keys.bluecard)    DrawSpriteAlpha("H_KC3", hid_key_blue,        130.2, 12.1, 0.1, 0.8);
}

// Extern Functions ----------------------------------------------------------|

//
// Activate
//
void Upgr_HeadsUpDis2_Activate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = true;
}

//
// Disable
//
void Upgr_HeadsUpDis2_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = false;
}

//
// Render
//
void Upgr_HeadsUpDis2_Render(player_t *p, upgrade_t *upgr)
{
   // Log
   Lith_HUD_Log(p);

   // Inventory
   HUD_KeyInd(p);

   if(p->getCVarI("lith_hud_showscore"))
      HUD_Score(p);
   if(p->getCVarI("lith_hud_showweapons"))
      HUD_Weapons(p);

   // Status
   HUD_Ammo(p);
   HUD_Health(p);
   HUD_Armor(p);
}

// EOF

