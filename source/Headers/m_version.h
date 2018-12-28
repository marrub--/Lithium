// Copyright © 2016-2018 Alison Sanderson, all rights reserved.
#if LITHIUM
#define Lith_Version "Lithium 1.6.1 (Hyper-Trilithium)"
#else
#define Lith_Version "DoubleTap 1.0"
#endif
#define Lith_APIVersion Lith_v1_6_1
#define Lith_CheckAPIVersion() \
   do { \
      if(world.apiversion != Lith_APIVersion) { \
         printf(c"%s: Invalid API version! Expected %i, but got %i\n", \
            __func__, Lith_APIVersion, world.apiversion); \
         abort(); \
      } \
   } while(0)
#include <stdio.h>
#define Lith_v1_6_1   16.1  // 1.6.1 (Hyper-Trilithium)
#define Lith_v1_6_0   16.0  // 1.6 (Hyper-Dilithium)
#define Lith_v1_5_3   15.3  // 1.5.3 (Indicolite Pegmatite)
#define Lith_v1_5_2   15.2  // 1.5.2 (Rubellite Pegmatite)
#define Lith_v1_5_1   15.1  // 1.5.1 (Rubellite Pegmatite)
#define Lith_v1_5_0   15.04 // 1.5 (Pegmatite)
#define Lith_v1_5_0b  15.03 // 1.5 Beta (Pegmatite)
#define Lith_v1_5_0a2 15.02 // 1.5 Alpha 2 (Pegmatite)
#define Lith_v1_5_0a  15.01 // 1.5 Alpha (Pegmatite)
#define Lith_v1_4_0   14.0  // 1.4 (Lumate)
#define Lith_v1_3_1   13.1  // 1.3.1 (LITHIUM - Hell Knights Insist On Dying)
#define Lith_v1_3_0   13.0  // 1.3 (Zabuyelite)
#define Lith_v1_2_0   12.0  // 1.2 (Lithium Carbonate)
#define Lith_v1_1_0   11.0  // 1.1 (Lithium Chloride)
#define Lith_v1_0_0   10.0  // 1.0 (Lithium)

// EOF
