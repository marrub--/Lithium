// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Player attributes.                                                       │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef p_attrib_h
#define p_attrib_h

enum ZscName(Attrib) {
   at_acc,
   at_def,
   at_str,
   at_vit,
   at_stm,
   at_luk,
   at_spc,
   at_max
};

#if !ZscOn
#include "m_types.h"

enum {
   atsys_auto,
   atsys_hybrid,
   atsys_manual,
   atsys_max,
};

enum {_base_exp = 4000};

struct player_attributes {
   i32 expprev, exp, expnext;
   i32 level;
   i32 points;
   i32 attrs[at_max];
};

void attr_giveexp(i32 amt);
cstr attr_name(i32 which);
void attr_draw(void);
void attr_gui(struct gui_state *g, i32 yofs);
stkcall k32 attr_accbuff(void);
stkcall i32 attr_defbuff(void);
stkcall i32 attr_strbuff(void);
stkcall k32 attr_vitbuff(void);
stkcall i32 attr_stmbuff(void);
stkcall i32 attr_stmtime(void);
stkcall k32 attr_lukbuff(void);
stkcall k32 attr_rgebuff(void);
stkcall k32 attr_conbuff(void);
stkcall k32 attr_refbuff(void);
stkcall i32 attr_refchargebuff(void);
#endif

#endif
