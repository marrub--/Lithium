#include "lith_upgrades_common.h"
#include "lith_world.h"

#define UserData upgr->UserData_VitalScan


//----------------------------------------------------------------------------
// Extern Functions
//

__str Lith_RandomName(int id);

//
// Update
//
[[__call("ScriptS")]]
void Upgr_VitalScan_Update(player_t *p, upgrade_t *upgr)
{
   ACS_SetActivator(0, AAPTR_PLAYER_GETTARGET);
   
   bool six = ACS_StrCmp(ACS_GetActorClass(0), "RLDeVileSix", 11) == 0;
   
   bool validtarget =
      six ||
      ACS_CheckFlag(0, "COUNTKILL") ||
      ACS_PlayerNumber() != -1;
   
   bool freaktarget =
      ACS_CheckFlag(0, "INVULNERABLE") ||
      ACS_CheckFlag(0, "NODAMAGE");
   
   bool boss = ACS_CheckFlag(0, "BOSS");
   
   if(ACS_GetActorProperty(0, APROP_Health) <= 0)
      UserData.target = UserData.oldtarget = 0;
   
   else if(validtarget)
   {
      int id = Lith_UniqueID();
      
      if(freaktarget || boss)
      {
         UserData.tagstr    = Lith_RandomName(boss ? id : 0);
         UserData.oldhealth = UserData.health = ACS_Random(0, 666666);
         UserData.maxhealth = ACS_Random(0, 666666);
      }
      else
      {
         if(six)
            UserData.tagstr = "\Cg6";
         else
            UserData.tagstr = StrParam("%tS", 0);
         
         UserData.oldhealth = UserData.health;
         UserData.health    = ACS_GetActorProperty(0, APROP_Health);
         UserData.maxhealth = ACS_GetActorProperty(0, APROP_SpawnHealth);
      }
      
      UserData.freak = six || freaktarget || boss;
      UserData.angle = atan2f(p->y - ACS_GetActorY(0), p->x - ACS_GetActorX(0));
      
      if((UserData.oldtarget = UserData.target) != (UserData.target = id))
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
      HudMessageParams(HUDMSG_FADEOUT, hid_vitalscanhitS, CR_RED, 160.4 + ofs, 196.2, 0.1, 0.4);
      
      for(int i = 1; i < 5; i++)
      {
         if(delta < 100 * i) break;
         
         HudMessageF("CBIFONT", "-%i", delta);
         HudMessageParams(HUDMSG_FADEOUT|HUDMSG_ADDBLEND, hid_vitalscanhitS - i, CR_RED, 160.4 + ofs, 196.2, 0.1, 0.4);
      }
   }
   
   HudMessageF("CBIFONT", "%S", UserData.tagstr);
   HudMessageParams(HUDMSG_FADEOUT, hid_vitalscannertag, CR_WHITE, 160.4 + ofs, 180.2, 0.1, 0.4);
   
   HudMessageF(UserData.freak ? "ALIENFONT" : "CBIFONT", "%i/%i", UserData.health, UserData.maxhealth);
   HudMessageParams(HUDMSG_FADEOUT, hid_vitalscannerhp, CR_WHITE, 160.4 + ofs, 188.2, 0.1, 0.4);
}

// EOF

