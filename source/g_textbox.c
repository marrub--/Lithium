// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ GUI text boxes.                                                          │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"
#include "m_cps.h"

bool G_TxtBox_Imp(struct gui_state *g, gid_t id, struct gui_arg_txt *a) {
   struct gui_txt *st = a->st;

   G_Auto(g, id, a->x, a->y, 260, 10);

   bool hot = g->hot == id;

   if(hot && g->clicklft && !g->old.clicklft) {
      ServCallV(sm_GrabInput, Cps_Expand_str(st->txtbuf, 0, st->tbptr));
   }

   if(pl.tb.changed) {
      fastmemcpy(st,     &pl.tb, sizeof(struct gui_txt));
      fastmemset(&pl.tb, 0,      sizeof(struct gui_txt));
      Cps_SetC(st->txtbuf, st->tbptr, '\0');
   } else {
      st->changed = false;
   }

   PrintSprite(sp_UI_ResultFrame, a->x-2 + g->ox,1, a->y-1 + g->oy,1);

   G_Clip(g, a->x, a->y, 130, 200, 130);
   ACS_BeginPrint();
   if(st->tbptr) {
      PrintStr(Cps_Expand(st->txtbuf, 0, st->tbptr));
      PrintStr(hot ? (ACS_Timer() & 31 < 15 ? "|" : "") : "");
   } else {
      ACS_PrintChar('\C');
      ACS_PrintChar(hot ? 'c' : 'm');
      ACS_PrintString(ns(lang(sl_gui_textbox)));
   }
   PrintText(sf_smallfnt, g->defcr, a->x + g->ox,1, a->y + g->oy,1);
   G_ClipRelease(g);

   return st->changed;
}

/* EOF */
