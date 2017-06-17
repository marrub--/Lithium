#include "lith_common.h"
#include "lith_player.h"
#include "lith_savedata.h"
#include "lith_file.h"

//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_PlayerSaveData
//
[[__call("ScriptS")]]
void Lith_PlayerSaveData(player_t *p)
{
   savefile_t *save;
   if((save = Lith_SaveBegin(p)))
   {
      Lith_SaveEnd(save);
   }
}

//
// Lith_PlayerLoadData
//
[[__call("ScriptS")]]
void Lith_PlayerLoadData(player_t *p)
{
   savefile_t *save;
   if((save = Lith_LoadBegin(p)))
   {
      Lith_LoadEnd(save);
   }
}

// EOF

