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

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"
#include "gui.h"

void P_CBI_TabNotes(struct gui_state *g) {
   struct gui_txt *st = &pl.cbi.st.notebox;
   G_TxtBox(g, st, 35, 27);
   if(G_Button(g, tmpstr(lang(sl_clear)), 3, 27, Pre(btnclear))) {
      G_TxtBoxRes(st);
   }
   G_ScrBeg(g, &pl.cbi.st.notescr, 2, 50, 280, 160, 30 * countof(pl.notes), 240);
   for(i32 i = 0; i < countof(pl.notes); i++) {
      if(G_ScrOcc(g, &pl.cbi.st.notescr, i * 30, 30)) {
         continue;
      }
      static struct fmt_arg args[] = {{_fmt_i32}};
      args[0].val.i = i + 1;
      ACS_BeginPrint();
      printfmt(tmpstr(lang(sl_note_fmt)), countof(args), args);
      PrintText(sf_lmidfont, CR_DARKGRAY, g->ox+2,1, i * 30 + g->oy,1);
      PrintText_str(pl.notes[i] ? fast_strdup(pl.notes[i]) : lang(sl_empty), sf_smallfnt, g->defcr, g->ox+44,1, i * 30 + g->oy,1);
      static struct gui_fil fil_edit = {20};
      if(G_Button_HId(g, i, tmpstr(lang(sl_edit)), 2, i * 30 + 8,
                      .fill = &fil_edit, Pre(btnclear)))
      {
         mem_size_t l = pl.cbi.st.notebox.tbptr;
         cstr       s = Cps_Expand(pl.cbi.st.notebox.txtbuf, 0, l);
         if(l) {
            pl.notes[i] = Talloc(pl.notes[i], l + 1, _tag_plyr);
            fastmemmove(pl.notes[i], s, l);
         } else {
            Dalloc(pl.notes[i]);
            pl.notes[i] = nil;
         }
         P_Data_Save();
      }
      static struct gui_fil fil_clear = {70};
      if(G_Button_HId(g, i, tmpstr(lang(sl_clear)), 2, i * 30 + 16,
                      .fill = &fil_clear, Pre(btnclear))) {
         Dalloc(pl.notes[i]);
         pl.notes[i] = nil;
         P_Data_Save();
      }
   }
   G_ScrEnd(g, &pl.cbi.st.notescr);
}

/* EOF */
