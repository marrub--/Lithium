// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// u_headsupdis3.c: HeadsUpDis3 upgrade.

#include "u_common.h"
#include "p_hud.h"

// Static Functions ----------------------------------------------------------|

static void HUD_Ammo(struct player *p)
{
   struct invweapon const *wep = p->weapon.cur;

   i32 type = 0;

   str typegfx = snil;

   if(wep->ammotype & AT_NMag) type |= 1;
   if(wep->ammotype & AT_Ammo && !(wep->info->flags & wf_magic)) type |= 2;

   if(type) PrintSprite(sp_HUD_D_AmmoBack, 320,2, 238,2);

   if(type & 1)
   {
      typegfx = sp_HUD_D_MAG;

      str txt;
      if(type & 2 && !wep->ammocur)
         txt = st_out_blue;
      else
         txt = StrParam(CrBlue "%i/%i", wep->magmax - wep->magcur, wep->magmax);
      PrintTextX_str(txt, s_lhudfont, 0, 242,1, 227,0);
   }

   if(type & 2)
   {
      typegfx = sp_HUD_D_AMM;

      i32 x = 0;

      if(type & 1)
      {
         PrintSprite(sp_HUD_D_Ammo2Back, 240,2, 238,2);
         x = -58;
      }

      PrintTextFmt(CrBlue "%i", wep->ammocur);
      PrintTextX(s_lhudfont, 0, x+242,1, 227,0);
   }

   if(typegfx) PrintSprite(typegfx, 318,2, 232,2);
}

static void HUD_Health(struct player *p)
{
   static str weapongfx[SLOT_MAX] = {
      s":HUD:H_D27",
      s":HUD:H_D28",
      s":HUD:H_D24",
      s":HUD:H_D23",
      s":HUD:H_D22",
      s":HUD:H_D21",
      s":HUD:H_D25",
      s":HUD:H_D26"
   };

   PrintSprite(sp_HUD_D_HPBack, 0,1, 239,2);

   PrintTextFmt(CrBlue "%i", p->health);
   PrintTextX(s_lhudfont, 0, 18,1, 228,0);

   str gfx = weapongfx[p->weapon.cur->info->slot];

   i32 x = (8 + p->ticks) % 40;

   for(i32 i = 0; i < 20; i++)
   {
      i32 xx = x - i;
      if(xx < 0) xx += 40;

      PrintSpriteA(gfx, 21+xx,1, 220,1, (20 - i) / 20.);
   }
}

// Extern Functions ----------------------------------------------------------|

stkcall
void Upgr_HeadsUpDis3_Activate(struct player *p, struct upgrade *upgr)
{
   p->hudenabled = true;
}

stkcall
void Upgr_HeadsUpDis3_Deactivate(struct player *p, struct upgrade *upgr)
{
   p->hudenabled = false;
}

stkcall
void Upgr_HeadsUpDis3_Render(struct player *p, struct upgrade *upgr)
{
   if(p->indialogue) return;

   HUD_Log(p, CR_LIGHTBLUE, 0, -10);
   HUD_KeyInd(p, 180, 21, true, 0.8);
   HUD_Score(p, CrBlue "%s\Cnscr", p->score, s_cnfont, s"a", 160,0, 3,1);

   if(p->getCVarI(sc_hud_showweapons))
      PrintSprite(sp_HUD_D_WepBack, 320,2, 219,2);

   HUD_WeaponSlots(p, CR_BLUE, CR_GREEN, CR_LIGHTBLUE, s"g", 323, 217);

   // Status
   HUD_Ammo(p);
   HUD_Health(p);
}

// EOF
