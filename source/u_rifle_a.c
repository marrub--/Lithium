/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Rifle_A upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

/* Extern Functions -------------------------------------------------------- */

void Upgr_Rifle_A_Deactivate(struct upgrade *upgr)
{
   pl.riflefiremode = 0;
}

void Upgr_Rifle_A_Render(struct upgrade *upgr)
{
   if(P_Wep_CurType() != weapon_m_rifle) return;

   if(CVarGetI(sc_weapons_riflescope) &&
      pl.riflefiremode == rifle_firemode_burst)
   {
      SetClip(40, 108, 240, 40);

      ACS_SetCameraToTexture(pl.cameratid, sp_LITHCAM1, 34);
      PrintSprite(sp_LITHCAM1, 0,1, 128,0);

      PrintSprite(sp_HUD_RifleScope, 40,1, 108,1);

      ClearClip();
   }

   StrAry(ws, s":HUD:H_W4", s":HUD:H_W5", s":HUD:H_W6");

   PrintSprite(sp_HUD_H_W3, 215,2, 240,2);
   PrintSprite(ws[rifle_firemode_max - pl.riflefiremode - 1], 215,2, 208 + (pl.riflefiremode * 16),2);
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "SwitchRifleFiremode")
void Sc_SwitchRifleFiremode(void)
{
   if(!P_None())
   {
      pl.riflefiremode = ++pl.riflefiremode % rifle_firemode_max;
      ACS_LocalAmbientSound(ss_weapons_rifle_firemode, 127);
   }
}

script_str ext("ACS") addr(OBJ "ResetRifleMode")
void Sc_ResetRifleMode(void)
{
   if(!P_None())
      if(CVarGetI(sc_weapons_riflemodeclear))
         pl.riflefiremode = 0;
}

/* EOF */
