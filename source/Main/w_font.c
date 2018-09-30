// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_common.h"

#define nglyphs 34
#define nblocks 64
#define ngroups 32
#define nplanes 16

struct glyph
{
   int key;
   int xadv;
   int yofs;
   int tex;
   int w, h;
};

typedef struct glyph glyphdata[nglyphs];
typedef glyphdata   *blockdata[nblocks];
typedef blockdata   *groupdata[ngroups];
typedef groupdata   *planedata[nplanes];

static planedata *planes;

//
// AllocFontMetric
//
stkcall
static struct glyph *AllocFontMetric(uint key)
{
   if(! planes)  planes = Salloc(planedata);
   groupdata   **groups = &( *planes)[key / (nglyphs * nblocks  * ngroups)];

   if(!*groups) *groups = Salloc(groupdata);
   blockdata   **blocks = &(**groups)[key / (nglyphs * nblocks) % ngroups ];

   if(!*blocks) *blocks = Salloc(blockdata);
   glyphdata   **glyphs = &(**blocks)[key / (nglyphs)           % nblocks ];

   if(!*glyphs) *glyphs = Salloc(glyphdata);
   return                 &(**glyphs)[key                       % nglyphs ];
}

//
// Lith_GetFontMetric
//
script ext("ACS")
struct glyph *Lith_GetFontMetric(uint key)
{
   groupdata *groups = (*planes)[key / (nglyphs * nblocks  * ngroups)];
   blockdata *blocks = (*groups)[key / (nglyphs * nblocks) % ngroups ];
   glyphdata *glyphs = (*blocks)[key / (nglyphs)           % nblocks ];
   return             &(*glyphs)[key                       % nglyphs ];
}

//
// Lith_SetFontMetric
//
script ext("ACS")
void Lith_SetFontMetric(uint key, int xadv, int yofs)
{
   struct glyph *metr = AllocFontMetric(key);

   int tex = URANUS("GetTex", StrParam("lgfx/Font/Jp12/%u.png", key));
   int   w = URANUS("GetTexW", tex) / 2;
   int   h = URANUS("GetTexH", tex) / 2;

   metr->key  = key;
   metr->xadv = xadv / 2;
   metr->yofs = (h + 2) + yofs / 2;
   metr->tex  = tex;
   metr->w    = w;
   metr->h    = h;
}

script ext("ACS") int Lith_Metr_Xadv(struct glyph *metr) {return metr->xadv;}
script ext("ACS") int Lith_Metr_Yofs(struct glyph *metr) {return metr->yofs;}
script ext("ACS") int Lith_Metr_Tex (struct glyph *metr) {return metr->tex ;}
script ext("ACS") int Lith_Metr_W   (struct glyph *metr) {return metr->w   ;}
script ext("ACS") int Lith_Metr_H   (struct glyph *metr) {return metr->h   ;}

// EOF
