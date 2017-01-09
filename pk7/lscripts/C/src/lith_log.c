#include "lith_common.h"
#include "lith_log.h"
#include "lith_player.h"
#include "lith_list.h"
#include "lith_hudid.h"

#include <stdio.h>

#define LOG_TIME 140


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
   N(log_default, "DEFAULT")
   N(log_allmap,  "ALLMAP")
   N(log_armorbonus, "ARMORBONUS")
   N(log_backpack,   "BACKPACK")
   N(log_berserk,    "BERSERK")
   N(log_bluearmor,   "BLUEARMOR")
   N(log_blursphere,  "BLURSPHERE")
   N(log_greenarmor,  "GREENARMOR")
   N(log_healthbonus, "HEALTHBONUS")
   N(log_infrared,    "INFRARED")
   N(log_invulnerability, "INVULNERABILITY")
   N(log_medikit,    "MEDIKIT")
   N(log_megasphere, "MEGASPHERE")
   N(log_radsuit,    "RADSUIT")
   N(log_soulsphere, "SOULSPHERE")
   N(log_stimpack,   "STIMPACK")
   N(log_redcard,    "REDCARD")
   N(log_bluecard,   "BLUECARD")
   N(log_yellowcard, "YELLOWCARD")
   N(log_redskull,    "REDSKULL")
   N(log_blueskull,   "BLUECARD")
   N(log_yellowskull, "YELLOWSKULL")
#undef N
   }
   
   Lith_Log(p, "%LS", ls);
}

//
// Lith_Log
//
void Lith_Log(player_t *p, __str fmt, ...)
{
   logdata_t *logdata = null;
   
   for(int i = 0; i < LOG_MAX; i++)
      if(p->logdata[i].time == 0)
         logdata = &p->logdata[i];
   
   if(!logdata)
   {
      logdata = p->log->head->data.vp;
      DList_DeleteFront(p->log);
   }
   
   va_list vl;
   
   ACS_BeginPrint(); va_start(vl, fmt); __vnprintf_str(fmt, vl); va_end(vl);
   logdata->info = ACS_EndStrParam();
   logdata->time = LOG_TIME;
   
   DList_InsertBack(p->log, (listdata_t){.vp = logdata});
}

//
// Lith_HUD_Log
//
[[__call("ScriptS")]]
void Lith_HUD_Log(player_t *p)
{
   int i = 0;
   ACS_SetHudSize(480, 300);
   for(slist_t *rover = p->log->head; rover;)
   {
      logdata_t *logdata = rover->data.vp;
      
      if(logdata->time == 0)
      {
         slist_t *next = rover->next;
         DList_Remove(p->log, rover);
         rover = next;
         continue;
      }
      else logdata->time--;
      
      HudMessageF("LOGFONT", "%S", logdata->info);
      HudMessageParams(HUDMSG_NOWRAP|HUDMSG_FADEOUT, hid_logE + i, CR_GREEN, 0.1, 262.2 - (10 * i), TICSECOND, 0.1);
      
      if(logdata->time > LOG_TIME - 10)
      {
         HudMessageF("LOGFONT", "%S", logdata->info);
         HudMessageParams(HUDMSG_NOWRAP|HUDMSG_FADEOUT|HUDMSG_ADDBLEND, hid_logAddE + i, CR_GREEN, 0.1, 262.2 - (10 * i), TICSECOND, 0.1);
      }
      
      rover = rover->next;
      i++;
   }
}

// EOF

