#include "lith_upgrades_common.h"

#define UserData upgr->UserData_VitalScan


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Update
//
[[__call("ScriptS")]]
void Upgr_VitalScan_Update(player_t *p, upgrade_t *upgr)
{
   ACS_SetActivator(0, AAPTR_PLAYER_GETTARGET);
   
   if((UserData.target = ACS_CheckFlag(0, "COUNTKILL") || ACS_PlayerNumber() != -1))
   {
      UserData.tagstr    = StrParam("%tS", 0);
      UserData.health    = ACS_GetActorProperty(0, APROP_Health);
      UserData.maxhealth = ACS_GetActorProperty(0, APROP_SpawnHealth);
      UserData.angle     = atan2f(p->y - ACS_GetActorY(0), p->x - ACS_GetActorX(0));
   }
}

//
// Render
//
void Upgr_VitalScan_Render(player_t *p, upgrade_t *upgr)
{
   if(!p->upgrades[UPGR_HeadsUpDisp].active || !UserData.target) return;
   
   int ofs = 0;
   
   if(ACS_GetUserCVar(p->number, "lith_hud_movescanner"))
   {
      float diff = p->yawf - UserData.angle;
      ofs = (UserData.old = lerpf(UserData.old, atan2f(sinf(diff), cosf(diff)), 0.1)) * 64;
   }
   
   HudMessageF("CBIFONT", "%S\n%i/%i", UserData.tagstr, UserData.health, UserData.maxhealth);
   HudMessageParams(HUDMSG_FADEOUT, hid_vitalscanner, CR_WHITE, 160.4 + ofs, 180.2, 0.1, 0.4);
}

// EOF

