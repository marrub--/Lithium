// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// w_cyberspace.c: Main Cyberspace handling.

#if LITHIUM
#include "common.h"
#include "m_file.h"
#include "m_token.h"
#include "m_tokbuf.h"

// Scripts -------------------------------------------------------------------|

script_str ext("ACS") addr("Lith_CSLoadMap")
void Sc_CSLoadMap(void)
{
   FILE *fp = W_Open(sp_lfiles_Cyberspace, "r");
   i32 w, h, *map;

   fscanf(fp, "%i %i", &w, &h);

   map = Malloc(sizeof *map * w * h);

   for(i32 y = 0; y < h; y++) for(i32 x = 0; x < w; x++)
      fscanf(fp, "%i ", &map[x + y * w]);

   fclose(fp);
   Dalloc(map);
}
#endif

// EOF
