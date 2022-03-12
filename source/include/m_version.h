// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Version numbers and current version.                                     │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#define VerNum(maj, min, pat, bui, bna, nam) \
   (maj * 1000000 + \
    min *   10000 + \
    pat *     100 + \
    bui)
#define VerNam(maj, min, pat, bui, bna, nam) \
   #maj "." #min "." #pat bna " (" nam ")"
#define VerStr(maj, min, pat, bui, bna, nam) \
   #maj "." #min "." #pat bna

#ifdef VERSION
VERSION(1, 0, 0, 0, "",   "Lithium")
VERSION(1, 1, 0, 0, "",   "Lithium Chloride")
VERSION(1, 2, 0, 0, "",   "Lithium Carbonate")
VERSION(1, 3, 0, 0, "",   "Zabuyelite")
VERSION(1, 3, 1, 0, "",   "LITHIUM - Hell Knights Insist On Dying")
VERSION(1, 4, 0, 0, "",   "Lumate")
VERSION(1, 5, 0, 0, "a",  "Pegmatite")
VERSION(1, 5, 0, 1, "a2", "Pegmatite")
VERSION(1, 5, 0, 2, "b",  "Pegmatite")
VERSION(1, 5, 0, 3, "",   "Pegmatite")
VERSION(1, 5, 1, 0, "",   "Rubellite Pegmatite")
VERSION(1, 5, 2, 0, "",   "Rubellite Pegmatite")
VERSION(1, 5, 3, 0, "",   "Indicolite Pegmatite")
VERSION(1, 6, 0, 0, "",   "Hyper-Dilithium")
VERSION(1, 6, 1, 0, "",   "Hyper-Trilithium")
VERSION(1, 6, 2, 0, "",   "Trilithionite")
VERSION(1, 6, 3, 0, "",   "Trilithionite Red")
VERSION(1, 7, 0, 0, "b",  "Yttrium Oxide")
#undef VERSION
#else
#define VERSION(maj, min, pat, bui, bna, nam) \
   extern i32  vernum_##maj##_##min##_##pat##_##bui; \
   extern cstr vernam_##maj##_##min##_##pat##_##bui; \
   extern cstr verstr_##maj##_##min##_##pat##_##bui;
#include "m_version.h"
i32 version_name_to_num(cstr s);
#define vernum vernum_1_7_0_0
#define vernam vernam_1_7_0_0
#define verstr verstr_1_7_0_0
#endif

/* EOF */
