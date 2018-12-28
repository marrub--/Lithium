// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifndef p_hudid_h
#define p_hudid_h

enum
{
   fid_health,
   fid_vscan,
   fid_schit1,
   fid_schit2,
   fid_scacum,
   fid_slotnS,
   fid_slotnE = fid_slotnS + 8,
   fid_logadS,
   fid_logadE = fid_logadS + 8,
};

enum
{
   hid_base = 0x100000,

   hid_base_payout = hid_base,
   hid_end_payout  = hid_base + 999,

   hid_base_misc = hid_base + 1000,
   hid_end_misc  = hid_base + 1999,

   hid_teleport = hid_base_misc,
   hid_teleportback,

   hid_zoomvignette,
   hid_zoombase,
   hid_zoomcomp,
   hid_zoomcam = hid_zoomcomp + 8,

   hid_sigil_title,
   hid_sigil_subtitle,

   hid_magicsel,
   hid_blade,

   hid_jetE,
   hid_jetS = hid_jetE + 15,
   hid_jettext,
   hid_jetbg,

   hid_adrenind_fxE,
   hid_adrenind_fxS = hid_adrenind_fxE + 45,

   hid_log_throwaway,

   hid_scope_sineE,
   hid_scope_sineS = hid_scope_sineE + 200,
   hid_scope_squareE,
   hid_scope_squareS = hid_scope_squareE + 200,
   hid_scope_triE,
   hid_scope_triS = hid_scope_triE + 200,
   hid_scope_overlayE,
   hid_scope_overlayS = hid_scope_overlayE + 5,
   hid_scope_lineE,
   hid_scope_lineS = hid_scope_lineE + 200,

   hid_scope_clearS = hid_scope_sineE,
   hid_scope_clearE = hid_scope_triS,
};

#endif
