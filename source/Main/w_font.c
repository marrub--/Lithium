// Copyright © 2018 Alison Sanderson, all rights reserved.
#if LITHIUM
#include "lith_common.h"
#include "lith_file.h"

StrEntON

#define nglyphs 34
#define nblocks 64
#define ngroups 32
#define nplanes 16

// Types ---------------------------------------------------------------------|

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
typedef planedata   *font;

// Static Objects ------------------------------------------------------------|

static char const *fontnames[] = {
   [font_misaki_gothic] = c"MisakiG",
   [font_misaki_mincho] = c"MisakiM",
   [font_jfdot_gothic]  = c"JFDotG",
};

static font fonts[font_num];

static FILE *fp;

static uint curfont, setfont;

// Static Functions ----------------------------------------------------------|

stkcall
static struct glyph *AllocFontMetric(font *planes, uint key)
{
   if(!*planes) *planes = Salloc(planedata);
   groupdata   **groups = &(**planes)[key / (nglyphs * nblocks  * ngroups)];

   if(!*groups) *groups = Salloc(groupdata);
   blockdata   **blocks = &(**groups)[key / (nglyphs * nblocks) % ngroups ];

   if(!*blocks) *blocks = Salloc(blockdata);
   glyphdata   **glyphs = &(**blocks)[key /  nglyphs            % nblocks ];

   if(!*glyphs) *glyphs = Salloc(glyphdata);
   return                 &(**glyphs)[key                       % nglyphs ];
}

stkcall
static void SetFontMetric(uint key, int xadv, int yofs)
{
   struct glyph *metr = AllocFontMetric(&fonts[curfont], key);

   int tex = DrawCallI("GetTex", StrParam("lgfx/Font/%s/%u.png", fontnames[curfont], key));
   int   w = DrawCallI("GetTexW", tex);
   int   h = DrawCallI("GetTexH", tex);

   metr->key  = key;
   metr->xadv = xadv;
   metr->yofs = h + yofs;
   metr->tex  = tex;
   metr->w    = w;
   metr->h    = h;
}

// Extern Functions ----------------------------------------------------------|

script ext("ACS")
struct glyph *Lith_GetFontMetric(uint key)
{
   planedata *planes = fonts[setfont];
   groupdata *groups = (*planes)[key / (nglyphs * nblocks  * ngroups)];
   blockdata *blocks = (*groups)[key / (nglyphs * nblocks) % ngroups ];
   glyphdata *glyphs = (*blocks)[key /  nglyphs            % nblocks ];
   return             &(*glyphs)[key                       % nglyphs ];
}

script ext("ACS") int Lith_Metr_Xadv(struct glyph *metr) {return metr->xadv;}
script ext("ACS") int Lith_Metr_Yofs(struct glyph *metr) {return metr->yofs;}
script ext("ACS") int Lith_Metr_Tex (struct glyph *metr) {return metr->tex ;}
script ext("ACS") int Lith_Metr_W   (struct glyph *metr) {return metr->w   ;}
script ext("ACS") int Lith_Metr_H   (struct glyph *metr) {return metr->h   ;}

script ext("ACS") void Lith_SetFontFace(uint f) {setfont = f;}

script ext("ACS")
bool Lith_SetupFontsBegin(uint fontnum)
{
   if(fontnum >= font_num) return false;

   curfont = fontnum;
   fp = W_Open(StrParam("lfiles/Font_%s.txt", fontnames[fontnum]), c"r");

   if(fp)
      return true;
   else
   {
      Log("Warning: Font file not found for font %u (%s), UI may break",
         fontnum, fontnames[fontnum]);
      return false;
   }
}

script ext("ACS")
bool Lith_SetupFontsContinue(void)
{
   for(int i = 0; i < 300; i++)
   {
      uint key;
      int xadv, yofs;

      if(fscanf(fp, c"%u,%i,%i\n", &key, &xadv, &yofs) != 3)
      {
         fclose(fp);
         return false;
      }

      SetFontMetric(key, xadv, yofs);
   }

   return true;
}
#endif

// EOF
