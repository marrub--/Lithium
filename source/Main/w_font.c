// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// w_font.c: Unicode font handling.

#if LITHIUM
#include "common.h"
#include "m_file.h"

#define nglyphs 34
#define nblocks 64
#define ngroups 32
#define nplanes 16

// Types ---------------------------------------------------------------------|

// NOTE: DO NOT change the layout of this without also changing `RetOfs`s
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

static i32 curfont, setfont;

// Static Functions ----------------------------------------------------------|

stkcall
static struct glyph *AllocFontMetric(font *planes, i32 key)
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
static void SetFontMetric(i32 key, i32 xadv, i32 yofs)
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

script_str ext("ACS")
struct glyph *Lith_GetFontMetric(i32 key)
{
   return &(*(*(*(*fonts[setfont])
      [key / (nglyphs * nblocks  * ngroups)])
      [key / (nglyphs * nblocks) % ngroups ])
      [key /  nglyphs            % nblocks ])
      [key                       % nglyphs ];
}

// hand-written assembly for these, saves a few cycles
#define RetOfs(n) \
   __asm \
   ( \
      "AddI(Stk 1() LocReg 1(Lit 1(:metr)) Lit 1(" #n "_s31.0))" \
      "Move(Stk 1() Sta 1(Stk 1()))" \
      "Retn(Stk 1())" \
   ); \
   return 0 /* dummy return to keep the compiler quiet */

script_str ext("ACS") i32 Lith_Metr_Xadv(struct glyph *metr) {RetOfs(1);}
script_str ext("ACS") i32 Lith_Metr_Yofs(struct glyph *metr) {RetOfs(2);}
script_str ext("ACS") i32 Lith_Metr_Tex (struct glyph *metr) {RetOfs(3);}
script_str ext("ACS") i32 Lith_Metr_W   (struct glyph *metr) {RetOfs(4);}
script_str ext("ACS") i32 Lith_Metr_H   (struct glyph *metr) {RetOfs(5);}

script_str ext("ACS") void Lith_SetFontFace(i32 f) {setfont = f;}

script_str ext("ACS")
bool Lith_SetupFontsBegin(i32 fontnum)
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

script_str ext("ACS")
bool Lith_SetupFontsContinue(void)
{
   for(i32 i = 0; i < 300; i++)
   {
      i32 key;
      i32 xadv, yofs;

      if(fscanf(fp, "%i,%i,%i\n", &key, &xadv, &yofs) != 3)
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
