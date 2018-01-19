// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#define Lith_Version "1.6 (Hyper-Dilithium)"
#define Lith_APIVersion 160
#define Lith_CheckAPIVersion() \
   do \
      if(world.apiversion != Lith_APIVersion) { \
         printf(c"%s: Invalid API version! Expected %i, but got %i\n", \
            __func__, Lith_APIVersion, world.apiversion); \
         abort(); \
      } \
   while(0)
#include <stdio.h>
// 160 - 1.6 (Hyper-Dilithium)
// 155 - 1.5.3 (Indicolite Pegmatite)
// 154 - 1.5.1 & 1.5.2 (Rubellite Pegmatite)
// 153 - 1.5 (Pegmatite)
// 152 - 1.5 Beta (Pegmatite)
// 151 - 1.5 Alpha 2 (Pegmatite)
// 150 - 1.5 Alpha (Pegmatite)
// 140 - 1.4 (Lumate)
// 131 - 1.3.1 (LITHIUM - Hell Knights Insist On Dying)
// 130 - 1.3 (Zabuyelite) aka. (LITHIUM - Hell Knights Insist On Dying)
// 120 - 1.2 (Lithium Carbonate)
// 110 - 1.1 (Lithium Chloride)
// 100 - 1.0 (Lithium)

// EOF
