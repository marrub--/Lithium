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

#define ATTR_BAS_MAX 60
#define ATTR_VIS_MAX 50
#define ATTR_VIS_DIFF (ATTR_BAS_MAX - ATTR_VIS_MAX)

enum {
   atsys_auto,
   atsys_hybrid,
   atsys_manual,
   atsys_max,
};

struct player_attributes {
   i32 expprev, exp, expnext;
   i32 level;
   i32 points;
   i32 attrs[at_max];

   char names[at_max][4];
   char lvupstr[130];
   i32  lvupstrn;
};

#endif
