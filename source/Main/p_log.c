// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_log.h"
#include "lith_player.h"
#include "lith_list.h"
#include "lith_hudid.h"
#include "lith_world.h"

#include <stdio.h>

// Extern Functions ----------------------------------------------------------|

//
// Lith_LogName
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_LogName(int name)
{
   withplayer(LocalPlayer)
      switch(name)
   {
#define BOTH(name) case msg_##name: p->log ("%LS", "LITH_TXT_LOG_" #name); break;
#define FULL(name) case msg_##name: p->logF("%LS", "LITH_TXT_LOG_" #name); break;
#define HUDS(name) case msg_##name: p->logH("%LS", "LITH_TXT_LOG_" #name); break;
#define AMMO(name) case msg_##name: if(p->getCVarI("lith_player_ammolog")) p->logH("%LS", "LITH_TXT_LOG_" #name); break;
   BOTH(allmap)
   HUDS(armorbonus)
   BOTH(backpack)
   BOTH(berserk)
   BOTH(bluearmor)
   BOTH(blursphere)
   BOTH(greenarmor)
   HUDS(healthbonus)
   BOTH(infrared)
   BOTH(invuln)
   BOTH(medikit)
   BOTH(megasphere)
   BOTH(radsuit)
   BOTH(soulsphere)
   BOTH(stimpack)
   BOTH(redcard)
   BOTH(bluecard)
   BOTH(yellowcard)
   BOTH(redskull)
   BOTH(blueskull)
   BOTH(yellowskull)
   BOTH(doggosphere)
   BOTH(dogs)
   AMMO(clip)
   AMMO(clipbox)
   AMMO(shell)
   AMMO(shellbox)
   AMMO(rocket)
   AMMO(rocketbox)
   AMMO(cell)
   AMMO(cellbox)
#undef BOTH
#undef FULL
#undef HUDS
#undef AMMO
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
   logdata_t *logdata = salloc(logdata_t);
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
// Lith_PlayerLogEntry
//
void Lith_PlayerLogEntry(player_t *p)
{
   logmap_t *logmap = salloc(logmap_t);
   logmap->link.construct(logmap);

   logmap->levelnum = world.mapnum;
   logmap->name = StrParam("%tS", PRINTNAME_LEVELNAME); // :|

   logmap->link.link(&p->loginfo.maps);

   p->logF("Entered %S at %S NE", logmap->name, world.canontime);
}

//
// Lith_CBITab_Log
//
void Lith_CBITab_Log(gui_state_t *g, player_t *p)
{
   size_t num = 0;
   int i = 0;

   logmap_t *selmap;
   list_t *sel = CBIState(g)->logsel;

   if(!sel) sel = p->loginfo.maps.next;

   if(Lith_GUI_Button(g, .x = 25, 38, Pre(btnprev)))
      if((sel = sel->prev) == &p->loginfo.maps)
         sel = sel->prev;

   if(Lith_GUI_Button(g, .x = 25 + guipre.btnprev.w, 38, Pre(btnnext)))
      if((sel = sel->next) == &p->loginfo.maps)
         sel = sel->next;

   CBIState(g)->logsel = sel;
   selmap = sel->object;

   HudMessageF("CBIFONT", "%S", selmap->name);
   HudMessagePlain(g->hid--, 28.1 + guipre.btnprev.w + guipre.btnnext.w, 40.1, TICSECOND);

   forlist(logdata_t *logdata, p->loginfo.full)
      num += (logdata->from == selmap->levelnum);

   Lith_GUI_ScrollBegin(g, &CBIState(g)->logscr, 15, 50, 280, 175, num * 8);

   forlist(logdata_t *logdata, p->loginfo.full)
   {
      if(logdata->from != selmap->levelnum)
         continue;

      int y = 8 * i++;

      if(Lith_GUI_ScrollOcclude(g, &CBIState(g)->logscr, y, 8))
         continue;

      DrawSpritePlain("lgfx/UI/LogList.png", g->hid--, g->ox + 0.1, y + g->oy + 0.1, TICSECOND);

      HudMessageF("CBIFONT", "%S", logdata->info);
      HudMessageParams(0, g->hid--, CR_GREEN, g->ox + 2.1, y + g->oy + 1.1, TICSECOND);
   }


   Lith_GUI_ScrollEnd(g, &CBIState(g)->logscr);
}

// EOF

