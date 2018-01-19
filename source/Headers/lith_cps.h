// Copyright © 2018 Graham Sanderson, all rights reserved.
#ifndef LITH_CPS_H
#define LITH_CPS_H

#define Lith_CPS_Count(name) (countof(name) * 4)

#define Lith_CPS_Defn(name, len) \
   unsigned (name)[(len) / 4] = {}

#define Lith_CPS_Decl(name, len) \
   unsigned (name)[(len) / 4]

#define Lith_CPS_SetC(name, i, set) \
   ((name)[(i) / 4] &= ~(0xFF << ((i) % 4 * 8)), \
    set ? (name)[(i) / 4] |= (set & 0xFF) << ((i) % 4 * 8) : 0)

#define Lith_CPS_GetC(name, i) \
   (((name)[(i) / 4] & (0xFF << ((i) % 4 * 8))) >> ((i) % 4 * 8))

__str Lith_CPS_Print(unsigned *cps);

#endif
