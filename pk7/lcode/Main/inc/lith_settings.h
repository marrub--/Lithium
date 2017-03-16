Category("Auto-Groups")
CBox("Automatically buy group #1", p->autobuy[0], ToggleAutoBuy(p, 0))
CBox("Automatically buy group #2", p->autobuy[1], ToggleAutoBuy(p, 1))
CBox("Automatically buy group #3", p->autobuy[2], ToggleAutoBuy(p, 2))
CBox("Automatically buy group #4", p->autobuy[3], ToggleAutoBuy(p, 3))

Category("GUI")
Float("Horizontal cursor speed", "lith_gui_xmul", 0.1f, 2.0f)
Float("Vertical cursor speed",   "lith_gui_ymul", 0.1f, 2.0f)

Category("Player")
Float("Damage bob multiplier", "lith_player_damagebobmul", 0.0f, 1.0f)
Bool("Bob view when damaged",  "lith_player_damagebob")
Bool("Make a sound when score is gained", "lith_player_scoresound")
Bool("Invert mouse in netgames (hack)",   "lith_player_invertmouse")
Bool("Log score gained",                  "lith_player_scorelog")
Bool("Play sound on results screen",      "lith_player_resultssound")

Category("Weapons")
Float("Scope zoom factor", "lith_weapons_zoomfactor", 1.0f, 10.0f)
Float("Scope opacity",     "lith_weapons_scopealpha", 0.0f, 1.0f)
Float("Weapon opacity",    "lith_weapons_alpha",      0.0f, 1.0f)
Bool("Modal Rifle scope",  "lith_weapons_riflescope")
Bool("Clear rifle mode on switch", "lith_weapons_riflemodeclear")

Category("Heads Up Display")
Bool("Show score",   "lith_hud_showscore")
Bool("Show weapons", "lith_hud_showweapons")
Bool("Show log",     "lith_hud_showlog")
Bool("Draw log from top of screen", "lith_hud_logfromtop")
Bool("Draw reactive armor indicator", "lith_hud_showarmorind")

Category("Vital Scanner")
Bool("Slide to target", "lith_scanner_slide")
Int("X offset",         "lith_scanner_xoffs", -160, 160)
Int("Y offset",         "lith_scanner_yoffs", -180, 20)
Enum("Color",           "lith_scanner_color", 'a', 'v', "\C%c%S", set, ColorName(set))
Bool("Alternate font",  "lith_scanner_altfont")

if(ACS_GameType() == GAME_SINGLE_PLAYER)
{
   Category("Single-Player")
   ServerBool("Explode on death",       "lith_sv_revenge")
   ServerBool("Stupid pickup messages", "lith_sv_stupidpickups")
   ServerFloat("Score multiplier (changes on new map)", "lith_sv_scoremul", 0, 10)
   ServerBool("Are you serious?",       "lith_sv_seriousmode")
}

#undef Category
#undef Bool
#undef ServerBool
#undef ServerFloat
#undef Float
#undef Int
#undef Enum
#undef CBox

// EOF

