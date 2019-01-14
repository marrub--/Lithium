// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// p_notes.c: Saved notes UI.

#include "common.h"
#include "p_player.h"
#include "w_world.h"
#include "gui.h"

void P_CBI_TabNotes(struct gui_state *g, struct player *p)
{
   struct gui_txt *st = G_TextBox(g, &CBIState(g)->notebox, 48, 40, p);

   PrintText_str(L(st_edit), s_cbifont, CR_WHITE, 32,2, 40,0);
   G_BasicCheckbox(g, &CBIState(g)->noteedit, 39, 40);

   if(G_Button(g, LC(LANG "CLEAR"), 16, 48, Pre(btnclear)))
      G_TextBox_Reset(st);

   G_ScrollBegin(g, &CBIState(g)->notescr, 15, 63, 280, 160, 30 * countof(p->notes), 240);

   for(i32 i = 0; i < countof(p->notes); i++)
   {
      if(G_ScrollOcclude(g, &CBIState(g)->notescr, i * 30, 30))
         continue;

      PrintTextFmt(LC(LANG "NOTE_FMT"), i + 1);
      PrintText(s_cbifont, CR_WHITE, g->ox+2,1, i * 30 + g->oy,1);

      if(G_Button_Id(g, i, p->notes[i] ? p->notes[i] : LC(LANG "EMPTY"),
         44, i * 30, .disabled = !CBIState(g)->noteedit, Pre(btnnote)))
      {
         i32 l = CBIState(g)->notebox.tbptr;
         char const *s = Cps_Print(CBIState(g)->notebox.txtbuf, l);

         Dalloc(p->notes[i]);
         p->notes[i] = Nalloc(l + 1);
         memmove(p->notes[i], s, l);

         P_Data_Save(p);
      }
   }

   G_ScrollEnd(g, &CBIState(g)->notescr);
}

// EOF
