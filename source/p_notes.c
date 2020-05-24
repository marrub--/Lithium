/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Saved notes UI.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "w_world.h"
#include "gui.h"

void P_CBI_TabNotes(struct gui_state *g, struct player *p)
{
   Str(edit, sLANG "EDIT");

   struct gui_txt *st = G_TxtBox(g, &CBIState(g)->notebox, 48, 40, p);

   PrintText_str(L(edit), s_smallfnt, g->defcr, 32,2, 41,0);
   if(G_ChkBox(g, CBIState(g)->noteedit, 34, 37))
      CBIState(g)->noteedit = !CBIState(g)->noteedit;

   if(G_Button(g, LC(LANG "CLEAR"), 16, 50, Pre(btnclear)))
      G_TxtBoxRes(st);

   G_ScrBeg(g, &CBIState(g)->notescr, 15, 63, 280, 160, 30 * countof(p->notes), 240);

   for(i32 i = 0; i < countof(p->notes); i++)
   {
      if(G_ScrOcc(g, &CBIState(g)->notescr, i * 30, 30))
         continue;

      PrintTextFmt(LC(LANG "NOTE_FMT"), i + 1);
      PrintText(s_lmidfont, g->defcr, g->ox+2,1, i * 30 + g->oy,1);

      if(G_Button_HId(g, i, p->notes[i] ? p->notes[i] : LC(LANG "EMPTY"),
                      44, i * 30, .disabled = !CBIState(g)->noteedit,
                      Pre(btnnote)))
      {
         i32  l = CBIState(g)->notebox.tbptr;
         cstr s = Cps_Expand(CBIState(g)->notebox.txtbuf, 0, l);

         Dalloc(p->notes[i]);
         p->notes[i] = Malloc(l + 1, _tag_plyr);
         fastmemmove(p->notes[i], s, l);

         P_Data_Save(p);
      }
   }

   G_ScrEnd(g, &CBIState(g)->notescr);
}

/* EOF */
