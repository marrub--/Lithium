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
// Lith_LogS
//
script ext("ACS")
void Lith_LogS(int type)
{
   __str name = HERMES_S("GetLogName");
   if(name[0] != '>') name = Language("LITH_TXT_LOG_%S", name);
   withplayer(LocalPlayer) switch(type)
   {
   case msg_ammo: if(!p->getCVarI("lith_player_ammolog")) break;
   case msg_huds: p->logH("%S", name); break;
   case msg_full: p->logF("%S", name); break;
   case msg_both: p->log ("%S", name); break;
   }
}

//
// Lith_Log
//
void Lith_Log(struct player *p, __str fmt, ...)
{
   va_list vl;

   va_start(vl, fmt);
   logdata_t *logdata = Lith_LogV(p, fmt, vl);
   va_end(vl);

   logdata->time = LOG_TIME;
   logdata->link    .link(&p->loginfo.hud);
   logdata->linkfull.link(&p->loginfo.full);
   logdata->keep = true;

   if(p->loginfo.hud.size > LOG_MAX)
      Dalloc(p->loginfo.hud.next->unlink());
}

//
// Lith_LogF
//
void Lith_LogF(struct player *p, __str fmt, ...)
{
   va_list vl;

   va_start(vl, fmt);
   logdata_t *logdata = Lith_LogV(p, fmt, vl);
   va_end(vl);

   logdata->linkfull.link(&p->loginfo.full);
   logdata->keep = true;
}

//
// Lith_LogH
//
void Lith_LogH(struct player *p, __str fmt, ...)
{
   va_list vl;

   va_start(vl, fmt);
   logdata_t *logdata = Lith_LogV(p, fmt, vl);
   va_end(vl);

   logdata->time = LOG_TIME;
   logdata->link.link(&p->loginfo.hud);

   if(p->loginfo.hud.size > LOG_MAX)
      Dalloc(p->loginfo.hud.next->unlink());
}

//
// Lith_LogV
//
logdata_t *Lith_LogV(struct player *p, __str fmt, va_list vl)
{
   logdata_t *logdata = Salloc(logdata_t);
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
script
void Lith_PlayerUpdateLog(struct player *p)
{
   forlist(logdata_t *logdata, p->loginfo.hud)
   {
      if(logdata->time == 0)
      {
         list_t *next = rover->next;
         rover->unlink();

         if(!logdata->keep) Dalloc(logdata);
      }
      else
         logdata->time--;
   }
}

//
// Lith_PlayerLogEntry
//
void Lith_PlayerLogEntry(struct player *p)
{
   logmap_t *logmap = Salloc(logmap_t);
   logmap->link.construct(logmap);

   logmap->levelnum = world.mapnum;
   logmap->name = StrParam("%tS", PRINTNAME_LEVELNAME); // :|

   logmap->link.link(&p->loginfo.maps);

   p->logF("Entered %S at %S NE", logmap->name, world.canontime);
}

//
// Lith_CBITab_Log
//
void Lith_CBITab_Log(gui_state_t *g, struct player *p)
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

   PrintTextStr(selmap->name);
   PrintText("CBIFONT", CR_WHITE, 28+guipre.btnprev.w+guipre.btnnext.w,1, 40,1);

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

      PrintSprite(":UI:LogList", g->ox,1, y + g->oy,1);

      PrintTextStr(logdata->info);
      PrintText("CBIFONT", CR_GREEN, g->ox + 2,1, y + g->oy + 1,1);
   }


   Lith_GUI_ScrollEnd(g, &CBIState(g)->logscr);
}

// EOF

