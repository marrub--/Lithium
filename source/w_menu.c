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

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"

#define section_beg() ACS_BeginPrint()
#define section_end() SCallV(so_CreditsMenu, sm_AddSection, ACS_EndStrParam())
#define skip_ws() while(*inp && *inp == ' ') ++inp
#define rest() while(*inp && *inp != '\n') _c(*inp++)

alloc_aut(0) script_str ext("ACS") addr(OBJ "OpenCreditsMenu")
void Z_OpenCreditsMenu(void) {
   astr inp = sl_credits;
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
         _l("\Ck");
         rest();
         _l("\C-");
         break;
      case '*':
         skip_ws();
         _l("\Cn");
         while(*inp && !(inp[0] == ' ' && inp[1] == '*')) _c(*inp++);
         _l("\C-");
         break;
      case '@':
         skip_ws();
         _l("\Cg");
         while(*inp && !(inp[0] == ' ' && (inp[1] == ' ' || inp[1] == '-'))) {
            _c(*inp++);
         }
         skip_ws();
         _l("\C- ::");
         if(*inp++ == '\\') _c('\n');
         else               _c(' ');
         skip_ws();
         _l("\Ci");
         rest();
         _l("\C-");
         break;
      case '|':
         skip_ws();
         _l("\Ci");
         rest();
         _l("\C-");
         break;
      default:
         --inp;
         rest();
         break;
      }
      while(*inp && *inp != '\n') ++inp;
      ++inp;
      _c('\n');
   }
   section_end();
}

/* EOF */
