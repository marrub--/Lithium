// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Static Functions ----------------------------------------------------------|

//
// HUD_IndicatorLine
//
static void HUD_IndicatorLine(player_t *p, __str image, int hid, int yadd)
{
   int pos = (8 + p->ticks) % 57;

   if(pos < 11)
      yadd += 11 - (pos % 12);

   DrawSpriteFade(image, hid, 88.1 - pos, 175.1 + yadd, 0.2, 0.7);
}

//
// HUD_Weapons
//
static void HUD_Weapons(player_t *p)
{
   DrawSpritePlain("lgfx/HUD/Bar.png", hid_weaponbg, 279.2, 199.2, TICSECOND);

   static int const ncolor[] = {
      CR_UNTRANSLATED,
      CR_LIGHTBLUE,
      CR_BRICK
   };

   Lith_HUD_DrawWeaponSlots(p, ncolor, countof(ncolor), 'k', 282, 198);
}

//
// HUD_Ammo
//
static void HUD_Ammo(player_t *p)
{
   invweapon_t  const *wep  = p->weapon.cur;
   weaponinfo_t const *info = wep->info;

   __str typegfx = null;
   __str typebg;

   int y;

   if(p->getCVarI("lith_hud_showweapons")) {
      typebg = "lgfx/HUD/SplitRight.png";
      y = 14;
   } else {
      typebg = "lgfx/HUD/SplitFront.png";
      y = 0;
   }

   if(wep->ammotype & AT_NMag)
   {
      int max = ACS_GetMaxInventory(0, wep->magclass);
      int cur = ACS_CheckInventory(wep->magclass);

      typegfx = "lgfx/HUD/MAG.png";
      HudMessageF("LHUDFONT", "%i/%i", max - cur, max);
      HudMessagePlain(hid_ammo1, 224.1, 190.0-y, TICSECOND);
   }

   if(wep->ammotype & AT_Ammo)
   {
      int x = 0;

      if(wep->ammotype & AT_NMag) {
         DrawSpritePlain("lgfx/HUD/BarBig.png", hid_ammobg2, 220.2, 199.2-y, TICSECOND);
         x = -59;
      }

      typegfx = "lgfx/HUD/AMMO.png";
      HudMessageF("LHUDFONT", "%i", ACS_CheckInventory(wep->ammoclass));
      HudMessagePlain(hid_ammo2, x+224.1, 190.0-y, TICSECOND);
   }

   if(!typegfx) return;

   DrawSpritePlain(typebg, hid_ammotypebg, 320.2, 199.2, TICSECOND);
   DrawSpritePlain("lgfx/HUD/BarBig.png", hid_ammobg1, 279.2, 199.2-y, TICSECOND);

   DrawSpritePlain(typegfx, hid_ammotype, 282.1, 197.2, TICSECOND);
}

//
// HUD_Health
//
static void HUD_Health(player_t *p)
{
   static __str weapongfx[SLOT_MAX] = {
      [0] = "lgfx/HUD/H_D27.png",
      [1] = "lgfx/HUD/H_D28.png",
      [2] = "lgfx/HUD/H_D24.png",
      [3] = "lgfx/HUD/H_D23.png",
      [4] = "lgfx/HUD/H_D22.png",
      [5] = "lgfx/HUD/H_D21.png",
      [6] = "lgfx/HUD/H_D25.png",
      [7] = "lgfx/HUD/H_D26.png"
   };

   DrawSpritePlain(
      ACS_CheckInventory("PowerStrength") ?
         "lgfx/HUD/SplitBackRed.png" :
         "lgfx/HUD/SplitBack.png",
      hid_healthbg, 0.1, 200.2, TICSECOND);

   if(p->dead) HudMessageF("LHUDFONT", "---");
   else        HudMessageF("LHUDFONT", "%i", p->health);

   HudMessagePlain(hid_health, 34.1, 192.0, TICSECOND);

   DrawSpritePlain("lgfx/HUD/VIT.png", hid_healthtxt, 2.1, 198.2, TICSECOND);

   if(!p->dead)
   {
      int cr = 0;
      fixed ft;

      if(p->health < p->old.health)
      {
         cr = CR_YELLOW;
         ft = (p->old.health - p->health) / 30.0;
         ft = minmax(ft, 0.1, 3.0);
      }
      else if(p->health > p->old.health)
      {
         cr = CR_PURPLE;
         ft = 0.2;
      }

      if(cr)
      {
         HudMessageF("LHUDFONT", "%i", p->health);
         HudMessageParams(HUDMSG_FADEOUT, hid_healthhit, cr, 34.1, 192.0, 0.1, ft);
      }
   }

   HUD_IndicatorLine(p, weapongfx[p->weapon.cur->info->slot], hid_healthbg_fxS - (p->ticks % 32), 9);
}

