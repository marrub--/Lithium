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
      PrintSprite(":HUD_D:AmmoBack", 320,2, 238,2);

   if(type & 1)
   {
      typegfx = ":HUD_D:MAG";

      if(type & 2 && !wep->ammocur)
         PrintTextStr("\C[Lith_Blue]OUT");
      else
         PrintTextFmt("\C[Lith_Blue]%i/%i", wep->magmax - wep->magcur, wep->magmax);
      PrintText("lhudfont", 0, 242,1, 227,0);
   }

   if(type & 2)
   {
      typegfx = ":HUD_D:AMM";

      int x = 0;

      if(type & 1)
      {
         PrintSprite(":HUD_D:Ammo2Back", 240,2, 238,2);
         x = -58;
      }

      PrintTextFmt("\C[Lith_Blue]%i", wep->ammocur);
      PrintText("lhudfont", 0, x+242,1, 227,0);
   }

   if(typegfx) PrintSprite(typegfx, 318,2, 232,2);
}

static void HUD_Health(struct player *p)
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

   PrintSprite(":HUD_D:HPBack", 0,1, 239,2);

   PrintTextFmt("\C[Lith_Blue]%i", p->health);
   PrintText("lhudfont", 0, 18,1, 228,0);

   __str gfx = weapongfx[p->weapon.cur->info->slot];

   int x = (8 + p->ticks) % 40;

   for(int i = 0; i < 20; i++)
   {
      int xx = x - i;
      if(xx < 0) xx += 40;

      PrintSpriteA(gfx, 21+xx,1, 220,1, (20 - i) / 20.);
   }
}

// Extern Functions ----------------------------------------------------------|

stkcall
void Upgr_HeadsUpDis3_Activate(struct player *p, upgrade_t *upgr)
{
   p->hudenabled = true;
}

stkcall
void Upgr_HeadsUpDis3_Deactivate(struct player *p, upgrade_t *upgr)
{
   p->hudenabled = false;
}

stkcall
void Upgr_HeadsUpDis3_Render(struct player *p, upgrade_t *upgr)
{
   if(p->indialogue) return;

   Lith_HUD_Log(p, CR_LIGHTBLUE, 0, -10);
   Lith_HUD_KeyInd(p, 180, 21, true, 0.8);
   Lith_HUD_Score(p, "\C[Lith_Blue]%S\Cnscr", p->score, "cnfont", "a", 160,0, 3,1);

   if(p->getCVarI(CVAR "hud_showweapons"))
      PrintSprite(":HUD_D:WepBack", 320,2, 219,2);

   Lith_HUD_WeaponSlots(p, CR_BLUE, CR_GREEN, CR_LIGHTBLUE, "g", 323, 217);

   // Status
   HUD_Ammo(p);
   HUD_Health(p);
}

// EOF
