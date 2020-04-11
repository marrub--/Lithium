/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Version numbers and current version.
 *
 * ---------------------------------------------------------------------------|
 */

#define Version "Lithium 1.7 (Yttrium Oxide)"
#define APIVersion v1_7_0
#define CheckAPIVersion() \
   if(apiversion != APIVersion) { \
      ACS_BeginLog(); \
      __nprintf(c"%s: Invalid API version! Expected %i, but got %i\n", \
                __func__, APIVersion, apiversion); \
      ACS_EndLog(); \
      abort(); \
   } else (void)0
#include <stdio.h>
#define v1_7_0   17.0  /* 1.7 (Yttrium Oxide) */
#define v1_6_3   16.3  /* 1.6.3 (Trilithionite Red) */
#define v1_6_2   16.2  /* 1.6.2 (Trilithionite) */
#define v1_6_1   16.1  /* 1.6.1 (Hyper-Trilithium) */
#define v1_6_0   16.0  /* 1.6 (Hyper-Dilithium) */
#define v1_5_3   15.3  /* 1.5.3 (Indicolite Pegmatite) */
#define v1_5_2   15.2  /* 1.5.2 (Rubellite Pegmatite) */
#define v1_5_1   15.1  /* 1.5.1 (Rubellite Pegmatite) */
#define v1_5_0   15.04 /* 1.5 (Pegmatite) */
#define v1_5_0b  15.03 /* 1.5 Beta (Pegmatite) */
#define v1_5_0a2 15.02 /* 1.5 Alpha 2 (Pegmatite) */
#define v1_5_0a  15.01 /* 1.5 Alpha (Pegmatite) */
#define v1_4_0   14.0  /* 1.4 (Lumate) */
#define v1_3_1   13.1  /* 1.3.1 (LITHIUM - Hell Knights Insist On Dying) */
#define v1_3_0   13.0  /* 1.3 (Zabuyelite) */
#define v1_2_0   12.0  /* 1.2 (Lithium Carbonate) */
#define v1_1_0   11.0  /* 1.1 (Lithium Chloride) */
#define v1_0_0   10.0  /* 1.0 (Lithium) */

/* EOF */
