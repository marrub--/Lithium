/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Legacy HudMessage IDs and new Fade IDs.
 *
 * ---------------------------------------------------------------------------|
 */

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
   fid_scopecoS,
   fid_scopecoE = fid_scopecoS + 4,
   fid_scopecgS,
   fid_scopecgE = fid_scopecgS + 200,
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
};

#endif
