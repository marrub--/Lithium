// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Version number handling.                                                 │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "m_version.h"

#define VERSION(maj, min, pat, bui, bna, nam) \
   i32  vernum_##maj##_##min##_##pat##_##bui = VerNum(maj, min, pat, bui, bna, nam); \
   cstr vernam_##maj##_##min##_##pat##_##bui = VerNam(maj, min, pat, bui, bna, nam); \
   cstr verstr_##maj##_##min##_##pat##_##bui = VerStr(maj, min, pat, bui, bna, nam);
#include "m_version.h"

i32 version_name_to_num(cstr s) {
   char *eptr = nil;
   k32 k = strtofxk(s, &eptr);
   if(eptr && *eptr == '\0') {
      switch((i32)(k * 100.0k)) {
      case 1000: return vernum_1_0_0_0;
      case 1100: return vernum_1_1_0_0;
      case 1200: return vernum_1_2_0_0;
      case 1300: return vernum_1_3_0_0;
      case 1310: return vernum_1_3_1_0;
      case 1400: return vernum_1_4_0_0;
      case 1501: return vernum_1_5_0_0;
      case 1502: return vernum_1_5_0_1;
      case 1503: return vernum_1_5_0_2;
      case 1504: return vernum_1_5_0_3;
      case 1510: return vernum_1_5_1_0;
      case 1520: return vernum_1_5_2_0;
      case 1530: return vernum_1_5_3_0;
      case 1600: return vernum_1_6_0_0;
      case 1610: return vernum_1_6_1_0;
      case 1620: return vernum_1_6_2_0;
      case 1630: return vernum_1_6_3_0;
      }
   }
   #define VERSION(maj, min, pat, bui, bna, nam) \
      if(faststrcasechk(s, verstr_##maj##_##min##_##pat##_##bui)) { \
         return vernum_##maj##_##min##_##pat##_##bui; \
      }
   #include "m_version.h"
   return vernum;
}

/* EOF */
