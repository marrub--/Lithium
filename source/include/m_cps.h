/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Compressed strings.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef m_cps_h
#define m_cps_h

#define Cps_Adjust(len) ((len) / 4 + (((len) % 4) != 0))

#define Cps_CountOf(name) (countof(name) * 4)

#define Cps_Defn(name, len) u32 (name)[Cps_Adjust(len)] = {}
#define Cps_Decl(name, len) u32 (name)[Cps_Adjust(len)]

#define Cps_Expand_str(cps, s)   fast_strdup(Cps_Expand(cps, s))
#define Cps_ExpandNT_str(cps, s) fast_strdup(Cps_ExpandNT(cps, s))

stkcall void Cps_SetC(u32 *cps, u32 p, u32 c);
stkcall byte Cps_GetC(u32 const *cps, u32 p);
stkcall cstr Cps_Expand(u32 *cps, u32 s, u32 l);
stkcall cstr Cps_ExpandNT(u32 *cps, u32 s);

#endif
