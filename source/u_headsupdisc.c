/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * HeadsUpDisC upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"
#include "p_hud.h"

/* Static Functions -------------------------------------------------------- */

static void HUD_Ammo(struct player *p)
{
   struct invweapon const *wep = p->weapon.cur;

   i32 type = 0;

   str typegfx = snil;

   if(wep->ammotype & AT_NMag) type |= 1;
   if(wep->ammotype & AT_Ammo && !get_bit(wep->info->flags, wf_magic)) type |= 2;

   if(type) PrintSprite(sp_HUD_C_SplitFront, 320,2, 238,2);

   if(type & 1) {
      typegfx = sp_HUD_C_MAG;

      char txt[16];
      if(type & 2 && !wep->ammocur)
         strcpy(txt, "OUT");
      else
         sprintf(txt, "%i/%i", wep->magmax - wep->magcur, wep->magmax);
      PrintTextChS(txt);
      PrintTextX(s_lhudfont, cr_red, 242,1, 229,0, ptf_no_utf);
   }

   if(type & 2) {
      typegfx = sp_HUD_C_AMMO;

      i32 x = 0;

      if(type & 1) {
         PrintSprite(sp_HUD_C_Back, 240,2, 238,2);
         x = -58;
      }

      ACS_BeginPrint();
      ACS_PrintInt(wep->ammocur);
      PrintTextX(s_lhudfont, cr_red, x+242,1, 229,0, ptf_no_utf);
   }

   if(typegfx)
      PrintSprite(typegfx, 316,2, 235,2);

   if(P_Wep_CurType(p) == weapon_c_smg) {
      k32 heat = InvNum(so_SMGHeat)/500.0;
      PrintSprite(sp_HUD_C_BarSmall, 320,2, 205,2);
      SetClip(320-63, 205-9, heat * 63, 9);
      PrintSprite(sp_HUD_C_HeatBar, 320,2, 205,2);
      ClearClip();
   }
}

static void HUD_Health(struct player *p)
{
   PrintSprite(sp_HUD_C_SplitBack, 0,1, 239,2);
   PrintSprite(sp_HUD_C_VIT,       2,1, 237,2);

   ACS_BeginPrint();
   ACS_PrintInt(p->health);
   PrintTextX(s_lhudfont, cr_red, 21,1, 229,0, ptf_no_utf);
   HUD_DrawHealth(p, p->health, 21, 229, cr_red, 0);
}

/* Extern Functions -------------------------------------------------------- */

stkcall
void Upgr_HeadsUpDisC_Render(struct player *p, struct upgrade *upgr)
{
   if(!p->hudenabled) return;

   HUD_Log(p, cr_red, 0, -10);
   HUD_KeyInd(p, 180, 21, true, 0.8);
   HUD_Score(p, "%s\Cnscr", p->score, s_smallfnt, cr_red, 160,0);

   if(p->getCVarI(sc_hud_showweapons))
      PrintSprite(sp_HUD_C_Bar, 320,2, 220,2);

   HUD_WeaponSlots(p, CR_DARKRED, CR_DARKGREEN, CR_BLUE, cr_red, 323, 219);

   /* Status */
   HUD_Ammo(p);
   HUD_Health(p);
}

/* EOF */
