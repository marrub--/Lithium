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

#include "common.h"
#include "p_player.h"
#include "w_monster.h"

#include <stdio.h>
#include <GDCC.h>

#ifndef NDEBUG
str dbgstat[64], dbgnote[64];
i32 dbgstatnum,  dbgnotenum;

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

   PrintChrLi("\nEOF\n");
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

   PrintChrLi("\nEOF\n");
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

   if(!P_None()) {
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

         G_UpdateState(&g);

         G_WinBeg(&g, &win, .preset = &pre);

         G_WinEnd(&g, &win);

         G_End(&g, gui_curs_outlineinv);

         ACS_Delay(1);
      }

      UnfreezeTime();
   }
}

script static
void dbg_font_test(str font) {
   static
   struct {cstr lhs, rhs;} const strings[] = {
      /*
      "WidTest", u8"0123456789ABCDEFGHIJKLMNOPQ",
      */
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

   static
   cstr const pangrams[] = {
      u8"Falsches Üben von Xylophonmusik quält jeden größeren Zwerg",
      u8"The quick brown fox jumps over the lazy dog",
      u8"Jovencillo emponzoñado de whisky: ¡qué figurota exhibe!",
      u8"Voix ambiguë d'un cœur qui au zéphyr préfère les jattes de kiwis",
      u8"Широкая электрификация южных подъёму сельского хозяйства",
   };

   for(;;) {
      enum {_h = 12};
      SetSize(320, 240);
      i32 y = 0;
      for(i32 i = 0; i < countof(strings); i++) {
         PrintLine(0, y, 70, y, 0xFF00FF);
         PrintLine(0, y, 0, y + _h, 0xFF00FF);
         PrintLine(70, y, 320, y, 0x00FFFF);
         PrintLine(70, y, 70, y + _h, 0x00FFFF);
         PrintTextChS(strings[i].lhs);
         PrintTextX(font, CR_WHITE, 0,1, y,1, _u_no_unicode);
         PrintTextChS(strings[i].rhs);
         PrintTextX(font, CR_WHITE, 70,1, y,1, _u_no_unicode);
         y += _h;
      }
      SetClipW(0, 0, 320, 240, 320);
      PrintTextChS(pangrams[wl.ticks / 35 % countof(pangrams)]);
      PrintTextX(font, CR_WHITE, 0,1, y,1, _u_no_unicode);
      ClearClip();
      ACS_Delay(1);
   }
}

script static
bool chtf_dbg_font_test(cheat_params_t const params) {
   str font;
   switch(FourCC(params[0], params[1], 0, 0)) {
   case FourCC('a', 'n', 0, 0): font = sf_areaname;  break;
   case FourCC('b', 'u', 0, 0): font = sf_bigupper;  break;
   case FourCC('i', 't', 0, 0): font = sf_italic;    break;
   case FourCC('j', 'k', 0, 0): font = sf_jiskan16;  break;
   case FourCC('l', 'j', 0, 0): font = sf_ljtrmfont; break;
   case FourCC('l', 'm', 0, 0): font = sf_lmidfont;  break;
   case FourCC('l', 's', 0, 0): font = sf_lsmlhfnt;  break;
   case FourCC('l', 't', 0, 0): font = sf_ltrmfont;  break;
   case FourCC('m', 'g', 0, 0): font = sf_misakig;   break;
   case FourCC('m', 'm', 0, 0): font = sf_misakim;   break;
   case FourCC('s', 'f', 0, 0): font = sf_smallfnt;  break;
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
   P_Scr_GivePos(0, 0, SCR_MAX, true);
   return true;
}

script static
bool chtf_end_game(cheat_params_t const params) {
   i32 which;
   switch(params[0]) {
   case 'n': which = _finale_normal;      break;
   case 'i': which = _finale_icon_of_sin; break;
   case 't': which = _finale_time_out;    break;
   case 'd': which = _finale_division;    break;
   default:
      return false;
   }
   F_Start(which);
   return true;
}

struct cheat cht_give_exp_to = cheat_s("pgdonation", 2, chtf_give_exp_to, "Tuition donated to monster");
struct cheat cht_give_score  = cheat_s("pgbezos",    0, chtf_give_score, "Player transmuted into Jeff Bezos");
struct cheat cht_end_game    = cheat_s("pgbedone",   1, chtf_end_game, "...Be Done");

/* EOF */
