// Copyright © 2018 Alison Sanderson, all rights reserved.
#ifndef m_cps_h
#define m_cps_h

#define Cps_Count(name) (countof(name) * 4)
#define Cps_Size(len) ((len) / 4 + (len) % 4)
#define Cps_Defn(name, len) u32 (name)[Cps_Size(len)] = {}
#define Cps_Decl(name, len) u32 (name)[Cps_Size(len)]
#define Cps_Shif(i, set) ((set) << ((i) % 4 * 8))

#define Cps_SetC(name, i, set) \
   ((name)[(i) / 4] &= ~Cps_Shif(i, 0xFF), \
    (name)[(i) / 4] |=  Cps_Shif(i, set & 0xFF))

#define Cps_GetC(name, i) \
   (((name)[(i) / 4] & (0xFF << ((i) % 4 * 8))) >> ((i) % 4 * 8))

char const *Cps_Print(u32 *cps, i32 l);

#endif
