#ifndef LITH_GAMES_H
#define LITH_GAMES_H

#include "lith_gui.h"


//----------------------------------------------------------------------------
// Type Definitions
//

enum
{
   GB_GAME_PONG,
   GB_GAME_ARKANOID,
   GB_GAME_SPACE_INVADERS,
   GB_GAME_TETRIS,
   GB_GAME_PUYOPUYO,
   GB_GAME_SOLITAIRE,
   GB_GAME_MINESWEEPER,
   GB_GAME_TRIPEAKS,
   GB_GAME_DOOMGUYS_CHALLENGE,
   GB_GAME_MAX
};

typedef struct gb_gameinfo_s
{
   __str name;
   void (*update)(void);
} gb_gameinfo_t;

typedef struct gb_s
{
   gui_state_t guistate;
   gb_gameinfo_t const *gameinfo;
} gb_t;


//----------------------------------------------------------------------------
// Extern Functions
//

[[__call("ScriptS")]] void Lith_PlayerUpdateGB(struct player *p);

#endif

