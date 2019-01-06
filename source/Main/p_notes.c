// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// p_notes.c: Saved notes UI.

#include "common.h"
#include "p_player.h"
#include "w_world.h"
#include "gui.h"

void Lith_CBITab_Notes(struct gui_state *g, struct player *p)
{
   struct gui_txt *st = Lith_GUI_TextBox(g, &CBIState(g)->notebox, 48, 40, p);

   PrintText_str(L(st_edit), s_cbifont, CR_WHITE, 32,2, 40,0);
   Lith_GUI_BasicCheckbox(g, &CBIState(g)->noteedit, 39, 40);

   if(Lith_GUI_Button(g, LC(LANG "CLEAR"), 16, 48, Pre(btnclear)))
      Lith_GUI_TextBox_Reset(st);

   Lith_GUI_ScrollBegin(g, &CBIState(g)->notescr, 15, 63, 280, 160, 30 * countof(p->notes), 240);

   for(i32 i = 0; i < countof(p->notes); i++)
   {
      if(Lith_GUI_ScrollOcclude(g, &CBIState(g)->notescr, i * 30, 30))
         continue;

      PrintTextFmt(LC(LANG "NOTE_FMT"), i + 1);
      PrintText(s_cbifont, CR_WHITE, g->ox+2,1, i * 30 + g->oy,1);

      if(Lith_GUI_Button_Id(g, i, p->notes[i] ? p->notes[i] : LC(LANG "EMPTY"),
         44, i * 30, .disabled = !CBIState(g)->noteedit, Pre(btnnote)))
      {
         i32 l = CBIState(g)->notebox.tbptr;
         char const *s = Cps_Print(CBIState(g)->notebox.txtbuf, l);

         Dalloc(p->notes[i]);
         p->notes[i] = Nalloc(l + 1);
         memmove(p->notes[i], s, l);

         p->saveData();
      }
   }

   Lith_GUI_ScrollEnd(g, &CBIState(g)->notescr);
}

// EOF
