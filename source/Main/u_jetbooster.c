// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// u_jetbooster.c: JetBooster upgrade.

#include "u_common.h"

#define UData UData_JetBooster(upgr)

#define CHARGE_MAX (35 * 7)

// Extern Functions ----------------------------------------------------------|

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

   if(p->buttonPressed(BT_SPEED) && !p->onground && UData.charge >= CHARGE_MAX)
   {
      k32 angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);

      ACS_PlaySound(0, ss_player_rocketboost);
      InvGive(so_RocketBooster, 1);
      p->setVel(p->velx + (ACS_Cos(angle) * 16.0), p->vely + (ACS_Sin(angle) * 16.0), 10.0);

      UData.charge = 0;
      UData_ReflexWetw(p->getUpgr(UPGR_ReflexWetw)).leaped = 0;
   }
}

stkcall
void Upgr_JetBooster_Render(struct player *p, struct upgrade *upgr)
{
   if(!p->hudenabled || UData.charge == CHARGE_MAX) return;

   k32 rocket = UData.charge / (k32)CHARGE_MAX;
   i32 max = (hid_jetS - hid_jetE) * rocket;

   DrawSpriteFade(sp_HUD_H_B3, hid_jetbg, 320.2, 80.1, 0.0, 0.5);

   ACS_SetFont(s_smallfnt);
   ACS_BeginPrint();
   ACS_PrintString(st_jet);
   ACS_MoreHudMessage();
   HudMessageParams(HUDMSG_FADEOUT, hid_jettext, CR_RED, 320.2, 160.1, 0.1, 0.5);

   for(i32 i = 0; i < max; i++)
      DrawSpriteXX(UData.discharged ? sp_HUD_H_C1 : sp_HUD_H_C2,
         HUDMSG_FADEOUT | HUDMSG_ADDBLEND | HUDMSG_ALPHA,
         hid_jetS - i,
         320.2,
         150.1 - (i * 5),
         0.1, 0.5, 0.5);
}

// EOF

