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

   DrawSpriteFade(image, hid, 88.1 - pos, 214.1 + yadd, 0.2, 0.7);
}

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

   invweapon_t const *wep = p->weapon.cur;

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

   if(wep->ammotype & AT_NMag) {
      typegfx = "lgfx/HUD/MAG.png";
      HudMessageF("LHUDFONT", "\C[Lith_Green]%i/%i", wep->magmax - wep->magcur, wep->magmax);
      HudMessagePlain(ammo1, 224.1, 229.0-y, TS);
   }

   if(wep->ammotype & AT_Ammo)
   {
      int x = 0;

      if(wep->ammotype & AT_NMag) {
         DrawSpritePlain("lgfx/HUD/BarBig.png", ammobg2, 220.2, 238.2-y, TS);
         x = -59;
      }

      typegfx = "lgfx/HUD/AMMO.png";
      HudMessageF("LHUDFONT", "\C[Lith_Green]%i", wep->ammocur);
      HudMessagePlain(ammo2, x+224.1, 229.0-y, TS);
   }

   if(typegfx)
   {
      DrawSpritePlain(typebg, ammotypebg, 320.2, 238.2, TS);
      DrawSpritePlain("lgfx/HUD/BarBig.png", ammobg1, 279.2, 238.2-y, TS);

      DrawSpritePlain(typegfx, ammotype, 282.1, 236.2, TS);
   }
}

//
// HUD_Health
//
static void HUD_Health(player_t *p, struct hud *h)
{
   HID(hphit, 1);
   HID(hp,    1);
   HID(hptxt, 1);
   HID(hpbg,  1);
   HID(hpfxE, 32);

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
      hpbg, 0.1, 239.2, TS);

   HudMessageF("LHUDFONT", "\C[Lith_Green]%i", p->health);
   HudMessagePlain(hp, 34.1, 231.0, TS);

   DrawSpritePlain("lgfx/HUD/VIT.png", hptxt, 2.1, 237.2, TS);

   if(!p->dead)
   {
      int cr = 0;
      fixed ft;

      if(p->health < p->oldhealth)
      {
         cr = CR_YELLOW;
         ft = (p->oldhealth - p->health) / 30.0;
         ft = minmax(ft, 0.1, 3.0);
      }
      else if(p->health > p->oldhealth)
      {
         cr = CR_PURPLE;
         ft = 0.2;
      }

      if(cr)
      {
         HudMessageF("LHUDFONT", "%i", p->health);
         HudMessageParams(HUDMSG_FADEOUT, hphit, cr, 34.1, 231.0, 0.1, ft);
      }
   }

   HUD_IndicatorLine(p, weapongfx[p->weapon.cur->info->slot], hpfxE + (p->ticks % 32), 9);
}

//
// HUD_Armor
//
static void HUD_Armor(player_t *p, struct hud *h)
{
   HID(armhit, 1);
   HID(arm,    1);
   HID(armtxt, 1);
   HID(armbg,  1);
   HID(armfxE, 32);

   static __str const armorgfx[armor_max] = {
      [armor_unknown] = "lgfx/HUD/H_D27.png",
      [armor_none]    = "lgfx/HUD/H_D28.png",
      [armor_bonus]   = "lgfx/HUD/H_D23.png",
      [armor_green]   = "lgfx/HUD/H_D24.png",
      [armor_blue]    = "lgfx/HUD/H_D25.png"
   };

   DrawSpritePlain("lgfx/HUD/SplitBack.png", armbg, 0.1, 223.2, TS);

   HudMessageF("LHUDFONT", "\C[Lith_Green]%i", p->armor);
   HudMessagePlain(arm, 34.1, 215.0, TS);

   DrawSpritePlain("lgfx/HUD/ARM.png", armtxt, 2.1, 221.2, TS);

   int cr = 0;
   fixed ft;

   if(p->armor < p->oldarmor)
   {
      cr = CR_YELLOW;
      ft = minmax((p->oldarmor - p->armor) / 30.0, 0.1, 3.0);
   }
   else if(p->armor > p->oldarmor)
   {
      cr = CR_PURPLE;
      ft = 0.2;
   }

   if(cr)
   {
      HudMessageF("LHUDFONT", "%i", p->armor);
      HudMessageParams(HUDMSG_FADEOUT, armhit, cr, 34.1, 215.0, 0.1, ft);
   }

   HUD_IndicatorLine(p, armorgfx[p->armortype], armfxE + (p->ticks % 32), -7);
}

//
// HUD_Mode
//
static void HUD_Mode(player_t *p, struct hud *h)
{
   HID(mode,   1);
   HID(modebg, 1);

   if(p->weapontype == weapon_rifle)
   {
      int addy = p->getUpgrActive(UPGR_RifleModes) ? 0 : 16;
      DrawSpritePlain("lgfx/HUD/H_W3.png", modebg, 215.2, 240.2 + addy, TS);
      DrawSpritePlain(StrParam("lgfx/HUD/H_W%i.png",
         (rifle_firemode_max - p->riflefiremode) + 3),
         mode, 215.2, 208.2 + (p->riflefiremode * 16) + addy, TS);
   }
}

// Extern Functions ----------------------------------------------------------|

//
// Activate
//
void Upgr_HeadsUpDisp_Activate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = true;

   p->hud.p   = p;
   p->hud.beg = hid_base_hud;
   p->hud.end = hid_end_hud;
}

//
// Disable
//
void Upgr_HeadsUpDisp_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = false;

   Lith_HUD_Clear(&p->hud);
}

//
// Render
//
void Upgr_HeadsUpDisp_Render(player_t *p, upgrade_t *upgr)
{
   struct hud *h = &p->hud;

   Lith_HUD_Begin(h);

   Lith_HUD_Log(h, CR_GREEN, 0, 0);
   Lith_HUD_KeyInd(h, 320, 20, true, 0.8);
   Lith_HUD_Score(h, "%S\Cnscr", p->score, "CNFONT", "j", 320.2, 3.1);

   Lith_HUD_WeaponSlots(h, 0, CR_LIGHTBLUE, CR_BRICK, "k", 282, 237);

   HID(weaponbg, 1);

   if(p->getCVarI("lith_hud_showweapons"))
      DrawSpritePlain("lgfx/HUD/Bar.png", weaponbg, 279.2, 238.2, TS);

   HUD_Mode(p, h);

   // Status
   HUD_Ammo(p, h);
   HUD_Health(p, h);
   HUD_Armor(p, h);

   Lith_HUD_End(h);
}

// EOF
