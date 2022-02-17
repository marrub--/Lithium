// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Menu stuff.                                                              │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "w_world.h"

#define section_beg() ACS_BeginPrint()
#define section_end() SCallI(so_CreditsMenu, sm_AddSection, ACS_EndStrParam())
#define ch(c) (ACS_PrintChar(c))
#define cr(c) (ch('\C'), ch(c))
#define skip_ws() while(*inp && *inp == ' ') ++inp
#define rest() while(*inp && *inp != '\n') ch(*inp++)

alloc_aut(0) script_str ext("ACS") addr(OBJ "OpenCreditsMenu")
void Sc_OpenCreditsMenu() {
   astr inp = ns(lang(sl_credits));

   section_beg();

   while(*inp) {
      skip_ws();

      switch(*inp++) {
      case '~':
         section_end();
         section_beg();
         break;
      case '-':
         skip_ws();
         cr('k');
         rest();
         cr('-');
         break;
      case '*':
         skip_ws();
         cr('n');
         while(*inp && !(inp[0] == ' ' && inp[1] == '*')) ch(*inp++);
         cr('-');
         break;
      case '@':
         skip_ws();
         cr('g');
         while(*inp && !(inp[0] == ' ' && (inp[1] == ' ' || inp[1] == '-'))) {
            ch(*inp++);
         }
         skip_ws();
         cr('-'); ch(' '); ch(':'); ch(':');
         if(*inp++ == '\\') ch('\n');
         else               ch(' ');
         skip_ws();
         cr('i');
         rest();
         cr('-');
         break;
      case '|':
         skip_ws();
         cr('i');
         rest();
         cr('-');
         break;
      default:
         --inp;
         rest();
         break;
      }
      while(*inp && *inp != '\n') ++inp;
      ++inp;

      ch('\n');
   }

   section_end();
}

/* EOF */
