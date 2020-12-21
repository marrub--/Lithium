/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * ReflexWetw upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

#define UData p->upgrdata.reflexwetw

#define CHARGE_MAX (35 * 0.8)

/* Static Functions -------------------------------------------------------- */

alloc_aut(0) script static
void DodgeView(struct player *p)
{
   k32 vh = p->viewheight;

   for(i32 i = 0; i < 20; i++)
   {
      k32 mul = 1.0 - (ACS_Sin(i / 40.0) * 0.6);
      SetViewHeight(0, vh * mul);
      ACS_Delay(1);
   }

   SetViewHeight(0, vh);
}

/* Extern Functions -------------------------------------------------------- */

void Upgr_ReflexWetw_Activate(struct player *p, struct upgrade *upgr)
{
   p->speedmul += 0.3;
   UData.charge = CHARGE_MAX;
   UData.leaped = 0;
}

void Upgr_ReflexWetw_Deactivate(struct player *p, struct upgrade *upgr)
{
   p->speedmul -= 0.3;
}

script
void Upgr_ReflexWetw_Update(struct player *p, struct upgrade *upgr)
{
   if(UData.charge < CHARGE_MAX)
      UData.charge++;

   if(p->frozen) return;

   if(UData.charge >= CHARGE_MAX)
   {
      if(p->onground) UData.leaped = 0;

      if(p->buttons & BT_SPEED &&
         (p->onground ||
          !get_bit(p->upgrades[UPGR_JetBooster].flags, _ug_active) ||
         p->upgrdata.jetbooster.discharged))
      {
         k32 angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);

         ACS_LocalAmbientSound(ss_player_slide, 127);
         P_SetVel(p, p->velx + (ACS_Cos(angle) * 32.0), p->vely + (ACS_Sin(angle) * 32.0), 0);

         DodgeView(p);

         UData.charge = 0;
      }
   }

   Str(so_PowerFlight, s"PowerFlight");
   if(p->waterlevel == 0 && P_ButtonPressed(p, BT_JUMP) && !InvNum(so_PowerFlight) &&
      !InvNum(so_RocketBooster) && !UData.leaped &&
      (!p->onground || UData.charge < CHARGE_MAX))
   {
      k32 angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);

      ACS_LocalAmbientSound(ss_player_doublejump, 127);
      P_SetVel(p, p->velx + (ACS_Cos(angle) * 4.0), p->vely + (ACS_Sin(angle) * 4.0), 12.0);

      UData.leaped = 1;
   }
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "DodgeView")
void Sc_DodgeView(void)
{
   with_player(LocalPlayer) DodgeView(p);
}

script_str ext("ACS") addr(OBJ "DodgeViewShadowWalk")
void Sc_DodgeViewShadowWalk(void) {
   with_player(LocalPlayer) {
      k32 vh = p->viewheight;

      for(i32 i = 0; i < 105; i++) {
         k32 mul = 1.0k - (ACS_Sin(i / 105.0k / 2.0k) * 0.3k) - 0.1k;
         SetViewHeight(0, vh * mul);
         ACS_Delay(1);
      }

      SetViewHeight(0, vh);
   }
}

/* EOF */
