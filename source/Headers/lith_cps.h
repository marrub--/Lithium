// Copyright © 2018 Alison Sanderson, all rights reserved.
#ifndef lith_cps_h
#define lith_cps_h

#define Lith_CPS_Count(name) (countof(name) * 4)

#define Lith_CPS_Len(len) ((len) / 4 + (len) % 4)

#define Lith_CPS_Defn(name, len) \
   u32 (name)[Lith_CPS_Len(len)] = {}

#define Lith_CPS_Decl(name, len) \
   u32 (name)[Lith_CPS_Len(len)]

#define Lith_CPS_Shif(i, set) ((set) << ((i) % 4 * 8))

#define Lith_CPS_SetC(name, i, set) \
   ((name)[(i) / 4] &= ~Lith_CPS_Shif(i, 0xFF), \
    (name)[(i) / 4] |=  Lith_CPS_Shif(i, set & 0xFF))

#define Lith_CPS_GetC(name, i) \
   (((name)[(i) / 4] & (0xFF << ((i) % 4 * 8))) >> ((i) % 4 * 8))

char const *Lith_CPS_Print(u32 *cps, int l);

#endif
