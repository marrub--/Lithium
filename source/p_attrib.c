// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Attribute handling.                                                      │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "w_world.h"

static
void AttrBar(struct gui_state *g, i32 x, i32 y, i32 w, str gfx) {
   G_Clip(g, g->ox+x, g->oy+y, w * 4, 8);
   PrintSprite(gfx, g->ox+x,1, g->oy+y,1);
   G_ClipRelease(g);
}

static
void DrawAttr(struct gui_state *g, i32 x, i32 y, i32 at) {
   i32  attr = pl.attr.attrs[at];
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

   if(attr > ATTR_VIS_MAX) {
      i32 vatr = attr - ATTR_VIS_MAX;
      AttrBar(g, x, y, (vatr / (k64)ATTR_VIS_DIFF) * ATTR_VIS_MAX, sp_UI_AttrBar4);
      helptrns += 0.3;
   }

   PrintTextA_str(ns(lang(fast_strdup2(LANG "ATTR_HELP_", name))), sf_smallfnt, g->defcr, g->ox+x+1,1, g->oy+y,1, helptrns);

   ACS_BeginPrint();
   ACS_PrintInt(attr);
   ACS_PrintChar('/');
   ACS_PrintInt(ATTR_VIS_MAX);
   PrintText(sf_lmidfont, g->defcr, g->ox+x+202,1, g->oy+y,1);
}

static
void StatusInfo(struct gui_state *g, i32 y, str left, str right) {
   PrintText_str(left,  sf_lmidfont, g->defcr, g->ox+ 17,1, g->oy+y,1);
   PrintText_str(right, sf_smallfnt, g->defcr, g->ox+267,2, g->oy+y,1);
}

void P_CBI_TabStatus(struct gui_state *g) {
   i32 y = 27;

   PrintText_str(pl.name, sf_lmidfont, g->defcr, g->ox+17,1, g->oy+y,1);
   y += 10;

   StatusInfo(g, y, ns(lang(sl_status_cl)), pl.classname);
   y += 10;

   StatusInfo(g, y, ns(lang(sl_status_lv)), strp(_p(pl.attr.level)));
   y += 10;
   StatusInfo(g, y, ns(lang(sl_status_hp)), strp(_p((i32)pl.health), _c('/'), _p(pl.maxhealth)));
   y += 10;

   if(pl.pclass & pcl_magicuser) {
      StatusInfo(g, y, ns(lang(sl_status_mp)), strp(_p((i32)pl.mana), _c('/'), _p((i32)pl.manamax)));
      y += 10;
   }

   StatusInfo(g, y, ns(lang(sl_status_xp)), strp(_p(pl.attr.exp)));
   y += 10;
   StatusInfo(g, y, ns(lang(sl_status_nx)), strp(_p(pl.attr.expnext)));
   y += 10;

   y += pl.pclass & pcl_magicuser ? 20 : 30;

   if(pl.attr.points) {
      PrintTextFmt(tmpstr(lang(sl_status_levelup)), pl.attr.points);
      PrintText(sf_smallfnt, g->defcr, g->ox+7,1, g->oy+y,1);
   }

   y += 10;

   for(i32 i = 0; i < at_max; i++, y += 10) {
      DrawAttr(g, 40, y, i);
   }
}

/* EOF */
