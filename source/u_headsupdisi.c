/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * HeadsUpDisI upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"
#include "p_hud.h"

#define UData p->upgrdata.headsupdisi

/* Static Functions -------------------------------------------------------- */

static void HUDI_Ammo(struct player *p, struct upgrade *upgr)
{
   struct invweapon const *wep = p->weapon.cur;

   str typegfx = snil;

   bool has_ammo = (wep->ammotype & AT_Ammo && !get_bit(wep->info->flags, wf_magic));
   bool has_nmag = wep->ammotype & AT_NMag;

   if(has_ammo && has_nmag) {
      PrintSprite(sp_HUD_I_AmmoExtend, 242,2, 227,2);
   }

   PrintSprite(sp_HUD_I_AmmoWepsBack, 320,2, 229,2);

   if(has_ammo) {
      typegfx = sp_HUD_I_AMMO;

      i32 x = 0;

      if(has_nmag) {
         x = -58;
      }

      PrintTextFmt(CrPurple "%i", wep->ammocur);
      PrintTextX(s_lhudfont, CR_PURPLE, x+242,1, 218,0, ptf_no_utf);
   }

   if(has_nmag) {
      typegfx = sp_HUD_I_MAG;

      str txt;
      if(wep->ammotype & AT_Ammo && !wep->ammocur)
         txt = st_out_purple;
      else
         txt = StrParam(CrPurple "%i/%i", wep->magmax - wep->magcur, wep->magmax);
      PrintTextX_str(txt, s_lhudfont, 0, 242,1, 218,0, ptf_no_utf);
   }

   if(typegfx) {
      PrintSprite(typegfx, 309,0, 219,0);
   }

   HUD_WeaponSlots(p, 0, CR_DARKGRAY, CR_GRAY, s"g", 323, 208);
}

static void HUDI_HealthArmor(struct player *p, struct upgrade *upgr)
{
   PrintSprite(sp_HUD_I_HPAPBack, 0,1, 230,2);

   UData.health.value = p->health;
   lerplli(&UData.health);

   HUD_DrawHealth(p, UData.health.value_display, 21, 202, CrPurple);

   UData.overdrive.value = p->overdrive;
   lerplli(&UData.overdrive);

   PrintTextFmt(CrPurple "%lli", UData.overdrive.value_display);
   PrintTextX(s_lhudfont, CR_PURPLE, 21,1, 220,0, ptf_no_utf);
}

/* Extern Functions -------------------------------------------------------- */

stkcall
void Upgr_HeadsUpDisI_Activate(struct player *p, struct upgrade *upgr)
{
   lerplli_init(&UData.score,     p->score,     4);
   lerplli_init(&UData.health,    p->health,    1);
   lerplli_init(&UData.overdrive, p->overdrive, 1);
}

stkcall
void Upgr_HeadsUpDisI_Render(struct player *p, struct upgrade *upgr)
{
   if(!p->hudenabled) return;

   HUD_Log(p, CR_LIGHTBLUE, 0, -15);

   HUD_KeyInd(p, 20, 20, false, 0.8);

   UData.score.value = p->score;
   lerplli(&UData.score);

   HUD_Score(p, "%s \CnScore", UData.score.value_display, s_lmidfont, s"[Lith_Purple]", 2,1);

   /* Status */
   HUDI_Ammo(p, upgr);
   HUDI_HealthArmor(p, upgr);
}

/* EOF */
