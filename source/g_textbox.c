/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * GUI text boxes.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "w_world.h"
#include "m_cps.h"
#include "m_char.h"

bool G_TxtBox_Imp(struct gui_state *g, gid_t id, struct gui_arg_txt const *a) {
   struct gui_txt *st = a->st;

   G_Auto(g, id, a->x, a->y, 260, 10);

   bool hot = g->hot == id;
   bool ret = false;

   if(hot && g->clicklft && !g->old.clicklft) {
      GrabInput(Cps_Expand_str(st->txtbuf, 0, st->tbptr));
   }

   if(pl.tb.txtbuf[0]) {
      fastmemcpy(st,     &pl.tb, sizeof pl.tb);
      fastmemset(&pl.tb, 0,      sizeof pl.tb);
      Cps_SetC(st->txtbuf, st->tbptr, '\0');
      ret = true;
   }

   PrintSprite(sp_UI_ResultFrame, a->x-3 + g->ox,1, a->y-3 + g->oy,1);

   SetClipW(a->x + g->ox, a->y + g->oy, 260, 200, 260);
   if(st->tbptr) {
      PrintTextFmt("%s%s", Cps_Expand(st->txtbuf, 0, st->tbptr),
                           hot ? Ticker("|", "") : "");
   } else {
      PrintTextFmt("\C%c%S", hot ? 'c' : 'm', ns(lang(sl_gui_textbox)));
   }
   PrintText(sf_smallfnt, g->defcr, a->x + g->ox,1, a->y + g->oy,1);
   ClearClip();

   return ret;
}

/* EOF */
