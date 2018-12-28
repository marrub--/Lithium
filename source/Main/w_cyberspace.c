// Copyright © 2018 Alison Sanderson, all rights reserved.
#if LITHIUM
#include "common.h"
#include "m_file.h"
#include "m_token.h"
#include "m_tokbuf.h"

script ext("ACS")
void Lith_CSLoadMap(void)
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
