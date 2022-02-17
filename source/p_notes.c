// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Saved notes UI.                                                          │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "w_world.h"
#include "gui.h"

void P_CBI_TabNotes(struct gui_state *g)
{
   struct gui_txt *st = &CBIState(g)->notebox;
   G_TxtBox(g, st, 35, 27);

   if(G_Button(g, tmpstr(lang(sl_clear)), 3, 37, Pre(btnclear)))
      G_TxtBoxRes(st);

   G_ScrBeg(g, &CBIState(g)->notescr, 2, 50, 280, 160, 30 * countof(pl.notes), 240);

   for(i32 i = 0; i < countof(pl.notes); i++)
   {
      if(G_ScrOcc(g, &CBIState(g)->notescr, i * 30, 30))
         continue;

      PrintTextFmt(tmpstr(lang(sl_note_fmt)), i + 1);
      PrintText(sf_lmidfont, g->defcr, g->ox+2,1, i * 30 + g->oy,1);

      if(G_Button_HId(g, i, pl.notes[i] |? tmpstr(lang(sl_empty)),
                      44, i * 30, Pre(btnnote)))
      {
         mem_size_t l = CBIState(g)->notebox.tbptr;
         cstr       s = Cps_Expand(CBIState(g)->notebox.txtbuf, 0, l);

         if(l) {
            pl.notes[i] = Ralloc(pl.notes[i], l + 1, _tag_plyr);
            fastmemmove(pl.notes[i], s, l);
         } else {
            Dalloc(pl.notes[i]);
            pl.notes[i] = nil;
         }

         P_Data_Save();
      }
   }

   G_ScrEnd(g, &CBIState(g)->notescr);
}

/* EOF */
