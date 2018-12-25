// Copyright © 2018 Alison Sanderson, all rights reserved.
#if defined(X)
X(COUNTKILL, "COUNTKILL")
X(GetPaused, "GetPaused")
X(LC, "LC")
X(LF, "LF")
X(LS, "LS")
X(LT, "LT")
X(LX, "LX")
X(LZ, "LZ")
X(MagicAmmo, sOBJ "MagicAmmo")
X(alienfont, "alienfont")
X(cbifont, "cbifont")
X(chfont, "chfont")
X(cnfont, "cnfont")
X(confont, "confont")
X(debug_level, sDCVAR "debug_level")
X(dbigfont, "dbigfont")
X(lhudfont, "lhudfont")
X(lhudfontsmall, "lhudfontsmall")
X(logfont, "logfont")
X(m_grabInput, "m_grabInput")
X(m_onground, "m_onground")
X(smallfnt, "smallfnt")
X(sv_autosave, sCVAR "sv_autosave")
X(sv_difficulty, sCVAR "sv_difficulty")

#if LITHIUM
X(Serv, sOBJ "HERMES")
X(Draw, sOBJ "URANUS")
#else
X(Serv, sOBJ "Server")
X(Draw, sOBJ "Render")

#endif
#undef X
#elif !defined(lith_stab_h)
#define lith_stab_h

#define X(n, s) extern __str const s_##n;
#include "lith_stab.h"

#endif
