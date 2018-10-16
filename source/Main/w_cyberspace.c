// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_file.h"
#include "lith_token.h"
#include "lith_tokbuf.h"

script ext("ACS")
void Lith_CSLoadMap(void)
{
   FILE *fp = W_Open("lfiles/Cyberspace.txt", c"r");
   int w, h, *map;

   __fscanf_str(fp, "%i %i", &w, &h);

   map = Malloc(sizeof(int) * w * h);

   for(int y = 0; y < h; y++) for(int x = 0; x < w; x++)
      __fscanf_str(fp, "%i ", &map[x + y * w]);

   fclose(fp);
   Dalloc(map);
}

// EOF
