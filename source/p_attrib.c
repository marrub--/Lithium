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

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"

enum {
   _attr_skill_max = 50,
   _attr_abs_max   = 150,
};

char lvupstr[130];
i32  lvupstrn;

cstr attr_name(i32 which) {
   switch(which) {
   case at_acc: return "ACC";
   case at_def: return "DEF";
   case at_str: return "STR";
   case at_vit:
      if(get_bit(pcl_robot, pl.pclass) || get_bit(pcl_nonhuman, pl.pclass)) {
         return "POT";
      } else {
         return "VIT";
      }
   case at_stm:
      /**/ if(get_bit(pcl_robot,    pl.pclass)) return "REP";
      else if(get_bit(pcl_nonhuman, pl.pclass)) return "REG";
      else                                      return "STM";
   case at_luk: return "LUK";
   case at_spc:
      switch(pl.pclass) {
      case pcl_marine:    return "RGE"; break;
      case pcl_cybermage: return "CON"; break;
      case pcl_informant: return "ADR"; break;
      case pcl_wanderer:  return "AGI"; break;
      case pcl_assassin:  return "RSH"; break;
      case pcl_darklord:  return "REF"; break;
      case pcl_thoth:     return "MNA"; break;
      }
   }
   return "???";
}

void attr_draw(void) {
   if(pl.old.attr.level && pl.old.attr.level < pl.attr.level) {
      AmbientSound(ss_player_levelup, 1.0);
      P_LogH(1, tmpstr(sl_log_levelup), ACS_Random(1000, 9000));
   }
   if(lvupstr[0]) {
      SetSize(320, 240);
      BeginPrintStrN(lvupstr, lvupstrn);
      PrintText(sf_smallfnt, CR_WHITE, pl.hudrpos-100,1, 75,1);
   }
}

alloc_aut(0) script
static void attr_printlvup(i32 *attr, char **attrptrs) {
   i32 level = pl.attr.level;
   for(i32 i = 0; i < 35 * 5; ++i) {
      if(level != pl.attr.level) {
         /* a new levelup started, so exit */
         return;
      }
      char **ptr = &attrptrs[i / (35 * 5 / at_max)];
      while(!*ptr) ptr++;
      lvupstrn = *ptr - lvupstr;
      ACS_Delay(1);
   }
   lvupstr[0] = '\0';
}

void attr_giveexp(i32 amt) {
   noinit static i32 attr[at_max];
   noinit static char *attrptrs[at_max];
   fastmemset(attr, 0, sizeof attr);
   i32 levelup = 0;
   while(pl.attr.exp + amt >= pl.attr.expnext) {
      i32 expcurve = 20 + 2 * pl.attr.level;
      pl.attr.expprev = pl.attr.expnext;
      pl.attr.expnext = _base_exp + expcurve * expcurve * expcurve;
      pl.attr.level++;
      i32 pts = 7;
      switch(CVarGetI(sc_player_lvsys)) {
      case atsys_manual:
         pl.attr.points += 7;
         break;
      case atsys_hybrid:
         pl.attr.points += 2;
         pts       -= 2;
      case atsys_auto:
         for(i32 i = 0; i < pts; ++i) attr[ACS_Random(0, 100) % at_max]++;
         levelup++;
         break;
      }
   }
   if(levelup) {
      char *sp = lvupstr;
      fastmemset(attrptrs, 0, sizeof attrptrs);
      sp += sprintf(sp, "LEVEL %u", pl.attr.level);
      if(pl.attr.points) {
         sp += sprintf(sp, " (%u points)", pl.attr.points);
      }
      *sp++ = '\n';
      for(i32 i = 0; i < at_max; ++i) {
         pl.attr.attrs[i] += attr[i];
         if(attr[i]) {
            sp += sprintf(sp, "%s +%u (%u)\n",
                          attr_name(i), attr[i], pl.attr.attrs[i]);
            attrptrs[i] = sp;
         }
      }
      attr_printlvup(attr, attrptrs);
   }
   pl.attr.exp += amt;
}

static void AttrBar(struct gui_state *g, i32 x, i32 y, i32 w, str gfx) {
   PrintSpriteClip(gfx, g->ox+x,1, g->oy+y,1, 0,0,mini(w*4, 200),7);
}

stkoff k32 attr_accbuff(void) {
   return 1.0k + pl.attr.attrs[at_acc] / 100.0k;
}

stkoff i32 attr_defbuff(void) {
   k32 def = pl.attr.attrs[at_def] / (_attr_abs_max / 0.88k);
   if(pl.pclass == pcl_darklord && pl.buttons & BT_CROUCH) {
      def += 0.11k;
   }
   return mini(def * 100, 100);
}

stkoff i32 attr_strbuff(void) {
   return pl.attr.attrs[at_str] << 1;
}

