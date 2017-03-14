#include "lith_common.h"
#include "lith_log.h"
#include "lith_player.h"
#include "lith_list.h"
#include "lith_hudid.h"
#include "lith_world.h"

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
   player_t *p = LocalPlayer;
   
   switch(name)
   {
#define BOTH(name, str) case name: p->log ("%LS", "LITH_TXT_LOG_" str); break;
#define FULL(name, str) case name: p->logF("%LS", "LITH_TXT_LOG_" str); break;
#define HUDS(name, str) case name: p->logH("%LS", "LITH_TXT_LOG_" str); break;
   BOTH(log_default,     "Default")
   BOTH(log_allmap,      "AllMap")
   HUDS(log_armorbonus,  "ArmorBonus")
   BOTH(log_backpack,    "Backpack")
   BOTH(log_berserk,     "Berserk")
   BOTH(log_bluearmor,   "BlueArmor")
   BOTH(log_blursphere,  "BlurSphere")
   BOTH(log_greenarmor,  "GreenArmor")
   HUDS(log_healthbonus, "HealthBonus")
   BOTH(log_infrared,    "InfraRed")
   BOTH(log_invulnerability, "Invulnerability")
   BOTH(log_medikit,     "Medikit")
   BOTH(log_megasphere,  "MegaSphere")
   BOTH(log_radsuit,     "RadSuit")
   BOTH(log_soulsphere,  "SoulSphere")
   BOTH(log_stimpack,    "StimPack")
   BOTH(log_redcard,     "RedCard")
   BOTH(log_bluecard,    "BlueCard")
   BOTH(log_yellowcard,  "YellowCard")
   BOTH(log_redskull,    "RedSkull")
   BOTH(log_blueskull,   "BlueSkull")
   BOTH(log_yellowskull, "YellowSkull")
   BOTH(log_doggosphere, "DoggoSphere")
   BOTH(log_dogs,        "Dogs")
#undef BOTH
#undef FULL
#undef HUDS
   }
}

//
// Lith_Log
//
void Lith_Log(player_t *p, __str fmt, ...)
{
   va_list vl;
   
   va_start(vl, fmt);
   logdata_t *logdata = Lith_LogV(p, fmt, vl);
   va_end(vl);
   
   logdata->time = LOG_TIME;
   logdata->link    .link(&p->loginfo.hud);
   logdata->linkfull.link(&p->loginfo.full);
   
   if(p->loginfo.hud.size > LOG_MAX)
      free(p->loginfo.hud.next->unlink());
}

//
// Lith_LogF
//
void Lith_LogF(player_t *p, __str fmt, ...)
{
   va_list vl;
   
   va_start(vl, fmt);
   logdata_t *logdata = Lith_LogV(p, fmt, vl);
   va_end(vl);
   
   logdata->linkfull.link(&p->loginfo.full);
}

//
// Lith_LogH
//
void Lith_LogH(player_t *p, __str fmt, ...)
{
   va_list vl;
   
   va_start(vl, fmt);
   logdata_t *logdata = Lith_LogV(p, fmt, vl);
   va_end(vl);
   
   logdata->time = LOG_TIME;
   logdata->link.link(&p->loginfo.hud);
   
   if(p->loginfo.hud.size > LOG_MAX)
      free(p->loginfo.hud.next->unlink());
}

//
// Lith_LogV
//
logdata_t *Lith_LogV(player_t *p, __str fmt, va_list vl)
{
   logdata_t *logdata = calloc(1, sizeof(logdata_t));
   logdata->link    .construct(logdata);
   logdata->linkfull.construct(logdata);
   
   ACS_BeginPrint();
   __vnprintf_str(fmt, vl);
   
   logdata->info = ACS_EndStrParam();
   logdata->from = world.mapnum;
   
   return logdata;
}

//
// Lith_PlayerUpdateLog
//
[[__call("ScriptS")]]
void Lith_PlayerUpdateLog(player_t *p)
{
   for(list_t *rover = p->loginfo.hud.next; rover != &p->loginfo.hud;)
   {
      logdata_t *logdata = rover->object;
      
      if(logdata->time == 0)
      {
         list_t *next = rover->next;
         rover->unlink();
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
   Lith_ForListIter(logdata_t *logdata, p->loginfo.hud, i++)
   {
      DrawMsg(hid_logE + i, HUDMSG_PLAIN);
      
      if(logdata->time > LOG_TIME - 10)
         DrawMsg(hid_logAddE + i, HUDMSG_ADDBLEND);
   }
}

//
// Lith_PlayerLogEntry
//
void Lith_PlayerLogEntry(player_t *p)
{
   logmap_t *logmap = calloc(1, sizeof(logmap_t));
   logmap->link.construct(logmap);
   
   logmap->levelnum = world.mapnum;
   logmap->name = StrParam("%tS", PRINTNAME_LEVELNAME); // :|
   
   logmap->link.link(&p->loginfo.maps);
   
   int seconds = 53 + (p->ticks / 35);
   int minutes = 30 + (seconds  / 60);
   int hours   = 14 + (minutes  / 60);
   int days    = 25 + (hours    / 24); // pls
   
   p->logF("Entered %S at %0.2i:%0.2i:%0.2i %i/7/1649 NE",
      logmap->name, hours % 24, minutes % 60, seconds % 60, days);
}

//
// Lith_CBITab_Log
//
void Lith_CBITab_Log(gui_state_t *g, player_t *p)
{
   size_t num = 0;
   int i = 0;
   
   logmap_t *selmap;
   list_t *sel = g->st[st_logsel].vp;
   
   if((sel = g->st[st_logsel].vp) == null)
      sel = p->loginfo.maps.next;
   
   if(Lith_GUI_Button(g, .x = 25, 28, .preset = &btnprev))
      if((sel = sel->prev) == &p->loginfo.maps)
         sel = sel->prev;
   
   if(Lith_GUI_Button(g, .x = 25 + btnprev.w, 28, .preset = &btnnext))
      if((sel = sel->next) == &p->loginfo.maps)
         sel = sel->next;
   
   g->st[st_logsel].vp = sel;
   selmap = sel->object;
   
   HudMessageF("CBIFONT", "%S", selmap->name);
   HudMessagePlain(g->hid--, 28.1 + btnprev.w + btnnext.w, 30.1, TICSECOND);
   
   Lith_ForList(logdata_t *logdata, p->loginfo.full)
      num += (logdata->from == selmap->levelnum);
   
   Lith_GUI_ScrollBegin(g, st_logscr, 15, 40, 280, 188, num * 8);
   
   Lith_ForList(logdata_t *logdata, p->loginfo.full)
   {
      if(logdata->from != selmap->levelnum)
         continue;
      
      int y = 8 * i++;
      
      if(Lith_GUI_ScrollOcclude(g, st_logscr, y, 8))
         continue;
      
      DrawSpritePlain("lgfx/UI/LogList.png", g->hid--, g->ox + 0.1, y + g->oy + 0.1, TICSECOND);
      
      HudMessageF("CBIFONT", "%S", logdata->info);
      HudMessageParams(0, g->hid--, CR_GREEN, g->ox + 2.1, y + g->oy + 1.1, TICSECOND);
   }
   
   
   Lith_GUI_ScrollEnd(g, st_logscr);
}

// EOF

