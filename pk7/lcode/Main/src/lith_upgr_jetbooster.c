#include "lith_upgrades_common.h"

#define CHARGE_MAX (35 * 7)


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Activate
//
void Upgr_JetBooster_Activate(player_t *p, upgrade_t *upgr)
{
   upgr->user_int[u_charge] = CHARGE_MAX;
}

//
// Update
//
[[__call("ScriptS")]]
void Upgr_JetBooster_Update(player_t *p, upgrade_t *upgr)
{
   if(upgr->user_int[u_charge] < CHARGE_MAX)
      upgr->user_int[u_charge]++;
   
   if(p->frozen) return;
   
   fixed grounddist = p->z - p->floorz;
   
   if(ButtonPressed(p, BT_SPEED) && grounddist > 16.0 && upgr->user_int[u_charge] >= CHARGE_MAX)
   {
      fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
      
      ACS_PlaySound(0, "player/rocketboost");
      ACS_GiveInventory("Lith_RocketBooster", 1);
      Lith_SetPlayerVelocity(p,
                           p->velx + (cosk(angle) * 16.0),
                           p->vely + (sink(angle) * 16.0),
                           10.0, false, true);
      
      upgr->user_int[u_charge] = 0;
      p->upgrades[UPGR_ReflexWetw].user_int[u_leaped] = 0;
   }
}

//
// Render
//
void Upgr_JetBooster_Render(player_t *p, upgrade_t *upgr)
{
   if(upgr->user_int[u_charge] != CHARGE_MAX && p->upgrades[UPGR_HeadsUpDisp].active)
   {
      fixed rocket = upgr->user_int[u_charge] / (fixed)CHARGE_MAX;
      int max = (hid_jetS - hid_jetE) * rocket;
      
      DrawSpriteFade("H_B3", hid_jetbg, 320.2, 80.1, 0.0, 0.5);
      HudMessageF("SMALLFNT", "J\nE\nT");
      HudMessageParams(HUDMSG_FADEOUT, hid_jettext, CR_RED, 305.2, 150.2, 0.1, 0.5);
      
      for(int i = 0; i < max; i++)
         DrawSprite(max < 4 ? "H_C2" : "H_C1",
            HUDMSG_FADEOUT | HUDMSG_ADDBLEND | HUDMSG_NOTWITHFULLMAP | HUDMSG_ALPHA,
            hid_jetS - i,
            320.2,
            150.1 - (i * 5),
            0.1, 0.5, 0.5);
   }
}

// EOF

