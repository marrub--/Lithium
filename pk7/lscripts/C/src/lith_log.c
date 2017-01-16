#include "lith_common.h"
#include "lith_log.h"
#include "lith_player.h"
#include "lith_list.h"
#include "lith_hudid.h"

#include <stdio.h>

#define LOG_TIME 140

#define DrawMsg(hid, flags) \
   ( \
      HudMessageF("LOGFONT", "%S", logdata->info), \
      HudMessageParams(HUDMSG_NOWRAP|HUDMSG_FADEOUT|flags, hid, CR_GREEN, 0.1, 262.2 - (10 * i), TICSECOND, 0.1) \
   )


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_LogName
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_LogName(int name)
{
   player_t *p = Lith_LocalPlayer;
   
   __str ls;
   
   switch(name)
   {
#define N(name, str) case name: ls = "LITH_TXT_LOG_" str; break;
   N(log_default,     "Default")
   N(log_allmap,      "AllMap")
   N(log_armorbonus,  "ArmorBonus")
   N(log_backpack,    "Backpack")
   N(log_berserk,     "Berserk")
   N(log_bluearmor,   "BlueArmor")
   N(log_blursphere,  "BlurSphere")
   N(log_greenarmor,  "GreenArmor")
   N(log_healthbonus, "HealthBonus")
   N(log_infrared,    "InfraRed")
   N(log_invulnerability, "Invulnerability")
   N(log_medikit,     "Medikit")
   N(log_megasphere,  "MegaSphere")
   N(log_radsuit,     "RadSuit")
   N(log_soulsphere,  "SoulSphere")
   N(log_stimpack,    "StimPack")
   N(log_redcard,     "RedCard")
   N(log_bluecard,    "BlueCard")
   N(log_yellowcard,  "YellowCard")
   N(log_redskull,    "RedSkull")
   N(log_blueskull,   "BlueSkull")
   N(log_yellowskull, "YellowSkull")
   N(log_doggosphere, "DoggoSphere")
   N(log_dogs,        "Dogs")
#undef N
   }
   
   Lith_Log(p, "%LS", ls);
}

//
// Lith_Log
//
void Lith_Log(player_t *p, __str fmt, ...)
{
   va_list vl;
   
   logdata_t *logdata = calloc(1, sizeof(logdata_t));
   Lith_LinkDefault(&logdata->link, logdata);
   
   ACS_BeginPrint();
   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);
   
   logdata->info = ACS_EndStrParam();
   logdata->time = LOG_TIME;
   
   Lith_ListLink(&p->log, &logdata->link);
   
   if(Lith_ListSize(&p->log) > LOG_MAX)
      Lith_ListUnlink(p->log.next, free);
}

//
// Lith_PlayerUpdateLog
//
[[__call("ScriptS")]]
void Lith_PlayerUpdateLog(player_t *p)
{
   for(list_t *rover = p->log.next; rover != &p->log;)
   {
      logdata_t *logdata = rover->object;
      
      if(logdata->time == 0)
      {
         list_t *next = rover->next;
         Lith_ListUnlink(rover, free);
         rover = next;
      }
      else
      {
         logdata->time--;
         rover = rover->next;
      }
   }
}

//
// Lith_HUD_Log
//
[[__call("ScriptS")]]
void Lith_HUD_Log(player_t *p)
{
   ACS_SetHudSize(480, 300);
   
   int i = 0;
   for(list_t *rover = p->log.next; rover != &p->log; rover = rover->next, i++)
   {
      logdata_t *logdata = rover->object;
      
      DrawMsg(hid_logE + i, HUDMSG_PLAIN);
      
      if(logdata->time > LOG_TIME - 10)
         DrawMsg(hid_logAddE + i, HUDMSG_ADDBLEND);
   }
}

// EOF

