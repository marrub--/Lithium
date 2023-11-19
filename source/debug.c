// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Debugging functions.                                                     │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "w_monster.h"

#include <stdio.h>
#include <GDCC.h>

#ifndef NDEBUG
void Dbg_PrintMemC(cps_t const *d, mem_size_t size) {
   i32 pos = 0;

   for(mem_size_t i = 0; i < size * 4; i++) {
      if(pos + 3 > 79) {
         ACS_PrintChar('\n');
         pos = 0;
      }

      mem_byte_t c = Cps_GetC(d, i);

      if(IsPrint(c)) {
         ACS_PrintChar(c);
         ACS_PrintChar(' ');
         ACS_PrintChar(' ');
      } else {
         if(c < 0x10) {
            ACS_PrintChar('0');
         }
         ACS_PrintHex(c);
         ACS_PrintChar(' ');
      }

      pos += 3;
   }

   PrintStrL("\nEOF\n");
}

void Dbg_PrintMem(void const *data, mem_size_t size) {
   mem_byte_t const *d = data;
   i32 pos = 0;

   for(mem_size_t i = 0; i < size; i++) {
      if(pos + 3 > 79) {
         ACS_PrintChar('\n');
         pos = 0;
      }

      if(IsPrint(d[i])) {
         ACS_PrintChar(d[i]);
         ACS_PrintChar(' ');
         ACS_PrintChar(' ');
      } else {
         if(d[i] < 0x10) {
            ACS_PrintChar('0');
         }
         ACS_PrintHex(d[i]);
         ACS_PrintChar(' ');
      }

      pos += 3;
   }

   PrintStrL("\nEOF\n");
}

dynam_aut script_str ext("ACS") addr(OBJ "Thingomamob")
void Z_Thimgomabjhdf(void) {
   static
   struct gui_pre_win const pre = {
      .bg = "Background",
      .w = 80,
      .h = 120,
      .a = 0.7k,
      .bx = 3,
      .by = 3,
   };

   if(pl.modal != _gui_none) return;

   pl.modal = _gui_waypoint;
   FreezeTime();

   struct gui_win win = {};

   struct gui_state g = {
      .cx = 320 / 2,
      .cy = 240 / 2,
      .gfxprefix = ":UI_Green:",
   };

   for(;;) {
      if(pl.dead) break;

      G_Begin(&g, 320, 240);

      G_WinBeg(&g, &win, .preset = &pre);

      G_WinEnd(&g, &win);

      G_End(&g, gui_curs_outlineinv);

      ACS_Delay(1);
   }

   UnfreezeTime();
}

script static
void dbg_font_test(str font) {
   static struct {cstr lhs, rhs;} const strings[] = {
      "L1SPnc1", u8"¡¢£¤¥¦§¨©ª«¬®¯°±²³´µ¶·¸¹º»¼",
      "L1SPnc2", u8"½¾¿×÷µ",
      "L1SLet1", u8"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛ",
      "L1SLet2", u8"ÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö",
      "L1SLet3", u8"øùúûüýþÿ",
      "LExtA",   u8"ĀāĒēĪīŌōŒœŪūſ",
      "GenPnc1", u8"‒–—‘’‚‛“”„‟†‡•‣․‥…‧‰‱′″‴‵‶‷",
      "GenPnc2", u8"‹›※‼‽⁂⁃⁄⁅⁆⁇⁈⁉⁊⁋⁌⁍⁎⁏⁑⁒⁓⁕⁖⁗⁘⁙",
      "GenPnc3", u8"⁚⁛⁜⁝⁞",
      "CyrMj1",  u8"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩ",
      "CyrMj2",  u8"ЪЫЬЭЮЯ",
      "CyrMi1",  u8"абвгдеёжзийклмнопрстуфхцчшщ",
      "CyrMi2",  u8"ъыьэюя",
      "Nums",    u8"0123456789",
      "AlphaU",  u8"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
      "AlphaL",  u8"abcdefghijklmnopqrstuvwxyz",
      "Extra",   u8"",
   };
   static cstr const pangrams[] = {
      u8"Falsches Üben von Xylophonmusik quält jeden größeren Zwerg",
      u8"The quick brown fox jumps over the lazy dog",
      u8"Jovencillo emponzoñado de whisky: ¡qué figurota exhibe!",
      u8"Voix ambiguë d'un cœur qui au zéphyr préfère les jattes de kiwis",
      u8"Широкая электрификация южных подъёму сельского хозяйства",
      u8"いろはにほへと　ちりぬるを / 色は匂へど　散りぬるを",
      u8"わかよたれそ　つねならむ / 我が世誰ぞ　常ならむ",
      u8"うゐのおくやま　けふこえて / 有為の奥山　今日越えて",
      u8"あさきゆめみし　ゑひもせす / 浅き夢見じ　酔ひもせず",
   };
   for(;;) {
      enum {_h = 12};
      SetSize(320, 240);
      i32 y = 0;
      for(i32 i = 0; i < countof(strings); i++) {
         PrintLine(0, y, 70, y, 0xFFFF00FF);
         PrintLine(0, y, 0, y + _h, 0xFFFF00FF);
         PrintLine(70, y, 320, y, 0xFF00FFFF);
         PrintLine(70, y, 70, y + _h, 0xFF00FFFF);
         BeginPrintStr(strings[i].lhs);
         PrintText(font, CR_WHITE, 0,1, y,1, _u_no_unicode);
         BeginPrintStr(strings[i].rhs);
         PrintText(font, CR_WHITE, 70,1, y,1, _u_no_unicode);
         y += _h;
      }
      BeginPrintStr(pangrams[ACS_Timer() / 35 % countof(pangrams)]);
      PrintText(font, CR_WHITE, 0,1, y,1, _u_no_unicode, 0, 320);
      ACS_Delay(1);
   }
}

