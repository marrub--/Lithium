#include "lith_upgrades_common.h"
#include "lith_world.h"

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
   
   if(ACS_GetActorProperty(0, APROP_Health) <= 0)
      UserData.target = UserData.oldtarget = 0;
   
   else if(ACS_CheckFlag(0, "COUNTKILL") || ACS_PlayerNumber() != -1)
   {
      UserData.tagstr    = StrParam("%tS", 0);
      UserData.oldhealth = UserData.health;
      UserData.health    = ACS_GetActorProperty(0, APROP_Health);
      UserData.maxhealth = ACS_GetActorProperty(0, APROP_SpawnHealth);
      UserData.angle     = atan2f(p->y - ACS_GetActorY(0), p->x - ACS_GetActorX(0));
      
      if((UserData.oldtarget = UserData.target) != (UserData.target = Lith_UniqueID(0)))
         UserData.oldhealth = UserData.health;
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
   
   if(UserData.health < UserData.oldhealth)
   {
      int delta = UserData.oldhealth - UserData.health;
      
      HudMessageF("CBIFONT", "-%i", delta);
      HudMessageParams(HUDMSG_FADEOUT, hid_vitalscanhitS, CR_RED, 160.4 + ofs, 188.2, 0.1, 0.4);
      
      for(int i = 1; i < 5; i++)
      {
         if(delta < 100 * i) break;
         
         HudMessageF("CBIFONT", "-%i", delta);
         HudMessageParams(HUDMSG_FADEOUT|HUDMSG_ADDBLEND, hid_vitalscanhitS - i, CR_RED, 160.4 + ofs, 188.2, 0.1, 0.4);
      }
   }
   
   HudMessageF("CBIFONT", "%S\n%i/%i", UserData.tagstr, UserData.health, UserData.maxhealth);
   HudMessageParams(HUDMSG_FADEOUT, hid_vitalscanner, CR_WHITE, 160.4 + ofs, 180.2, 0.1, 0.4);
}

// EOF

