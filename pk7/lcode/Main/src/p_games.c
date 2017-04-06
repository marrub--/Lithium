#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"

void Lith_GB_Pong_Update(void)
{
}

static gb_gameinfo_t const gbgameinfo[GB_GAME_MAX] = {
// {"Name"},
   {"Pong", Lith_GB_Pong_Update},
   {"Arkanoid"},
   {"Tetris"},
   {"PuyoPuyo"},
   {"Solitaire"},
   {"Minesweeper"},
};

[[__call("ScriptS"), __extern("ACS")]]
void Lith_UseGameboy(void)
{
   player_t *p = Lith_LocalPlayer;
   
   if(p->dead)
      return;
   
   p->useGUI(GUI_GB);
}

[[__call("ScriptS")]]
void Lith_PlayerUpdateGB(player_t *p)
{
   gb_t *gb = &p->gb;
   gui_state_t *g = &gb->guistate;
   
   Lith_GUI_Begin(g, hid_end_gb);
   Lith_GUI_UpdateState(g, p);
   
   if(!gb->gameinfo)
      for(int i = 0; i < GB_GAME_MAX; i++)
         if(Lith_GUI_Button_Id(g, i, gbgameinfo[i].name, 40, (btnlist.h * i) + 50, .preset = &btnlist))
            gb->gameinfo = &gbgameinfo[i];
   else
      gb->gameinfo->update();
   
   Lith_GUI_End(g);
}

// EOF

