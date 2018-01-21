// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"
#include "lith_gui.h"

//
// Lith_CBITab_Notes
//
void Lith_CBITab_Notes(gui_state_t *g, player_t *p)
{
   gui_txtbox_state_t *st = Lith_GUI_TextBox(g, &CBIState(g)->notebox, 48, 40, p->num, p->txtbuf);
   p->clearTextBuf();

   HudMessageF("CBIFONT", "Edit");
   HudMessagePlain(g->hid--, 32.2, 40.0, TS);
   Lith_GUI_BasicCheckbox(g, &CBIState(g)->noteedit, 39, 40);

   if(Lith_GUI_Button(g, "Clear", 16, 48, .preset = &guipre.btnclear))
      Lith_GUI_TextBox_Reset(st);

   Lith_GUI_ScrollBegin(g, &CBIState(g)->notescr, 15, 63, 280, 160, 30 * countof(p->notes), 240);

   for(int i = 0; i < countof(p->notes); i++)
   {
      if(Lith_GUI_ScrollOcclude(g, &CBIState(g)->notescr, i * 30, 30))
         continue;

      HudMessageF("CBIFONT", "\CmNote %i", i + 1);
      HudMessagePlain(g->hid--, 32.2 + g->ox, 0.1 + i * 30 + g->oy, TS);

      if(Lith_GUI_Button_Id(g, i, p->notes[i] == "" ? "<empty>" : p->notes[i],
         37, i * 30, .disabled = !CBIState(g)->noteedit,
         .preset = &guipre.btnnote))
      {
         p->notes[i] = Lith_CPS_Print(CBIState(g)->notebox.txtbuf);
         p->saveData();
      }
   }

   Lith_GUI_ScrollEnd(g, &CBIState(g)->notescr);
}

// EOF
