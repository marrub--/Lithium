#ifndef LITH_HUD_H
#define LITH_HUD_H

[[__call("ScriptI")]]
void Lith_RenderHUDWaves(struct player_s *p);

[[__call("ScriptI")]]
void Lith_RenderHUDStringStack(struct player_s *p);

[[__call("ScriptI")]]
void Lith_PlayerHUD(struct player_s *p);

[[__call("ScriptI")]]
void Lith_SendingACK(player_t *p);

#endif

