// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"
#include "lith_gui.h"

//
// Lith_CBITab_Notes
//
void Lith_CBITab_Notes(gui_state_t *g, struct player *p)
{
   gui_txtbox_state_t *st = Lith_GUI_TextBox(g, &CBIState(g)->notebox, 48, 40, p->num, p->txtbuf);
   p->clearTextBuf();

   PrintTextStr(L("LITH_EDIT"));
   PrintText("CBIFONT", CR_WHITE, 32,2, 40,0);
   Lith_GUI_BasicCheckbox(g, &CBIState(g)->noteedit, 39, 40);

   if(Lith_GUI_Button(g, L("LITH_CLEAR"), 16, 48, Pre(btnclear)))
      Lith_GUI_TextBox_Reset(st);

   Lith_GUI_ScrollBegin(g, &CBIState(g)->notescr, 15, 63, 280, 160, 30 * countof(p->notes), 240);

   for(int i = 0; i < countof(p->notes); i++)
   {
      if(Lith_GUI_ScrollOcclude(g, &CBIState(g)->notescr, i * 30, 30))
         continue;

      PrintTextFmt(L("LITH_NOTE_FMT"), i + 1);
      PrintText("CBIFONT", CR_WHITE, 32+g->ox,2, i * 30 + g->oy,1);

      if(Lith_GUI_Button_Id(g, i, p->notes[i] == "" ? L("LITH_EMPTY") : p->notes[i],
         37, i * 30, .disabled = !CBIState(g)->noteedit, Pre(btnnote)))
      {
         p->notes[i] = Lith_CPS_Print(CBIState(g)->notebox.txtbuf);
         p->saveData();
      }
   }

   Lith_GUI_ScrollEnd(g, &CBIState(g)->notescr);
}

// EOF
