// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

#define UData UData_ReflexWetw(upgr)

#define CHARGE_MAX (35 * 0.8)

// Extern Functions ----------------------------------------------------------|

script ext("ACS")
void Lith_DodgeView()
{
   withplayer(LocalPlayer)
   {
      fixed vh = p->viewheight;

      for(int i = 0; i < 20; i++)
      {
         fixed mul = 1.0 - (ACS_Sin(i / 40.0) * 0.6);
         SetPropK(0, APROP_ViewHeight, vh * mul);
         ACS_Delay(1);
      }

      SetPropK(0, APROP_ViewHeight, vh);
   }
}

stkcall
void Upgr_ReflexWetw_Activate(struct player *p, upgrade_t *upgr)
{
   #if LITHIUM
   p->speedmul += 0.3;
   #endif
   UData.charge = CHARGE_MAX;
   UData.leaped = 0;
}

stkcall
void Upgr_ReflexWetw_Deactivate(struct player *p, upgrade_t *upgr)
{
   #if LITHIUM
   p->speedmul -= 0.3;
   #endif
}

script
void Upgr_ReflexWetw_Update(struct player *p, upgrade_t *upgr)
{
   if(UData.charge < CHARGE_MAX)
      UData.charge++;

   if(p->frozen) return;

   if(UData.charge >= CHARGE_MAX)
   {
      if(p->onground) UData.leaped = 0;

      if(p->buttons & BT_SPEED && (p->onground ||
         !p->getUpgrActive(UPGR_JetBooster) ||
         UData_JetBooster(p->getUpgr(UPGR_JetBooster)).discharged))
      {
         fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);

         ACS_LocalAmbientSound("player/slide", 127);
         p->setVel(p->velx + (ACS_Cos(angle) * 32.0), p->vely + (ACS_Sin(angle) * 32.0), 0);

         Lith_DodgeView();

         UData.charge = 0;
      }
   }

   if(p->waterlevel == 0 && p->buttonPressed(BT_JUMP) && !InvNum("PowerFlight") &&
      !InvNum(OBJ "RocketBooster") && !UData.leaped &&
      (!p->onground || UData.charge < CHARGE_MAX))
   {
      fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);

      ACS_LocalAmbientSound("player/doublejump", 127);
      p->setVel(p->velx + (ACS_Cos(angle) * 4.0), p->vely + (ACS_Sin(angle) * 4.0), 12.0);

      UData.leaped = 1;
   }
}

// EOF