stkoff k32 attr_vitbuff(void) {
   return 1.0k + pl.attr.attrs[at_vit] / 80.0k;
}

stkoff i32 attr_stmbuff(void) {
   return pl.attr.attrs[at_stm] << 2;
}

stkoff i32 attr_stmtime(void) {
   return maxi(75 - pl.attr.attrs[at_stm], 10);
}

stkoff k32 attr_lukbuff(void) {
   return 1.0k + pl.attr.attrs[at_luk] / 77.7k;
}

stkoff k32 attr_rgebuff(void) {
   return pl.attr.attrs[at_spc] / 1000.0k;
}

stkoff k32 attr_conbuff(void) {
   return pl.attr.attrs[at_spc] / 1100.0k;
}

stkoff k32 attr_refbuff(void) {
   return pl.attr.attrs[at_spc] / 150.0k;
}

stkoff i32 attr_refchargebuff(void) {
   return clampi(pl.attr.attrs[at_spc] >> 1, 0, 100);
}

static struct {
   mem_size_t     n;
   struct fmt_arg fmt[4];
} args[at_max] = {
   [at_acc] = {1, {{_fmt_k32}}},
   [at_def] = {1, {{_fmt_i32}}},
   [at_str] = {1, {{_fmt_i32}}},
   [at_vit] = {1, {{_fmt_k32}}},
   [at_luk] = {1, {{_fmt_k32}}},
   [at_stm] = {2, {{_fmt_i32}, {_fmt_k32}}},
};

static void attr_guiattr(struct gui_state *g, i32 at, i32 yofs) {
   i32 const x = 40;
   i32 const y = yofs + at * 14;
   i32  attr = pl.attr.attrs[at];
   cstr name = attr_name(at);
   if(pl.attr.points && attr < _attr_abs_max &&
      G_Button_HId(g, at, .x = x-42 + gui_p.btnnext.w, y-2, Pre(btnnext),
                   .slide = true))
   {
      pl.attr.points--;
      pl.attr.attrs[at]++;
   }
   BeginPrintStrN(name, 3);
   PrintText(sf_lmidfont, g->defcr, g->ox+x-24,1, g->oy+y,1);
   PrintSprite(sp_UI_AttrBar1, g->ox+x,1, g->oy+y,1);
   AttrBar(g, x, y, attr, sp_UI_AttrBar2);
   if(attr > _attr_skill_max) {
      AttrBar(g, x, y, (attr - _attr_skill_max) / 2, sp_UI_AttrBar4);
   }
   ACS_BeginPrint();
   printfmt(tmpstr(lang(fast_strdup2(LANG "ATTR_HELP_", name))), args[at].n, args[at].fmt);
   PrintText(sf_smallfnt, g->defcr, g->ox+x+1,1, g->oy+y,1);
   ACS_BeginPrint();
   if(attr > _attr_skill_max) {_c('\C'); _c('n');}
   _p(mini(attr, _attr_skill_max));
   if(attr > _attr_skill_max) {
      _c('/');
      _p(_attr_skill_max);
      _l("\Cd +");
      _p(attr - _attr_skill_max);
      if(attr == _attr_abs_max) {
         _c('/');
         _p(_attr_abs_max - _attr_skill_max);
      }
   }
   PrintText(sf_lmidfont, g->defcr, g->ox+x+202,1, g->oy+y,1);
}

void attr_gui(struct gui_state *g, i32 yofs) {
   args[at_acc].fmt[0].val.k = attr_accbuff();
   args[at_def].fmt[0].val.i = attr_defbuff();
   args[at_str].fmt[0].val.i = attr_strbuff();
   args[at_stm].fmt[0].val.i = attr_stmbuff();
   args[at_stm].fmt[1].val.k = attr_stmtime() / 35.0k;
   args[at_vit].fmt[0].val.k = attr_vitbuff();
   args[at_luk].fmt[0].val.k = attr_lukbuff();
   switch(pl.pclass) {
   case pcl_marine:
      args[at_spc].n            = 1;
      args[at_spc].fmt[0].tag   = _fmt_k32;
      args[at_spc].fmt[0].val.k = attr_rgebuff();
      break;
   case pcl_cybermage:
      args[at_spc].n            = 1;
      args[at_spc].fmt[0].tag   = _fmt_k32;
      args[at_spc].fmt[0].val.k = attr_conbuff();
      break;
   case pcl_darklord:
      args[at_spc].n            = 2;
      args[at_spc].fmt[0].tag   = _fmt_k32;
      args[at_spc].fmt[0].val.k = attr_refchargebuff() / 35.0k;
      args[at_spc].fmt[1].tag   = _fmt_k32;
      args[at_spc].fmt[1].val.k = attr_refbuff();
      break;
   }
   for(i32 i = 0; i < at_max; i++) {
      attr_guiattr(g, i, yofs);
   }
}

/* EOF */
