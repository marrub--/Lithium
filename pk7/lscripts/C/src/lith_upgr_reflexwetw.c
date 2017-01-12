#include "lith_upgrades_common.h"


//----------------------------------------------------------------------------
// Static Functions
//

//
// DOOOOODGE
//
[[__call("ScriptS")]]
static void DOOOOODGE(player_t *p)
{
   _Accum vh = p->viewheight;
   
   for(int i = 0; i < 20; i++)
   {
      _Accum mul = 1.0 - (sink(i / 40.0) * 0.6);
      ACS_SetActorPropertyFixed(0, APROP_ViewHeight, vh * mul);
      ACS_Delay(1);
   }
   
   ACS_SetActorPropertyFixed(0, APROP_ViewHeight, vh);
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Activate
//
void Upgr_ReflexWetw_Activate(player_t *p, upgrade_t *upgr)
{
   p->speedmul += 0.3;
}

//
// Deactivate
//
void Upgr_ReflexWetw_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->speedmul -= 0.3;
}

//
// Update
//
[[__call("ScriptS")]]
void Upgr_ReflexWetw_Update(player_t *p, upgrade_t *upgr)
{
   if(p->slidecharge < slidecharge_max)
      p->slidecharge++;
   
   if(p->frozen) return;
   
   fixed grounddist = p->z - p->floorz;
   
   if(p->slidecharge >= slidecharge_max)
   {
      if(grounddist == 0.0)
         p->leaped = false;
      
      if(p->buttons & BT_SPEED &&
         (grounddist <= 16.0 || !p->upgrades[UPGR_JetBooster].active))
      {
         fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
         
         ACS_PlaySound(0, "player/slide");
         Lith_SetPlayerVelocity(p, p->velx + (cosk(angle) * 32.0),
                                 p->vely + (sink(angle) * 32.0),
                                 0,
                              false, true);
         
         DOOOOODGE(p);
         
         p->slidecharge = 0;
      }
   }
   
   if(ButtonPressed(p, BT_JUMP) &&
      !ACS_CheckInventory("Lith_RocketBooster") && !p->leaped &&
      ((grounddist <= 16.0 && p->slidecharge < slidecharge_max) || grounddist > 16.0))
   {
      fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
      
      ACS_PlaySound(0, "player/doublejump");
      Lith_SetPlayerVelocity(p, p->velx + (cosk(angle) * 4.0),
                              p->vely + (sink(angle) * 4.0),
                              12.0,
                           false, true);
      
      p->leaped = true;
   }
}

// EOF

