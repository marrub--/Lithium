// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Version number handling.                                                 │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "w_world.h"
#include "m_version.h"

i32 version_name_to_num(cstr s) {
   char *eptr = nil;
   k32 k = strtofxk(s, &eptr);
   if(eptr && *eptr == '\0') {
      switch((i32)(k * 100.0k)) {
      #define LEGACYVERSION(num, ver) case num: return ver;
      #include "m_version.h"
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
