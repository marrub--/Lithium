// Copyright © 2018 Alison Sanderson, all rights reserved.
#if LITHIUM
#include "common.h"
#include "m_file.h"

#define nglyphs 34
#define nblocks 64
#define ngroups 32
#define nplanes 16

// Types ---------------------------------------------------------------------|

struct glyph
{
   i32 key;
   i32 xadv;
   i32 yofs;
   i32 tex;
   i32 w, h;
};

typedef struct glyph glyphdata[nglyphs];
typedef glyphdata   *blockdata[nblocks];
typedef blockdata   *groupdata[ngroups];
typedef groupdata   *planedata[nplanes];
typedef planedata   *font;

// Static Objects ------------------------------------------------------------|

static str const fontnames[] = {
   [font_misaki_gothic] = s"MisakiG",
   [font_misaki_mincho] = s"MisakiM",
   [font_jfdot_gothic]  = s"JFDotG",
};

static font fonts[font_num];

static FILE *fp;

static u32 curfont, setfont;

// Static Functions ----------------------------------------------------------|

stkcall
static struct glyph *AllocFontMetric(font *planes, u32 key)
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
static void SetFontMetric(u32 key, i32 xadv, i32 yofs)
{
   struct glyph *metr = AllocFontMetric(&fonts[curfont], key);

   i32 tex = DrawCallI(sm_GetTex, StrParam("lgfx/Font/%S/%u.png", fontnames[curfont], key));
   i32   w = DrawCallI(sm_GetTexW, tex);
   i32   h = DrawCallI(sm_GetTexH, tex);

   metr->key  = key;
   metr->xadv = xadv;
   metr->yofs = h + yofs;
   metr->tex  = tex;
   metr->w    = w;
   metr->h    = h;
}

// Extern Functions ----------------------------------------------------------|

script ext("ACS")
struct glyph *Lith_GetFontMetric(u32 key)
{
   planedata *planes = fonts[setfont];
   groupdata *groups = (*planes)[key / (nglyphs * nblocks  * ngroups)];
   blockdata *blocks = (*groups)[key / (nglyphs * nblocks) % ngroups ];
   glyphdata *glyphs = (*blocks)[key /  nglyphs            % nblocks ];
   return             &(*glyphs)[key                       % nglyphs ];
}

script ext("ACS") i32 Lith_Metr_Xadv(struct glyph *metr) {return metr->xadv;}
script ext("ACS") i32 Lith_Metr_Yofs(struct glyph *metr) {return metr->yofs;}
script ext("ACS") i32 Lith_Metr_Tex (struct glyph *metr) {return metr->tex ;}
script ext("ACS") i32 Lith_Metr_W   (struct glyph *metr) {return metr->w   ;}
script ext("ACS") i32 Lith_Metr_H   (struct glyph *metr) {return metr->h   ;}

script ext("ACS") void Lith_SetFontFace(u32 f) {setfont = f;}

script ext("ACS")
bool Lith_SetupFontsBegin(u32 fontnum)
{
   if(fontnum >= font_num) return false;

   curfont = fontnum;
   fp = W_Open(StrParam("lfiles/Font_%S.txt", fontnames[fontnum]), c"r");

   if(fp)
      return true;
   else
   {
      Log("Warning: Font file not found for font %u (%S), UI may break",
         fontnum, fontnames[fontnum]);
      return false;
   }
}

script ext("ACS")
bool Lith_SetupFontsContinue(void)
{
   for(i32 i = 0; i < 300; i++)
   {
      u32 key;
      i32 xadv, yofs;

      if(fscanf(fp, "%u,%i,%i\n", &key, &xadv, &yofs) != 3)
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
