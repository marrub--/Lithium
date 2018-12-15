// Copyright © 2017-2018 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

StrEntON

// Static Functions ----------------------------------------------------------|

static void AttrBar(gui_state_t *g, int x, int y, int w, __str gfx)
{
   Lith_GUI_Clip(g, x, y, w * 4, 8);
   PrintSprite(gfx, x,1, y,1);
   Lith_GUI_ClipRelease(g);
}

static void DrawAttr(gui_state_t *g, int x, int y, struct player *p, int at)
{
   u32         attr = p->attr.attrs[at];
   char const *name = p->attr.names[at];
   fixed helptrns = 0.5;

   if(p->attr.points)
      if(Lith_GUI_Button_Id(g, at, .x = x-42 + guipre.btnnext.w, y-2, Pre(btnnext), .slide = true))
   {
      p->attr.points--;
      p->attr.attrs[at]++;
   }

   PrintTextFmt("%.3s", name);
   PrintText("chfont", CR_WHITE, x-24,1, y,1);

   PrintSprite(":UI:AttrBar1", x,1, y,1);

   AttrBar(g, x, y, attr, ":UI:AttrBar2");

   if(attr > ATTR_VIS_MAX)
   {
      int vatr = attr - ATTR_VIS_MAX;
      AttrBar(g, x, y, (vatr / (fixed64)ATTR_VIS_DIFF) * ATTR_VIS_MAX, ":UI:AttrBar4");
      helptrns += 0.3;
   }

   PrintTextStr(Language(LANG "ATTR_HELP_%s", name));
   PrintTextA("chfont", CR_WHITE, x+1,1, y+1,1, helptrns);

   PrintTextFmt("%u/%i", attr, ATTR_VIS_MAX);
   PrintText("chfont", CR_WHITE, x+202,1, y,1);
}

static void StatusInfo(gui_state_t *g, int x, int y, __str left, __str right)
{
   PrintTextStr(left);
   PrintText("chfont", CR_WHITE, x,1, y,1);

   PrintTextStr(right);
   PrintText("chfont", CR_WHITE, x+80,2, y,1);
}

// Extern Functions ----------------------------------------------------------|

void Lith_CBITab_Status(gui_state_t *g, struct player *p)
{
   int x = 30, y = 40;

   PrintTextStr(p->name);
   PrintText("chfont", CR_WHITE, x,1, y,1);
   y += 10;

   PrintTextStr(p->classname);
   PrintText("chfont", CR_WHITE, x,1, y,1);

   StatusInfo(g, x, y += 10, "Lv.",  StrParam("%u", p->attr.level));
   StatusInfo(g, x, y += 10, "HP",   StrParam("%i/%i", p->health, p->maxhealth));

   if(p->pclass & pcl_magicuser)
      StatusInfo(g, x, y += 10, "MP", StrParam("%i/%i", p->mana, p->manamax));

   StatusInfo(g, x, y += 10, "EXP",  StrParam("%u", p->attr.exp));
   StatusInfo(g, x, y += 10, "Next", StrParam("%u", p->attr.expnext));

   x  = 20;
   y += p->pclass & pcl_magicuser ? 20 : 30;

   if(p->attr.points)
   {
      PrintTextFmt("Divide %u points among your attributes.", p->attr.points);
      PrintText("chfont", CR_WHITE, x,1, y,1);
   }

   x  = 53;
   y += 10;

   for(int i = 0; i < at_max; i++, y += 10)
      DrawAttr(g, x, y, p, i);
}

// EOF
