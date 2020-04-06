/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * JetBooster upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

#define UData p->upgrdata.jetbooster

#define CHARGE_MAX (35 * 7)

/* Extern Functions -------------------------------------------------------- */

stkcall
void Upgr_JetBooster_Activate(struct player *p, struct upgrade *upgr)
{
   UData.charge = CHARGE_MAX;
}

script
void Upgr_JetBooster_Update(struct player *p, struct upgrade *upgr)
{
   UData.discharged = UData.charge > 60 && UData.charge < CHARGE_MAX;

   if(UData.charge < CHARGE_MAX)
      UData.charge++;

   if(p->frozen) return;

   if(P_ButtonPressed(p, BT_SPEED) && !p->onground && UData.charge >= CHARGE_MAX)
   {
      k32 angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);

      StartSound(ss_player_rocketboost, lch_auto, 0);
      InvGive(so_RocketBooster, 1);
      P_SetVel(p, p->velx + (ACS_Cos(angle) * 16.0), p->vely + (ACS_Sin(angle) * 16.0), 10.0);

      UData.charge = 0;
      p->upgrdata.reflexwetw.leaped = 0;
   }
}

stkcall
void Upgr_JetBooster_Render(struct player *p, struct upgrade *upgr)
{
   if(!p->hudenabled) return;

   if(UData.charge != CHARGE_MAX)
      SetFade(fid_jet, 1, 16);

   if(!CheckFade(fid_jet)) return;

   k32 rocket = UData.charge / (k32)CHARGE_MAX;
   i32 max    = rocket * 15;

   PrintSpriteF(sp_HUD_H_B3, 320,2, 80,1, fid_jet);
   PrintTextF_str(st_jet, s_smallfnt, CR_RED, 320,2, 160,1, fid_jet);

   for(i32 i = 0; i < max; i++)
      PrintSpriteFP(UData.discharged ? sp_HUD_H_C1 : sp_HUD_H_C2,
                    320,2, 150 - i * 5,1, fid_jet);
}

/* EOF */