//
// HUD_Armor
//
static void HUD_Armor(player_t *p)
{
   static __str const armorgfx[armor_max] = {
      [armor_unknown] = "lgfx/HUD/H_D27.png",
      [armor_none]    = "lgfx/HUD/H_D28.png",
      [armor_bonus]   = "lgfx/HUD/H_D23.png",
      [armor_green]   = "lgfx/HUD/H_D24.png",
      [armor_blue]    = "lgfx/HUD/H_D25.png"
   };

   DrawSpritePlain("lgfx/HUD/SplitBack.png", hid_armorbg, 0.1, 184.2, TICSECOND);

   HudMessageF("LHUDFONT", "%i", p->armor);
   HudMessagePlain(hid_armor, 34.1, 176.0, TICSECOND);

   DrawSpritePlain("lgfx/HUD/ARM.png", hid_armortxt, 2.1, 182.2, TICSECOND);

   int cr = 0;
   fixed ft;

   if(p->armor < p->old.armor)
   {
      cr = CR_YELLOW;
      ft = minmax((p->old.armor - p->armor) / 30.0, 0.1, 3.0);
   }
   else if(p->armor > p->old.armor)
   {
      cr = CR_PURPLE;
      ft = 0.2;
   }

   if(cr)
   {
      HudMessageF("LHUDFONT", "%i", p->armor);
      HudMessageParams(HUDMSG_FADEOUT, hid_armorhit, cr, 34.1, 176.0, 0.1, ft);
   }

   HUD_IndicatorLine(p, armorgfx[p->armortype], hid_armorbg_fxS - (p->ticks % 32), -7);
}

//
// HUD_Score
//
static void HUD_Score(player_t *p)
{
   HudMessageF("CNFONT", "%S\Cnscr", Lith_ScoreSep(p->score));
   HudMessageParams(HUDMSG_PLAIN, hid_score, CR_WHITE, 320.2, 3.1, 0.1);

   if(p->score > p->old.score)
   {
      HudMessageF("CNFONT", "%S\Cnscr", Lith_ScoreSep(p->score));
      HudMessageParams(HUDMSG_FADEOUT, hid_scorehit, CR_ORANGE, 320.2, 3.1, 0.1, 0.2);
   }
   else if(p->score < p->old.score)
   {
      fixed ft = minmax((p->old.score - p->score) / 3000.0, 0.1, 3.0);
      HudMessageF("CNFONT", "%S\Cnscr", Lith_ScoreSep(p->score));
      HudMessageParams(HUDMSG_FADEOUT, hid_scorehit, CR_PURPLE, 320.2, 3.1, 0.1, ft);
   }

   if(p->scoreaccumtime > 0)
   {
      HudMessageF("CNFONT", "%+S", Lith_ScoreSep(p->scoreaccum));
      HudMessageParams(HUDMSG_FADEOUT, hid_scoreaccum, CR_WHITE, 320.2, 13.1, 0.1, 0.4);
   }
}

//
// HUD_KeyInd
//
static void HUD_KeyInd(player_t *p)
{
   if(p->keys.redskull)    DrawSpriteAlpha("H_KS1", hid_key_redskull,    320.2, 10.1, 0.1, 0.8);
   if(p->keys.yellowskull) DrawSpriteAlpha("H_KS2", hid_key_yellowskull, 310.2, 10.1, 0.1, 0.8);
   if(p->keys.blueskull)   DrawSpriteAlpha("H_KS3", hid_key_blueskull,   300.2, 10.1, 0.1, 0.8);
   if(p->keys.redcard)     DrawSpriteAlpha("H_KC1", hid_key_red,         290.2, 10.1, 0.1, 0.8);
   if(p->keys.yellowcard)  DrawSpriteAlpha("H_KC2", hid_key_yellow,      280.2, 10.1, 0.1, 0.8);
   if(p->keys.bluecard)    DrawSpriteAlpha("H_KC3", hid_key_blue,        270.2, 10.1, 0.1, 0.8);
}

//
// HUD_Mode
//
static void HUD_Mode(player_t *p)
{
   if(p->weapontype == weapon_rifle)
   {
      int addy = p->getUpgr(UPGR_RifleModes)->active ? 0 : 16;
      DrawSpritePlain("lgfx/HUD/H_W3.png", hid_riflemodebg, 215.2, 200.2 + addy, TICSECOND);
      DrawSpritePlain(StrParam("lgfx/HUD/H_W%i.png",
         (rifle_firemode_max - p->riflefiremode) + 3),
         hid_riflemode, 215.2, 168.2 + (p->riflefiremode * 16) + addy, TICSECOND);
   }
}

// Extern Functions ----------------------------------------------------------|

//
// Activate
//
void Upgr_HeadsUpDisp_Activate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = true;
}

//
// Disable
//
void Upgr_HeadsUpDisp_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = false;
}

//
// Render
//
void Upgr_HeadsUpDisp_Render(player_t *p, upgrade_t *upgr)
{
   // Log
   Lith_HUD_Log(p);

   // Inventory
   HUD_KeyInd(p);

   if(p->getCVarI("lith_hud_showscore"))
      HUD_Score(p);
   if(p->getCVarI("lith_hud_showweapons"))
      HUD_Weapons(p);

   HUD_Mode(p);

   // Status
   HUD_Ammo(p);
   HUD_Health(p);
   HUD_Armor(p);
}

// EOF

