/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Fade IDs.
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
   fid_jet,
   fid_divsigil,
   fid_blade,
   fid_result,
   fid_result2,
   fid_bosstext,
   fid_advice,
};

#endif
