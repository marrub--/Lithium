/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Attribute handling.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "w_world.h"

/* Static Functions -------------------------------------------------------- */

static void AttrBar(struct gui_state *g, i32 x, i32 y, i32 w, str gfx)
{
   G_Clip(g, x, y, w * 4, 8);
   PrintSprite(gfx, x,1, y,1);
   G_ClipRelease(g);
}

static void DrawAttr(struct gui_state *g, i32 x, i32 y, struct player *p, i32 at)
{
   u32  attr = p->attr.attrs[at];
   cstr name = p->attr.names[at];
   k32  helptrns = 0.5;

   if(p->attr.points)
      if(G_Button_Id(g, at, .x = x-42 + gui_p.btnnext.w, y-2, Pre(btnnext), .slide = true))
   {
      p->attr.points--;
      p->attr.attrs[at]++;
   }

   PrintTextChr(name, 3);
   PrintText(s_lmidfont, g->defcr, x-24,1, y,1);

   PrintSprite(sp_UI_AttrBar1, x,1, y,1);

   AttrBar(g, x, y, attr, sp_UI_AttrBar2);

   if(attr > ATTR_VIS_MAX)
   {
      i32 vatr = attr - ATTR_VIS_MAX;
      AttrBar(g, x, y, (vatr / (k64)ATTR_VIS_DIFF) * ATTR_VIS_MAX, sp_UI_AttrBar4);
      helptrns += 0.3;
   }

   PrintTextA_str(Language(LANG "ATTR_HELP_%.3s", name), s_smallfnt, g->defcr, x+1,1, y,1, helptrns);

   PrintTextFmt("%u/%i", attr, ATTR_VIS_MAX);
   PrintText(s_lmidfont, g->defcr, x+202,1, y,1);
}

static void StatusInfo(struct gui_state *g, i32 y, str left, str right)
{
   PrintText_str(left,  s_lmidfont, g->defcr,  30,1, y,1);
   PrintText_str(right, s_smallfnt, g->defcr, 280,2, y,1);
}

/* Extern Functions -------------------------------------------------------- */

void P_CBI_TabStatus(struct gui_state *g, struct player *p)
{
   Str(exp,   sLANG "STATUS_XP");
   Str(hp,    sLANG "STATUS_HP");
   Str(lv,    sLANG "STATUS_LV");
   Str(mp,    sLANG "STATUS_MP");
   Str(next,  sLANG "STATUS_NX");
   Str(class, sLANG "STATUS_CL");

   i32 y = 40;

   PrintText_str(p->name, s_lmidfont, g->defcr, 30,1, y,1);
   y += 10;

   StatusInfo(g, y += 10, L(class), p->classname);

   StatusInfo(g, y += 10, L(lv), StrParam("%u", p->attr.level));
   StatusInfo(g, y += 10, L(hp), StrParam("%i/%i", p->health, p->maxhealth));

   if(p->pclass & pcl_magicuser)
      StatusInfo(g, y += 10, mp, StrParam("%i/%i", p->mana, p->manamax));

   StatusInfo(g, y += 10, L(exp),  StrParam("%u", p->attr.exp));
   StatusInfo(g, y += 10, L(next), StrParam("%u", p->attr.expnext));

   y += p->pclass & pcl_magicuser ? 20 : 30;

   if(p->attr.points) {
      PrintTextFmt(LC(LANG "STATUS_LEVELUP"), p->attr.points);
      PrintText(s_smallfnt, g->defcr, 20,1, y,1);
   }

   y += 10;

   for(i32 i = 0; i < at_max; i++, y += 10)
      DrawAttr(g, 53, y, p, i);
}

/* EOF */
