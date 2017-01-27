Category("Player")
Float("Damage bob multiplier", "lith_player_damagebobmul", 0.0f, 1.0f)
Bool("Bob view when damaged", "lith_player_damagebob")
Bool("Make a sound when score is gained", "lith_player_scoresound")
Bool("Invert mouse in netgames (hack)",   "lith_player_invertmouse")

Category("Weapons")
Float("Scope zoom factor", "lith_weapons_zoomfactor", 1.0f, 10.0f)
Float("Scope opacity",  "lith_weapons_scopealpha", 0.0f, 1.0f)
Float("Weapon opacity", "lith_weapons_alpha", 0.0f, 1.0f)
Bool("Modal Rifle scope", "lith_weapons_riflescope")

Category("Heads Up Display")
Bool("Show score",   "lith_hud_showscore")
Bool("Show weapons", "lith_hud_showweapons")
Bool("Sliding Vital Scanner", "lith_hud_movescanner")

if(ACS_GameType() == GAME_SINGLE_PLAYER)
{
   Category("Single-Player")
   ServerBool("Explode on death",       "lith_sv_revenge")
   ServerBool("Stupid pickup messages", "lith_sv_stupidpickups")
}

#undef Category
#undef Bool
#undef ServerBool
#undef Float

// EOF