script static
bool chtf_dbg_font_test(cheat_params_t const params) {
   str font;
   switch(fourcc(params[0], params[1], 0, 0)) {
   case fourcc('a', 'n', 0, 0): font = sf_lareanam; break;
   case fourcc('h', 's', 0, 0): font = sf_lsmlhfnt; break;
   case fourcc('j', 's', 0, 0): font = sf_ljsmlfnt; break;
   case fourcc('j', 't', 0, 0): font = sf_ljtrmfnt; break;
   case fourcc('l', 'b', 0, 0): font = sf_bigupper; break;
   case fourcc('l', 'i', 0, 0): font = sf_litalics; break;
   case fourcc('l', 'm', 0, 0): font = sf_lmidfont; break;
   case fourcc('l', 's', 0, 0): font = sf_smallfnt; break;
   case fourcc('l', 't', 0, 0): font = sf_ltrmfont; break;
   default:
      return false;
   }
   dbg_font_test(font);
   return true;
}

script static
bool chtf_dbg_dump_alloc(cheat_params_t const params) {
   __GDCC__alloc_dump();
   return true;
}

script static
bool chtf_dbg_mons_info(cheat_params_t const params) {
   pl.setActivator();
   ACS_SetActivator(0, AAPTR_PLAYER_GETTARGET);
   dmon_t *m = DmonSelf();
   if(m) PrintMonsterInfo(m);
   return true;
}

struct cheat cht_dbg_dump_alloc = cheat_s("pgsegv",    0, chtf_dbg_dump_alloc, "Segfault triggered");
struct cheat cht_dbg_font_test  = cheat_s("pgfontdbg", 2, chtf_dbg_font_test, "Player baptized");
struct cheat cht_dbg_mons_info  = cheat_s("pginfo",    0, chtf_dbg_mons_info, "Monster doxxed");
#endif

script static
bool chtf_give_exp_to(cheat_params_t const params) {
   if(!IsDigit(params[0]) || !IsDigit(params[1])) {
      return false;
   }

   pl.setActivator();
   ACS_SetActivator(0, AAPTR_PLAYER_GETTARGET);
   dmon_t *m = DmonSelf();
   if(m) m->exp += (params[0] - '0') * 1000 + (params[1] - '0') * 100;
   return true;
}

script static
bool chtf_give_score(cheat_params_t const params) {
   pl.setActivator();
   P_Scr_GivePos(0, 0, SCR_MAX / (score_t)(params[0] - '0'), true);
   return true;
}

script static
bool chtf_end_game(cheat_params_t const params) {
   i32 which;
   switch(params[0]) {
   case 'n': which = _finale_normal;   break;
   case 't': which = _finale_time_out; break;
   case 'd': which = _finale_division; break;
   default:
      return false;
   }
   F_Start(which);
   return true;
}

struct cheat cht_give_exp_to = cheat_s("pgdonation", 2, chtf_give_exp_to, "Tuition donated to monster");
struct cheat cht_give_score  = cheat_s("pgbezos",    1, chtf_give_score, "Player transmuted into Jeff Bezos");
struct cheat cht_end_game    = cheat_s("pgbedone",   1, chtf_end_game, "...Be Done");

void P_Ren_Debug(void) {
   if(!dbglevel(log_devh)) {
      return;
   }
   SetSize(640, 480);
   str sky1 = EDataS(_edt_sky1), sky2 = EDataS(_edt_sky2);
   ACS_BeginPrint();
   _l("mission%: "); _p(ml.missionprc); _c('\n');
   _l("exp: lv.");
   _p(pl.attr.level); _c(' '); _p(pl.attr.expprev); _c('/');
   _p(pl.attr.exp);   _c('/'); _p(pl.attr.expnext); _c('\n');
   _l("pos: ");        _v(pl.x, pl.y, pl.z);                   _c('\n');
   _l("vel: ");        _v(pl.velx, pl.vely, pl.velz);          _c('\n');
   _l("v: ");          _p(pl.getVel());                        _c('\n');
   _l("ang: ");        _v(pl.yaw, pl.pitch, pl.roll);          _c('\n');
   _l("addang: ");     _v(pl.addyaw, pl.addpitch, pl.addroll); _c('\n');
   _l("rage: ");       _p(pl.rage);                            _c('\n');
   _l("score: ");      _p(pl.score); _c('*'); _p(pl.scoremul); _c('\n');
   _l("speedmul: ");   _p(pl.speedmul);                        _c('\n');
   _l("jumpboost: ");  _p(pl.jumpboost);                       _c('\n');
   _l("sky: ");        _p(sky1); _c(','); _p(sky2);            _c('\n');
   _l("*target: ");    _p(pl.light.target);                    _c('\n');
   _l("*intensity: "); _p(pl.light.intensity);                 _c('\n');
   _l("*speed: ");     _p(pl.light.speed);                     _c('\n');
   _l("*battery: ");   _p(pl.light.battery);                   _c('\n');
   if(get_bit(wl.cbiupgr, cupg_d_shield)) {
      _l("shield: ");     _p((i32)pl.shield);  _c('\n');
      _l("shieldmax: ");  _p(pl.shieldmax);    _c('\n');
      _l("regen: ");      _p(pl.regenwait);    _c('\n');
      _l("regenmax: ");   _p(pl.regenwaitmax); _c('\n');
   }
   _l("scorethreshold: "); _p(wl.scorethreshold); _c('\n');
   _l("realtime: "); _p(wl.realtime); _c('\n');
   PrintText(sf_smallfnt, CR_WHITE, pl.hudlpos*2,1, 240,0, _u_alpha, 0.2);
}

/* EOF */
