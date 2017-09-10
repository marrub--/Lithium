// Copyright © 2017 Graham Sanderson, all rights reserved.
#include "lith_player.h"
#include "lith_world.h"

// Static Functions ----------------------------------------------------------|

//
// Dodge
//
static void Dodge(player_t *p)
{
   fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
   fixed xn = ACS_Cos(angle), yn = ACS_Sin(angle);
   bool dodged = false;

   for(int i = 1; i < 32; i++)
   {
      fixed xv = p->x + xn * i * 8;
      fixed yv = p->y + yn * i * 8;
      fixed floorz = ACS_GetSectorFloorZ(0, xv, yv);
      fixed zdiff = minmax(floorz - p->z, -24, 24);
      int tid;

      if(!ACS_Warp(0, xv, yv, p->z + zdiff, 0, WARPF_ABSOLUTEPOSITION))
         break;
      else
         dodged = true;
   }

   if(dodged) ACS_LocalAmbientSound("player/informant/dodge", 127);
}

// Extern Functions ----------------------------------------------------------|

//
// Cpk1_PlayerUpdate
//
void Cpk1_PlayerUpdate(player_t *p)
{
   DebugStat("CPK1 PlayerUpdate %i\n", p->num);

   if(p->pclass == pcl_informant)
   {
      if(p->buttonPressed(BT_SPEED))
         Dodge(p);
   }
}

// EOF
