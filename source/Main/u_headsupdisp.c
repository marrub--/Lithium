/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * HeadsUpDisp upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"
#include "p_hud.h"

#define UData UData_HeadsUpDisp(upgr)

/* Static Functions -------------------------------------------------------- */

#if LITHIUM
static void HUD_Ammo(struct player *p)
{
   struct invweapon const *wep = p->weapon.cur;

   str typebg;
   i32 y;

   if(p->getCVarI(sc_hud_showweapons)) {y = 14; typebg = sp_HUD_SplitRight;}
   else                                {y = 0;  typebg = sp_HUD_SplitFront;}

   if(wep->ammotype & AT_NMag || wep->ammotype & AT_Ammo)
   {
      PrintSprite(sp_HUD_BarBig, 279,2, 238-y,2);
      PrintSprite(typebg, 320,2, 238,2);
   }

   str typegfx = snil;

   if(wep->ammotype & AT_NMag)
   {
      typegfx = sp_HUD_MAG;

      str txt;
      if(wep->ammotype & AT_Ammo && !wep->ammocur)
         txt = st_out_green;
      else
         txt = StrParam(CrGreen "%i/%i", wep->magmax - wep->magcur, wep->magmax);
      PrintTextX_str(txt, s_lhudfont, 0, 224,1, 229-y,0);
   }

   if(wep->ammotype & AT_Ammo)
   {
      typegfx = sp_HUD_AMMO;

      i32 x = 0;

      if(wep->ammotype & AT_NMag)
      {
         PrintSprite(sp_HUD_BarBig, 220,2, 238-y,2);
         x = -59;
      }

      PrintTextFmt(CrGreen "%i", wep->ammocur);
      PrintTextX(s_lhudfont, 0, x+224,1, 229-y,0);
   }

   if(typegfx)
      PrintSprite(typegfx, 282,1, 236,2);

   if(P_Wep_CurType(p) == weapon_rifle && ServCallI(sm_GetRifleGrenade))
      PrintSprite(sp_HUD_H_D44, 281,1, 236,1);
}

static void HUD_Health(struct player *p, struct upgrade *upgr)
{
   static str ws[SLOT_MAX] = {s":HUD:H_D27", s":HUD:H_D28", s":HUD:H_D24",
                              s":HUD:H_D23", s":HUD:H_D22", s":HUD:H_D21",
                              s":HUD:H_D25", s":HUD:H_D26"};

   PrintSprite(InvNum(so_PowerStrength) ? sp_HUD_SplitBackRed : sp_HUD_SplitBack, 0,1, 239,2);

   PrintTextFmt(CrGreen "%i", p->health);
   PrintTextX(s_lhudfont, 0, 34,1, 231,0);

   PrintSprite(sp_HUD_VIT, 2,1, 237,2);

   k32 ft = 0;

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
      PrintTextFX(s_lhudfont, UData.cr, 34,1, 231,0, fid_health);
   }

   str gfx = ws[p->weapon.cur->info->slot];

   i32 x = (8 + p->ticks) % 57;

   for(i32 i = 0; i < 20; i++)
   {
      i32 xx = x - i;
      if(xx < 0) xx += 57;

      i32 y = 9;
      if(xx < 11) y += 11 - xx % 12;

      PrintSpriteA(gfx, 88-xx,1, 214+y,1, (20 - i) / 20.);
   }
}
#endif

/* Extern Functions -------------------------------------------------------- */

stkcall
void Upgr_HeadsUpDisp_Activate(struct player *p, struct upgrade *upgr)
{
   p->hudenabled = true;
}

stkcall
void Upgr_HeadsUpDisp_Deactivate(struct player *p, struct upgrade *upgr)
{
   p->hudenabled = false;
}

stkcall
void Upgr_HeadsUpDisp_Render(struct player *p, struct upgrade *upgr)
{
   if(p->dlg.active) return;

   HUD_Log(p, CR_GREEN, 0, 0);

   #if LITHIUM
   HUD_KeyInd(p, 320, 20, true, 0.8);
   HUD_Score(p, "%s\Cnscr", p->score, s_cnfont, s"j", 320,2, 3,1);

   if(p->getCVarI(sc_hud_showweapons))
      PrintSprite(sp_HUD_Bar, 279,2, 238,2);

   HUD_WeaponSlots(p, 0, CR_LIGHTBLUE, CR_BRICK, s"k", 282, 237);

   /* Status */
   HUD_Ammo(p);
   HUD_Health(p, upgr);
   #endif
}

/* EOF */
