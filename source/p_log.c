// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Log UI and logging functions.                                            │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_log.h"
#include "p_player.h"
#include "m_list.h"
#include "p_hudid.h"
#include "w_world.h"

struct logfdt
{
   str inf;
};

struct logmap
{
   str name;
   i32 lnum;

   Vec_Decl(struct logfdt, data);
};

struct logdat
{
   struct logfdt fdta;
   i32           time;
   i32           ftim;
};

struct loginfo
{
   struct logdat hudV[7];
   mem_size_t    hudC;

   Vec_Decl(struct logmap, maps);

   struct logmap *curmap;
   i32            curtime;
};

static
struct loginfo log;

static
void LogV(i32 levl) {
   ACS_BeginPrint();

   if(levl) {
      for(i32 i = 0; i < levl; i++) ACS_PrintChar('>');
      ACS_PrintChar(' ');
   }
}

static
void LogPop() {
   log.hudC--;
   fastmemmove(&log.hudV[0], &log.hudV[1], sizeof log.hudV[0] * log.hudC);
}

static
void LogH(struct logdat *ld) {
   ld->ftim = 5;
   ld->time = 140 - log.curtime;
   log.curtime = 140;

   if(log.hudC >= countof(log.hudV)) LogPop();
   log.hudV[log.hudC++] = *ld;
}

static
void LogF(struct logfdt *lf) {
   Vec_GrowN(log.curmap->data, 1, 8, _tag_logs);
   Vec_Next(log.curmap->data) = *lf;
}

void P_Log_Both(i32 levl, cstr fmt, ...) {
   struct logdat ld = {};

   LogV(levl);

   va_list vl;
   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   ld.fdta.inf = ACS_EndStrParam();

   LogF(&ld.fdta);
   LogH(&ld);
}

void P_Log_HUDs(i32 levl, cstr fmt, ...) {
   struct logdat ld = {};

   LogV(levl);

   va_list vl;
   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   ld.fdta.inf = ACS_EndStrParam();

   LogH(&ld);
}

void P_Log_Full(cstr fmt, ...) {
   struct logfdt lf = {};

   ACS_BeginPrint();

   va_list vl;
   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   lf.inf = ACS_EndStrParam();

   LogF(&lf);
}

void P_Log_Entry() {
   struct logmap *lm = nil;
   i32 lnum = MapNum;

   for(i32 i = 0; i < log.mapsC; i++)
      if(log.mapsV[i].lnum == lnum) {lm = &log.mapsV[i]; break;}

   if(!lm) {
      Vec_GrowN(log.maps, 1, 32, _tag_logs);
      lm = &Vec_Next(log.maps);
      lm->name = (ACS_BeginPrint(), ACS_PrintName(PRINTNAME_LEVELNAME), ACS_EndStrParam());
      lm->lnum = lnum;
   }

   log.curmap = lm;

   pl.logF(tmpstr(lang(sl_enter_fmt)), lm->name, CanonTime(ct_full, ticks));
}

script void P_Log_PTick() {
   if(log.curtime == 0) {
      if(log.hudC) {
         LogPop();
         log.curtime = log.hudV[log.hudC - 1].time;
      }
   } else {
      log.curtime--;
   }

   for(i32 i = 0; i < log.hudC; i++) {
      log.hudV[i].ftim--;
   }
}

void P_CBI_TabLog(struct gui_state *g) {
   static
   i32 const ht = 10;

   if(G_Button(g, .x = 12, 25, Pre(btnprev)))
      if(--CBIState(g)->logsel < 0) CBIState(g)->logsel = log.mapsC - 1;

   if(G_Button(g, .x = 12 + gui_p.btnprev.w, 25, Pre(btnnext)))
      if(++CBIState(g)->logsel >= log.mapsC) CBIState(g)->logsel = 0;

   struct logmap *lm = &log.mapsV[CBIState(g)->logsel];

   PrintText_str(lm->name, sf_lmidfont, g->defcr, g->ox+15+gui_p.btnprev.w+gui_p.btnnext.w,1, g->oy+27,1);

   G_ScrBeg(g, &CBIState(g)->logscr, 2, 37, 280, 175, lm->dataC * ht);

   for(i32 i = 0; i < lm->dataC; i++) {
      i32 const y = ht * i;

      if(G_ScrOcc(g, &CBIState(g)->logscr, y, ht)) continue;

      PrintSprite(sp_UI_LogList, g->ox,1, y + g->oy,1);
      PrintText_str(lm->dataV[i].inf, sf_smallfnt, CR_GREEN, g->ox + 2,1, y + g->oy + 1,1);
   }

   G_ScrEnd(g, &CBIState(g)->logscr);
}

script void P_Log(i32 cr, i32 x, i32 yy) {
   if(CVarGetI(sc_hud_showlog)) {
      k32 scale = CVarGetK(sc_hud_logsize);
      i32 yo = 200 / scale;
      i32 xs = 320 / scale;
      i32 ys = 240 / scale;

      SetSize(xs, ys);
      SetClipW(0, 0, xs, ys, xs);

      i32 i = 0;
      for(i32 i = 0; i < log.hudC; i++) {
         struct logdat const *const ld = &log.hudV[i];

         i32 y = 10 * i;
         i32 ya;

         if(CVarGetI(sc_hud_logfromtop)) {ya = 1; y = 64 + y;}
         else                            {ya = 2; y = (yo - y) + yy;}

         k32 a = 1.0;
         if(ld->ftim > 0) {
            SetFade(fid_logadS + i, 1, 8);
         } else if(ld->ftim < -129) {
            a = (ld->ftim + 129) / 10.0 + 1.0;
         }

         PrintTextA_str(ld->fdta.inf, sf_lmidfont, cr, x,1, y,ya, a);

         if(CheckFade(fid_logadS + i)) {
            cstr s = RemoveTextColors_str(ld->fdta.inf, ACS_StrLen(ld->fdta.inf));
            PrintTextChS(s);
            PrintTextFX(sf_lmidfont, CR_WHITE, x,1, y,ya, fid_logadS + i, _u_add);
         }
      }

      SetSize(320, 240);
   }
}

script_str ext("ACS") addr(OBJ "LogS")
void Sc_Log(i32 levl, i32 type) {
   if(P_None()) return;

   str name = ServCallS(sm_GetLogName);

   if(name[0] == '_') name = lang_fmt(LANG "LOG%S", name);

   switch(type) {
   case msg_scri:
      if(CVarGetI(sc_player_itemdisp) & _itm_disp_log) {
      case msg_huds:
         pl.logH(levl, tmpstr(name));
      }
      break;
   case msg_full:
      pl.logF(tmpstr(name));
      break;
   case msg_item:
      if(CVarGetI(sc_player_itemdisp) & _itm_disp_log)
         pl.logB(levl, tmpstr(name));
      break;
   }
}

/* EOF */
