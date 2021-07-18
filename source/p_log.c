/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Log UI and logging functions.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_log.h"
#include "p_player.h"
#include "m_list.h"
#include "p_hudid.h"
#include "w_world.h"

/* Static Functions -------------------------------------------------------- */

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
   pl.log.hudC--;
   fastmemmove(&pl.log.hudV[0], &pl.log.hudV[1], sizeof pl.log.hudV[0] * pl.log.hudC);
}

static
void LogH(struct logdat *ld) {
   ld->ftim = 5;
   ld->time = 140 - pl.log.curtime;
   pl.log.curtime = 140;

   if(pl.log.hudC >= countof(pl.log.hudV)) LogPop();
   pl.log.hudV[pl.log.hudC++] = *ld;
}

static
void LogF(struct logfdt *lf) {
   Vec_GrowN(pl.log.curmap->data, 1, 8, _tag_logs);
   Vec_Next(pl.log.curmap->data) = *lf;
}

/* Extern Functions -------------------------------------------------------- */

void P_Log_Both(i32 levl, cstr fmt, ...) {
   struct logdat ld = {};

   LogV(levl);

   va_list vl;
   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   ld.inf = ACS_EndStrParam();

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

   ld.inf = ACS_EndStrParam();

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

   for(i32 i = 0; i < pl.log.mapsC; i++)
      if(pl.log.mapsV[i].lnum == lnum) {lm = &pl.log.mapsV[i]; break;}

   if(!lm) {
      Vec_GrowN(pl.log.maps, 1, 32, _tag_logs);
      lm = &Vec_Next(pl.log.maps);
      lm->name = (ACS_BeginPrint(), ACS_PrintName(PRINTNAME_LEVELNAME), ACS_EndStrParam());
      lm->lnum = lnum;
   }

   pl.log.curmap = lm;

   pl.logF(tmpstr(lang(sl_enter_fmt)), lm->name, CanonTime(ct_full, ticks));
}

script void P_Log_PTick() {
   if(pl.log.curtime == 0) {
      if(pl.log.hudC) {
         LogPop();
         pl.log.curtime = pl.log.hudV[pl.log.hudC - 1].time;
      }
   } else {
      pl.log.curtime--;
   }

   for(i32 i = 0; i < pl.log.hudC; i++) {
      if(pl.log.hudV[i].ftim) {
         pl.log.hudV[i].ftim--;
      }
   }
}

void P_CBI_TabLog(struct gui_state *g) {
   static
   i32 const ht = 10;

   if(G_Button(g, .x = 12, 25, Pre(btnprev)))
      if(--CBIState(g)->logsel < 0) CBIState(g)->logsel = pl.log.mapsC - 1;

   if(G_Button(g, .x = 12 + gui_p.btnprev.w, 25, Pre(btnnext)))
      if(++CBIState(g)->logsel >= pl.log.mapsC) CBIState(g)->logsel = 0;

   struct logmap *lm = &pl.log.mapsV[CBIState(g)->logsel];

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

script void HUD_Log(i32 cr, i32 x, i32 yy) {
   if(CVarGetI(sc_hud_showlog)) {
      k32 scale = CVarGetK(sc_hud_logsize);
      i32 yo = 200 / scale;
      i32 xs = 320 / scale;
      i32 ys = 240 / scale;

      SetSize(xs, ys);
      SetClipW(0, 0, xs, ys, xs);

      i32 i = 0;
      for(i32 i = 0; i < pl.log.hudC; i++) {
         struct logdat const *const ld = &pl.log.hudV[i];

         i32 y = 10 * i;
         i32 ya;

         if(CVarGetI(sc_hud_logfromtop)) {ya = 1; y = 20 + y;}
         else                            {ya = 2; y = (yo - y) + yy;}

         PrintText_str(ld->inf, sf_lmidfont, cr, x,1, y,ya);

         if(ld->ftim) SetFade(fid_logadS + i, 1, 8);

         if(CheckFade(fid_logadS + i)) {
            cstr s = RemoveTextColors_str(ld->inf, ACS_StrLen(ld->inf));
            PrintTextChS(s);
            PrintTextFX(sf_lmidfont, CR_WHITE, x,1, y,ya, fid_logadS + i, _u_add);
         }
      }

      SetSize(320, 240);
   }
}

/* Scripts ----------------------------------------------------------------- */

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
