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
   G_Clip(g, g->ox+x, g->oy+y, w * 4, 8);
   PrintSprite(gfx, g->ox+x,1, g->oy+y,1);
   G_ClipRelease(g);
}

static void DrawAttr(struct gui_state *g, i32 x, i32 y, i32 at)
{
   u32  attr = pl.attr.attrs[at];
   cstr name = pl.attr.names[at];
   k32  helptrns = 0.5;

   if(pl.attr.points &&
      G_Button_HId(g, at, .x = x-42 + gui_p.btnnext.w, y-2, Pre(btnnext),
                   .slide = true))
   {
      pl.attr.points--;
      pl.attr.attrs[at]++;
   }

   PrintTextChr(name, 3);
   PrintText(sf_lmidfont, g->defcr, g->ox+x-24,1, g->oy+y,1);

   PrintSprite(sp_UI_AttrBar1, g->ox+x,1, g->oy+y,1);

   AttrBar(g, x, y, attr, sp_UI_AttrBar2);

   if(attr > ATTR_VIS_MAX)
   {
      i32 vatr = attr - ATTR_VIS_MAX;
      AttrBar(g, x, y, (vatr / (k64)ATTR_VIS_DIFF) * ATTR_VIS_MAX, sp_UI_AttrBar4);
      helptrns += 0.3;
   }

   PrintTextA_str(Language(LANG "ATTR_HELP_%.3s", name), sf_smallfnt, g->defcr, g->ox+x+1,1, g->oy+y,1, helptrns);

   PrintTextFmt("%u/%i", attr, ATTR_VIS_MAX);
   PrintText(sf_lmidfont, g->defcr, g->ox+x+202,1, g->oy+y,1);
}

static void StatusInfo(struct gui_state *g, i32 y, str left, str right)
{
   PrintText_str(left,  sf_lmidfont, g->defcr, g->ox+ 17,1, g->oy+y,1);
   PrintText_str(right, sf_smallfnt, g->defcr, g->ox+267,2, g->oy+y,1);
}

/* Extern Functions -------------------------------------------------------- */

void P_CBI_TabStatus(struct gui_state *g)
{
   Str(sl_exp,   sLANG "STATUS_XP");
   Str(sl_hp,    sLANG "STATUS_HP");
   Str(sl_lv,    sLANG "STATUS_LV");
   Str(sl_mp,    sLANG "STATUS_MP");
   Str(sl_next,  sLANG "STATUS_NX");
   Str(sl_class, sLANG "STATUS_CL");

   i32 y = 27;

   PrintText_str(pl.name, sf_lmidfont, g->defcr, g->ox+17,1, g->oy+y,1);
   y += 10;

   StatusInfo(g, y, L(sl_class), pl.classname);
   y += 10;

   StatusInfo(g, y, L(sl_lv), StrParam("%u", pl.attr.level));
   y += 10;
   StatusInfo(g, y, L(sl_hp), StrParam("%i/%i", pl.health, pl.maxhealth));
   y += 10;

   if(pl.pclass & pcl_magicuser) {
      StatusInfo(g, y, L(sl_mp), StrParam("%i/%i", pl.mana, pl.manamax));
      y += 10;
   }

   StatusInfo(g, y, L(sl_exp),  StrParam("%u", pl.attr.exp));
   y += 10;
   StatusInfo(g, y, L(sl_next), StrParam("%u", pl.attr.expnext));
   y += 10;

   y += pl.pclass & pcl_magicuser ? 20 : 30;

   if(pl.attr.points) {
      PrintTextFmt(LC(LANG "STATUS_LEVELUP"), pl.attr.points);
      PrintText(sf_smallfnt, g->defcr, g->ox+7,1, g->oy+y,1);
   }

   y += 10;

   for(i32 i = 0; i < at_max; i++, y += 10)
      DrawAttr(g, 40, y, i);
}

/* EOF */
