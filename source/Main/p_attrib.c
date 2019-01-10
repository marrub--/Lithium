// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// p_attrib.c: Attribute handling.

#include "common.h"
#include "p_player.h"
#include "w_world.h"

// Static Functions ----------------------------------------------------------|

static void AttrBar(struct gui_state *g, i32 x, i32 y, i32 w, str gfx)
{
   Lith_GUI_Clip(g, x, y, w * 4, 8);
   PrintSprite(gfx, x,1, y,1);
   Lith_GUI_ClipRelease(g);
}

static void DrawAttr(struct gui_state *g, i32 x, i32 y, struct player *p, i32 at)
{
   u32         attr = p->attr.attrs[at];
   char const *name = p->attr.names[at];
   k32 helptrns = 0.5;

   if(p->attr.points)
      if(Lith_GUI_Button_Id(g, at, .x = x-42 + gui_p.btnnext.w, y-2, Pre(btnnext), .slide = true))
   {
      p->attr.points--;
      p->attr.attrs[at]++;
   }

   PrintTextChr(name, 3);
   PrintText(s_chfont, CR_WHITE, x-24,1, y,1);

   PrintSprite(sp_UI_AttrBar1, x,1, y,1);

   AttrBar(g, x, y, attr, sp_UI_AttrBar2);

   if(attr > ATTR_VIS_MAX)
   {
      i32 vatr = attr - ATTR_VIS_MAX;
      AttrBar(g, x, y, (vatr / (k64)ATTR_VIS_DIFF) * ATTR_VIS_MAX, sp_UI_AttrBar4);
      helptrns += 0.3;
   }

   PrintTextA_str(Language(LANG "ATTR_HELP_%.3s", name), s_chfont, CR_WHITE, x+1,1, y+1,1, helptrns);

   PrintTextFmt("%u/%i", attr, ATTR_VIS_MAX);
   PrintText(s_chfont, CR_WHITE, x+202,1, y,1);
}

static void StatusInfo(struct gui_state *g, i32 x, i32 y, str left, str right)
{
   PrintText_str(left,  s_chfont, CR_WHITE, x,1, y,1);
   PrintText_str(right, s_chfont, CR_WHITE, x+80,2, y,1);
}

// Extern Functions ----------------------------------------------------------|

void Lith_CBITab_Status(struct gui_state *g, struct player *p)
{
   i32 x = 30, y = 40;

   PrintText_str(p->name, s_chfont, CR_WHITE, x,1, y,1);
   y += 10;

   PrintText_str(p->classname, s_chfont, CR_WHITE, x,1, y,1);

   StatusInfo(g, x, y += 10, st_lv, StrParam("%u", p->attr.level));
   StatusInfo(g, x, y += 10, st_hp, StrParam("%i/%i", p->health, p->maxhealth));

   if(p->pclass & pcl_magicuser)
      StatusInfo(g, x, y += 10, st_mp, StrParam("%i/%i", p->mana, p->manamax));

   StatusInfo(g, x, y += 10, st_exp,  StrParam("%u", p->attr.exp));
   StatusInfo(g, x, y += 10, st_next, StrParam("%u", p->attr.expnext));

   x  = 20;
   y += p->pclass & pcl_magicuser ? 20 : 30;

   if(p->attr.points)
   {
      PrintTextFmt("Divide %u points among your attributes.", p->attr.points);
      PrintText(s_chfont, CR_WHITE, x,1, y,1);
   }

   x  = 53;
   y += 10;

   for(i32 i = 0; i < at_max; i++, y += 10)
      DrawAttr(g, x, y, p, i);
}

// EOF
