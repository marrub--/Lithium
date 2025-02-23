// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Tutorial GUI.                                                            │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"

static str tutorials[] = {
   s"ModdedEnemies",
   s"EnemyBehaviour",
   s"MarineItem",
   s"MarineMove",
   s"CyberMageWeapon",
   s"CyberMageItem",
   s"CyberMageMove",
   s"DarkLordWeapon",
   s"DarkLordItem",
   s"DarkLordMove",
   s"Story",
};

static i32 cur_page;

static void stat_text(struct gui_state *g, i32 x, i32 y, str left, str right) {
   PrintText_str(left,  sf_lmidfont, g->defcr, g->ox+x,1,     g->oy+y,1);
   PrintText_str(right, sf_smallfnt, g->defcr, g->ox+x+120,2, g->oy+y,1);
}

void P_CBI_TabStatus(struct gui_state *g) {
   PrintText_str(pl.name, sf_lmidfont, g->defcr, g->ox+7,1, g->oy+27,1);
   stat_text(g, 7, 37, sl_status_cl, pl.classname);
   stat_text(g, 7, 47, sl_status_hp, strp(_p((i32)pl.health), _c('/'), _p(pl.maxhealth)));
   if(get_bit(pcl_magicuser, pl.pclass)) {
      stat_text(g, 7, 57, sl_status_mp, strp(_p((i32)pl.mana), _c('/'), _p((i32)pl.manamax)));
   }
   stat_text(g, 147, 37, sl_status_lv, strp(_p(pl.attr.level)));
   stat_text(g, 147, 47, sl_status_xp, strp(_p(pl.attr.exp)));
   stat_text(g, 147, 57, sl_status_nx, strp(_p(pl.attr.expnext)));
   if(pl.attr.points) {
      BeginPrintFmt(tmpstr(sl_status_levelup), pl.attr.points);
      PrintText(sf_smallfnt, g->defcr, g->ox+7,1, g->oy+67,1);
   }
   attr_gui(g, 77);
}

stkoff static str tut_name(i32 which, cstr suf) {
   return strp(_l(LANG "TUT_"), _p(tutorials[which]), _p(suf));
}

void P_CBI_TabTuts(struct gui_state *g) {
   G_ScrBeg(g, &pl.cbi.st.tutscr, 2, 23, gui_p.btnlist.w, 190, gui_p.btnlist.h * countof(tutorials));

   for(i32 i = 0; i < countof(tutorials); ++i) {
      i32 y = gui_p.btnlist.h * i;

      if(G_ScrOcc(g, &pl.cbi.st.tutscr, y, gui_p.btnlist.h)) {
         continue;
      }

      if(G_Button_HId(g, i, tmpstr(lang(tut_name(i, "_NAM"))), 0, y, .color = i == cur_page ? CR_ORANGE : -1, Pre(btnlist))) {
         cur_page = i;
      }
   }

   G_ScrEnd(g, &pl.cbi.st.tutscr);

   G_Clip(g, 97, 27, 190, gui_p.scrdef.scrlh * 23, 184);
   g->ox += 97;
   g->oy += 27;

   enum {
      _cb_hilight,
      _cb_bind,
      _cb_esc,
      _cb_end,
      _cb_sol,
   };

   enum {
      _pal_default,
      _pal_list,
      _pal_head,
      _pal_warn,
   };

   static struct {
      i32 def;
      i32 hi;
      i32 bind;
   } const crs[] = {
      [_pal_default] = {.def = CR_WHITE,   .hi = 'd',.bind = 'v'},
      [_pal_list]    = {.def = CR_ICE,     .hi = 'q',.bind = 'n'},
      [_pal_head]    = {.def = CR_SAPPHIRE,.hi = 'p',.bind = 'h'},
      [_pal_warn]    = {.def = CR_ORANGE,  .hi = 'p',.bind = 'h'},
   };

   str page = lang(tut_name(cur_page, "_DSC"));
   i32 pal = _pal_default, y = 0, cb = dst_bit(_cb_sol), which = 0;
   ACS_BeginPrint();
   for(astr eol = page; !get_bit(cb, _cb_end); ++eol) {
      switch(*eol) {
      case '\0':
         set_bit(cb, _cb_end);
      case '\n': {
         str p = ACS_EndStrParam();
         if(p[0] != '\0') {
            struct i32v2 s;
            TextSize(&s, p, sf_smallfnt, 184);
            i32 x  = pal >= _pal_head ? g->ox + 92 : g->ox;
            i32 xa = pal >= _pal_head ? 4 : 1;
            str fn = pal == _pal_head ? sf_lmidfont : sf_smallfnt;
            PrintText_str(p, fn, crs[pal].def, x,xa, g->oy + y,1);
            y += s.y;
         } else {
            ACS_EndStrParam();
            y += 8;
         }
         if(!get_bit(cb, _cb_end)) {
            ACS_BeginPrint();
            pal = _pal_default;
         }
         set_bit(cb, _cb_sol);
         continue;
      }
      case '\\':
         set_bit(cb, _cb_esc);
         continue;
      case '{':
         if(!get_bit(cb, _cb_esc) && !get_bit(cb, _cb_bind)) {
            set_bit(cb, _cb_bind);
            ACS_PrintChar('\C');
            ACS_PrintChar(crs[pal].bind);
            ACS_BeginPrint();
         }
         break;
      case '}':
         if(!get_bit(cb, _cb_esc) && get_bit(cb, _cb_bind)) {
            ACS_PrintBind(ACS_EndStrParam());
            ACS_PrintChar('\C');
            ACS_PrintChar('-');
            dis_bit(cb, _cb_bind);
         }
         break;
      case '<':
         if(!get_bit(cb, _cb_esc) && !get_bit(cb, _cb_hilight)) {
            set_bit(cb, _cb_hilight);
            ACS_PrintChar('\C');
            ACS_PrintChar(crs[pal].hi);
         }
         break;
      case '>':
         if(!get_bit(cb, _cb_esc) && get_bit(cb, _cb_hilight)) {
            ACS_PrintChar('\C');
            ACS_PrintChar('-');
            dis_bit(cb, _cb_hilight);
         }
         break;
      case '*':
         pal = _pal_list;
         PrintStrL(u8" ×");
         break;
      case '-':
         if(get_bit(cb, _cb_sol)) {
            pal = _pal_head;
            ++eol;
            PrintStr(alientext(which++));
            ACS_PrintChar(' ');
            break;
         }
         goto print;
      case '!':
         if(get_bit(cb, _cb_sol)) {
            pal = _pal_warn;
            ++eol;
            break;
         }
         goto print;
      print:
      default:
         ACS_PrintChar(*eol);
         break;
      }
      dis_bit(cb, _cb_esc);
      dis_bit(cb, _cb_sol);
   }

done:
   g->oy -= 27;
   g->ox -= 97;
   G_ClipRelease(g);
}

/* EOF */
