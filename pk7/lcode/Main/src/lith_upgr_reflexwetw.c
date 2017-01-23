#include "lith_upgrades_common.h"

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
   upgr->user_int[u_charge] = CHARGE_MAX;
   upgr->user_int[u_leaped] = 0;
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
   if(upgr->user_int[u_charge] < CHARGE_MAX)
      upgr->user_int[u_charge]++;
   
   if(p->frozen) return;
   
   fixed grounddist = p->z - p->floorz;
   
   if(upgr->user_int[u_charge] >= CHARGE_MAX)
   {
      if(grounddist == 0.0)
         upgr->user_int[u_leaped] = 0;
      
      if(p->buttons & BT_SPEED &&
         (grounddist <= 16.0 || !p->upgrades[UPGR_JetBooster].active))
      {
         fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
         
         ACS_PlaySound(0, "player/slide");
         Lith_SetPlayerVelocity(p, p->velx + (cosk(angle) * 32.0), p->vely + (sink(angle) * 32.0), 0, false, true);
         
         DOOOOODGE(p);
         
         upgr->user_int[u_charge] = 0;
      }
   }
   
   if(ButtonPressed(p, BT_JUMP) &&
      !ACS_CheckInventory("Lith_RocketBooster") && !upgr->user_int[u_leaped] &&
      ((grounddist <= 16.0 && upgr->user_int[u_charge] < CHARGE_MAX) || grounddist > 16.0))
   {
      fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
      
      ACS_PlaySound(0, "player/doublejump");
      Lith_SetPlayerVelocity(p, p->velx + (cosk(angle) * 4.0), p->vely + (sink(angle) * 4.0), 12.0, false, true);
      
      upgr->user_int[u_leaped] = 1;
   }
}

//
// Render
//
void Upgr_ReflexWetw_Render(player_t *p, upgrade_t *upgr)
{
   if(p->upgrades[UPGR_HeadsUpDisp].active)
   {
      int  time11 = p->ticks % 11;
      float slide = upgr->user_int[u_charge] / (float)CHARGE_MAX;
      
      DrawSprite(slide < 0.95f ? "H_D21" : "H_D24",
         HUDMSG_FADEOUT | HUDMSG_ALPHA,
         hid_slideind_fxS - time11,
         77.1 - time11,
         188.1 + (11 - time11),
         (fixed)(0.3f * slide),
         (fixed)(0.6f * slide),
         0.8);
   }
}

// EOF

