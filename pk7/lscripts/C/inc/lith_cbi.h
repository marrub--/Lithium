#ifndef LITH_CBI_H
#define LITH_CBI_H

[[__call("ScriptI")]]
void Lith_PlayerInitCBI(struct player_s *p);

[[__call("ScriptI")]]
void Lith_PlayerUpdateCBI(struct player_s *p);

[[__call("ScriptI")]]
void Lith_PlayerDrawCBI(struct player_s *p);

#endif

