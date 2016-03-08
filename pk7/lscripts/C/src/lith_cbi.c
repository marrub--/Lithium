#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"

// ---------------------------------------------------------------------------
// Computer-Brain Interface (CBI) Scripts.
//

[[__call("ScriptS"), __extern("ACS")]]
void Lith_KeyOpenCBI()
{
   player_t *p = &players[ACS_PlayerNumber()];
   
   p->cbi.open = !p->cbi.open;
   
   if(p->cbi.open)
      ACS_LocalAmbientSound("player/cbi/open", 127);
   else
      ACS_LocalAmbientSound("player/cbi/close", 127);
}

[[__call("ScriptI")]]
void Lith_PlayerUpdateCBI(player_t *p)
{
}

[[__call("ScriptI")]]
void Lith_PlayerDrawCBI(player_t *p)
{
   if(p->cbi.open)
   {
      HudMessageRainbowsF("BIGFONT", "yay it works");
      HudMessagePlain(hid_cbi_test, 0.0, 0.0, 0.1);
   }
}

//
// ---------------------------------------------------------------------------

