// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Fade IDs.                                                                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef p_hudid_h
#define p_hudid_h

enum
{
   fid_deathmsg,
   fid_health,
   fid_vscan,
   fid_schit1,
   fid_schit2,
   fid_scacum,
   fid_slotnS,
   fid_slotnE = fid_slotnS + 8 - 1,
   fid_logadS,
   fid_logadE = fid_logadS + 8 - 1,
   fid_scopecoS,
   fid_scopecoE = fid_scopecoS + 4 - 1,
   fid_scopecgS,
   fid_scopecgE = fid_scopecgS + 200 - 1,
   fid_jet,
   fid_divsigil,
   fid_blade,
   fid_rendS,
   fid_rendE = fid_rendS + 4 - 1,
   fid_result,
   fid_result2,
   fid_bosstext,
   fid_advice,
   fid_subwepS,
   fid_subwepE = fid_subwepS + 6 - 1,
};

#endif
