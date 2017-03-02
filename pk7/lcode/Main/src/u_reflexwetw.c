#include "lith_upgrades_common.h"

#define UserData upgr->UserData_ReflexWetw

#define CHARGE_MAX (35 * 0.8)


//----------------------------------------------------------------------------
// Static Functions
//

//
// DOOOOODGE
//
[[__call("ScriptS")]]
static void DOOOOODGE(player_t *p)
{
   fixed vh = p->viewheight;
   
   for(int i = 0; i < 20; i++)
   {
      fixed mul = 1.0 - (sink(i / 40.0) * 0.6);
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
   UserData.charge = CHARGE_MAX;
   UserData.leaped = 0;
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
   if(UserData.charge < CHARGE_MAX)
      UserData.charge++;
   
   if(p->frozen) return;
   
   fixed grounddist = p->z - p->floorz;
   
   if(UserData.charge >= CHARGE_MAX)
   {
      if(grounddist == 0.0)
         UserData.leaped = 0;
      
      if(p->buttons & BT_SPEED &&
         (grounddist <= 16.0 || !p->upgrades[UPGR_JetBooster].active))
      {
         fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
         
         ACS_PlaySound(0, "player/slide");
         Lith_SetPlayerVelocity(p, p->velx + (cosk(angle) * 32.0), p->vely + (sink(angle) * 32.0), 0, false, true);
         
         DOOOOODGE(p);
         
         UserData.charge = 0;
      }
   }
   
   if(ButtonPressed(p, BT_JUMP) &&
      !ACS_CheckInventory("Lith_RocketBooster") && !UserData.leaped &&
      ((grounddist <= 16.0 && UserData.charge < CHARGE_MAX) || grounddist > 16.0))
   {
      fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
      
      ACS_PlaySound(0, "player/doublejump");
      Lith_SetPlayerVelocity(p, p->velx + (cosk(angle) * 4.0), p->vely + (sink(angle) * 4.0), 12.0, false, true);
      
      UserData.leaped = 1;
   }
}

//
// Render
//
void Upgr_ReflexWetw_Render(player_t *p, upgrade_t *upgr)
{
   if(!p->upgrades[UPGR_HeadsUpDisp].active) return;
   
   int  time11 = p->ticks % 11;
   float slide = UserData.charge / (float)CHARGE_MAX;
   
   DrawSprite(slide < 0.95f ? "H_D21" : "H_D24",
      HUDMSG_FADEOUT | HUDMSG_ALPHA,
      hid_slideind_fxS - time11,
      77.1 - time11,
      188.1 + (11 - time11),
      (fixed)(0.3f * slide),
      (fixed)(0.6f * slide),
      0.8);
}

// EOF

