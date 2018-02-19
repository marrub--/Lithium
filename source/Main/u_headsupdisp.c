// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"
#include "lith_hud.h"

#define UData UData_HeadsUpDisp(upgr)

// Static Functions ----------------------------------------------------------|

//
// HUD_Ammo
//
static void HUD_Ammo(struct player *p)
{
   invweapon_t const *wep = p->weapon.cur;

   __str typebg;
   int y;

   if(p->getCVarI("lith_hud_showweapons"))
      {y = 14; typebg = "lgfx/HUD/SplitRight.png";}
   else
      {y = 0;  typebg = "lgfx/HUD/SplitFront.png";}

   if(wep->ammotype & AT_NMag || wep->ammotype & AT_Ammo)
   {
      PrintSprite("lgfx/HUD/BarBig.png", 279,2, 238-y,2);
      PrintSprite(typebg, 320,2, 238,2);
   }

   __str typegfx = null;

   if(wep->ammotype & AT_NMag)
   {
      typegfx = "lgfx/HUD/MAG.png";

      if(wep->ammotype & AT_Ammo && !wep->ammocur)
         PrintTextStr("\C[Lith_Green]OUT");
      else
         PrintTextFmt("\C[Lith_Green]%i/%i", wep->magmax - wep->magcur, wep->magmax);
      PrintText("LHUDFONT", 0, 224,1, 229-y,0);
   }

   if(wep->ammotype & AT_Ammo)
   {
      typegfx = "lgfx/HUD/AMMO.png";

      int x = 0;

      if(wep->ammotype & AT_NMag)
      {
         PrintSprite("lgfx/HUD/BarBig.png", 220,2, 238-y,2);
         x = -59;
      }

      PrintTextFmt("\C[Lith_Green]%i", wep->ammocur);
      PrintText("LHUDFONT", 0, x+224,1, 229-y,0);
   }

   if(typegfx)
   {
      PrintSprite(typegfx, 282,1, 236,2);
   }
}

//
// HUD_Health
//
static void HUD_Health(struct player *p, upgrade_t *upgr)
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

   PrintSprite(InvNum("PowerStrength") ?
      "lgfx/HUD/SplitBackRed.png" : "lgfx/HUD/SplitBack.png", 0,1, 239,2);

   PrintTextFmt("\C[Lith_Green]%i", p->health);
   PrintText("LHUDFONT", 0, 34,1, 231,0);

   PrintSprite("lgfx/HUD/VIT.png", 2,1, 237,2);

   fixed ft = 0;

   if(p->health < p->oldhealth)
   {
      UData.cr = CR_YELLOW;
      ft = (p->oldhealth - p->health) / 300.0;
      ft = minmax(ft, 0.1, 1.0);
   }
   else if(p->health > p->oldhealth)
   {
      UData.cr = CR_PURPLE;
      ft = 0.1;
   }

   if(ft) SetFade(fid_health, 4, ft);

   if(CheckFade(fid_health))
   {
      PrintTextFmt("%i", p->health);
      PrintTextF("LHUDFONT", UData.cr, 34,1, 231,0, fid_health);
   }

   __str gfx = weapongfx[p->weapon.cur->info->slot];

   int x = (8 + p->ticks) % 57;

   for(int i = 0; i < 20; i++)
   {
      int xx = x - i;
      if(xx < 0) xx += 57;

      int y = 9;
      if(xx < 11) y += 11 - xx % 12;

      PrintSpriteA(gfx, 88-xx,1, 214+y,1, (20 - i) / 20.);
   }
}

//
// HUD_Mode
//
static void HUD_Mode(struct player *p)
{
   if(p->weapontype == weapon_rifle)
   {
      int addy = p->getUpgrActive(UPGR_RifleModes) ? 0 : 16;
      PrintSprite("lgfx/HUD/H_W3.png", 215,2, 240 + addy,2);
      PrintSprite(StrParam("lgfx/HUD/H_W%i.png", (rifle_firemode_max - p->riflefiremode) + 3),
         215,2, 208 + (p->riflefiremode * 16) + addy,2);
   }
}

// Extern Functions ----------------------------------------------------------|

//
// Activate
//
void Upgr_HeadsUpDisp_Activate(struct player *p, upgrade_t *upgr)
{
   p->hudenabled = true;
}

//
// Disable
//
void Upgr_HeadsUpDisp_Deactivate(struct player *p, upgrade_t *upgr)
{
   p->hudenabled = false;
}

//
// Render
//
void Upgr_HeadsUpDisp_Render(struct player *p, upgrade_t *upgr)
{
   Lith_HUD_Log(p, CR_GREEN, 0, 0);
   Lith_HUD_KeyInd(p, 320, 20, true, 0.8);
   Lith_HUD_Score(p, "%S\Cnscr", p->score, "CNFONT", "j", 320,2, 3,1);

   if(p->getCVarI("lith_hud_showweapons"))
      PrintSprite("lgfx/HUD/Bar.png", 279,2, 238,2);

   Lith_HUD_WeaponSlots(p, 0, CR_LIGHTBLUE, CR_BRICK, "k", 282, 237);

   HUD_Mode(p);

   // Status
   HUD_Ammo(p);
   HUD_Health(p, upgr);
}

// EOF
