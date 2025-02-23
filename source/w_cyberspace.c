// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Main Cyberspace handling.                                                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "m_file.h"
#include "m_token.h"
#include "m_tokbuf.h"

script_str ext("ACS") addr(OBJ "CSLoadMap")
void Z_CSLoadMap(void) {
   #if 0
   FILE *fp = W_Open(sp_lmisc_Cyberspace, 't');
   i32 w, h, *map;

   fscanf(fp, "%i %i", &w, &h);

   for(i32 y = 0; y < h; y++) {
      for(i32 x = 0; x < w; x++) {
         fscanf(fp, "%i ", &map[x + y * w]);
      }
   }

   fclose(fp);
   #endif
}

/* EOF */
