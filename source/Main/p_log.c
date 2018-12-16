// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_log.h"
#include "lith_player.h"
#include "lith_list.h"
#include "lith_hudid.h"
#include "lith_world.h"

StrEntON

// Static Functions ----------------------------------------------------------|

static __str LogV(int levl, __str fmt, va_list vl)
{
   ACS_BeginPrint();

   if(levl)
   {
      for(int i = 0; i < levl; i++) ACS_PrintChar('>');
      ACS_PrintChar(' ');
   }

   __vnprintf_str(fmt, vl);

   return ACS_EndStrParam();
}

static void LogPop(struct player *p)
{
   p->log.hudC--;
   memmove(&p->log.hudV[0], &p->log.hudV[1], sizeof p->log.hudV[0] * p->log.hudC);
}

static void LogH(struct player *p, struct logdat *ld)
{
   ld->ftim = 5;
   ld->time = 140 - p->log.curtime;
   p->log.curtime = 140;

   if(p->log.hudC >= countof(p->log.hudV)) LogPop(p);
   p->log.hudV[p->log.hudC++] = *ld;
}

static void LogF(struct player *p, struct logfdt *lf)
{
   Vec_GrowN(p->log.curmap->data, 1, 8);
   Vec_Next(p->log.curmap->data) = *lf;
}

// Extern Functions ----------------------------------------------------------|

void Lith_LogB(struct player *p, int levl, __str fmt, ...)
{
   struct logdat ld = {};

   va_list vl;
   va_start(vl, fmt);
   ld.info = LogV(levl, fmt, vl);
   va_end(vl);

   LogF(p, &ld.fdta);
   LogH(p, &ld);
}

void Lith_LogH(struct player *p, int levl, __str fmt, ...)
{
   struct logdat ld = {};

   va_list vl;
   va_start(vl, fmt);
   ld.info = LogV(levl, fmt, vl);
   va_end(vl);

   LogH(p, &ld);
}

void Lith_LogF(struct player *p, __str fmt, ...)
{
   struct logfdt lf = {};

   va_list vl;
   va_start(vl, fmt);
   lf.info = LogV(0, fmt, vl);
   va_end(vl);

   LogF(p, &lf);
}

void Lith_PlayerLogEntry(struct player *p)
{
   struct logmap *lm = null;
   int lnum = world.mapnum;

   for(int i = 0; i < p->log.mapsC; i++)
      if(p->log.mapsV[i].lnum == lnum)
         {lm = &p->log.mapsV[i]; break;}

   if(!lm)
   {
      Vec_GrowN(p->log.maps, 1, 32);
      lm = &Vec_Next(p->log.maps);
      *lm = (struct logmap){
         .name = StrParam("%tS", PRINTNAME_LEVELNAME),
         .lnum = lnum
      };
   }

   p->log.curmap = lm;

   p->logF(L(LANG "ENTER_FMT"), lm->name, world.canontime);
}

script ext("ACS")
void Lith_LogS(int levl, int type)
{
   __str name = ServCallS("GetLogName");

   if(name[0] == '_') name = Language(LANG "LOG%S", name);

   withplayer(LocalPlayer) switch(type) {
   case msg_ammo: if(p->getCVarI(CVAR "player_ammolog"))
   case msg_huds: p->logH(levl, "%S", name); break;
   case msg_full: p->logF(      "%S", name); break;
   case msg_both: p->logB(levl, "%S", name); break;
   }
}

script
void Lith_PlayerUpdateLog(struct player *p)
{
   if(p->log.curtime == 0)
   {
      if(p->log.hudC)
      {
         LogPop(p);
         p->log.curtime = p->log.hudV[p->log.hudC - 1].time;
      }
   }
   else
      p->log.curtime--;

   for(int i = 0; i < p->log.hudC; i++)
      if(p->log.hudV[i].ftim) p->log.hudV[i].ftim--;
}

void Lith_CBITab_Log(gui_state_t *g, struct player *p)
{
   if(Lith_GUI_Button(g, .x = 25, 38, Pre(btnprev)))
      if(--CBIState(g)->logsel < 0) CBIState(g)->logsel = p->log.mapsC - 1;

   if(Lith_GUI_Button(g, .x = 25 + guipre.btnprev.w, 38, Pre(btnnext)))
      if(++CBIState(g)->logsel >= p->log.mapsC) CBIState(g)->logsel = 0;

   struct logmap *lm = &p->log.mapsV[CBIState(g)->logsel];

   PrintTextStr(lm->name);
   PrintText("cbifont", CR_WHITE, 28+guipre.btnprev.w+guipre.btnnext.w,1, 40,1);

   Lith_GUI_ScrollBegin(g, &CBIState(g)->logscr, 15, 50, 280, 175, lm->dataC * 8);

   for(int i = 0; i < lm->dataC; i++)
   {
      int const y = 8 * i;

      if(Lith_GUI_ScrollOcclude(g, &CBIState(g)->logscr, y, 8)) continue;

      PrintSprite(":UI:LogList", g->ox,1, y + g->oy,1);

      PrintTextStr(lm->dataV[i].info);
      PrintText("cbifont", CR_GREEN, g->ox + 2,1, y + g->oy + 1,1);
   }


   Lith_GUI_ScrollEnd(g, &CBIState(g)->logscr);
}

script
void Lith_HUD_Log(struct player *p, int cr, int x, int yy)
{
   if(p->getCVarI(CVAR "hud_showlog"))
   {
      int yo;

      if(p->getCVarI(CVAR "hud_logbig")) {yo = 200; SetSize(320, 240);}
      else                               {yo = 255; SetSize(480, 300);}

      int i = 0;
      for(int i = 0; i < p->log.hudC; i++)
      {
         struct logdat const *const ld = &p->log.hudV[i];

         int y = 10 * i;
         int ya;

         if(p->getCVarI(CVAR "hud_logfromtop"))
            {ya = 1; y = 20 + y;}
         else
            {ya = 2; y = (yo - y) + yy;}

         PrintTextStr(ld->info);
         PrintText("logfont", cr, x,1, y,ya);

         if(ld->ftim) SetFade(fid_logadS + i, 1, 0.07);

         if(CheckFade(fid_logadS + i))
         {
            PrintTextStr(ld->info);
            PrintTextF("logfont", CR_WHITE, x,1, y,ya, fid_logadS + i);
         }
      }

      SetSize(320, 240);
   }
}

// EOF
