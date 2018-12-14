// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"
#include "lith_hud.h"

#define UData UData_HeadsUpDisp(upgr)

// Static Functions ----------------------------------------------------------|

static void HUD_Ammo(struct player *p)
{
   invweapon_t const *wep = p->weapon.cur;

   __str typebg;
   int y;

   if(p->getCVarI(CVAR "hud_showweapons")) {y = 14; typebg = ":HUD:SplitRight";}
   else                                    {y = 0;  typebg = ":HUD:SplitFront";}

   if(wep->ammotype & AT_NMag || wep->ammotype & AT_Ammo)
   {
      PrintSprite(":HUD:BarBig", 279,2, 238-y,2);
      PrintSprite(typebg, 320,2, 238,2);
   }

   __str typegfx = null;

   if(wep->ammotype & AT_NMag)
   {
      typegfx = ":HUD:MAG";

      if(wep->ammotype & AT_Ammo && !wep->ammocur)
         PrintTextStr("\C[Lith_Green]OUT");
      else
         PrintTextFmt("\C[Lith_Green]%i/%i", wep->magmax - wep->magcur, wep->magmax);
      PrintText("lhudfont", 0, 224,1, 229-y,0);
   }

   if(wep->ammotype & AT_Ammo)
   {
      typegfx = ":HUD:AMMO";

      int x = 0;

      if(wep->ammotype & AT_NMag)
      {
         PrintSprite(":HUD:BarBig", 220,2, 238-y,2);
         x = -59;
      }

      PrintTextFmt("\C[Lith_Green]%i", wep->ammocur);
      PrintText("lhudfont", 0, x+224,1, 229-y,0);
   }

   if(typegfx)
   {
      PrintSprite(typegfx, 282,1, 236,2);
   }
}

static void HUD_Health(struct player *p, upgrade_t *upgr)
{
   static __str weapongfx[SLOT_MAX] = {
      ":HUD:H_D27",
      ":HUD:H_D28",
      ":HUD:H_D24",
      ":HUD:H_D23",
      ":HUD:H_D22",
      ":HUD:H_D21",
      ":HUD:H_D25",
      ":HUD:H_D26"
   };

   PrintSprite(InvNum("PowerStrength") ?
      ":HUD:SplitBackRed" : ":HUD:SplitBack", 0,1, 239,2);

   PrintTextFmt("\C[Lith_Green]%i", p->health);
   PrintText("lhudfont", 0, 34,1, 231,0);

   PrintSprite(":HUD:VIT", 2,1, 237,2);

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
      PrintTextF("lhudfont", UData.cr, 34,1, 231,0, fid_health);
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

// Extern Functions ----------------------------------------------------------|

stkcall
void Upgr_HeadsUpDisp_Activate(struct player *p, upgrade_t *upgr)
{
   p->hudenabled = true;
}

stkcall
void Upgr_HeadsUpDisp_Deactivate(struct player *p, upgrade_t *upgr)
{
   p->hudenabled = false;
}

stkcall
void Upgr_HeadsUpDisp_Render(struct player *p, upgrade_t *upgr)
{
   if(p->indialogue) return;

   Lith_HUD_Log(p, CR_GREEN, 0, 0);
   Lith_HUD_KeyInd(p, 320, 20, true, 0.8);
   Lith_HUD_Score(p, "%S\Cnscr", p->score, "cnfont", "j", 320,2, 3,1);

   if(p->getCVarI(CVAR "hud_showweapons"))
      PrintSprite(":HUD:Bar", 279,2, 238,2);

   Lith_HUD_WeaponSlots(p, 0, CR_LIGHTBLUE, CR_BRICK, "k", 282, 237);

   // Status
   HUD_Ammo(p);
   HUD_Health(p, upgr);
}

// EOF
