/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Zoe Elsie Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Cheats.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "w_world.h"
#include "p_player.h"

script static
void print_cheat_msg(cstr msg) {
   pl.logH(1, "%s", msg);
}

stkcall alloc_aut(0) static
i32 check_cheat(struct cheat *cht, i32 ch) {
   if(cht->state.chrnum < cht->len) {
      if(cht->seq[cht->state.chrnum] == ch) cht->state.chrnum++;
      else                                  cht->state.chrnum = 0;
      cht->state.parnum = 0;
   } else if(cht->state.parnum < cht->par) {
      cht->state.params[cht->state.parnum] = ch;
      cht->state.parnum++;
   }

   if(cht->state.chrnum == cht->len && cht->state.parnum == cht->par) {
      cht->state.chrnum = 0;
      cht->state.parnum = 0;
      if(cht->fun(cht->state.params)) {
         print_cheat_msg(cht->msg);
         return 1;
      } else {
         return 0;
      }
   } else if(cht->state.chrnum > 2 || cht->state.parnum > 0) {
      return 2;
   } else {
      return 0;
   }
}

alloc_aut(0) script ext("ACS") addr(lsc_cheatinput)
bool Sc_CheatInput(i32 ch) {
   if(P_None()) return false;
   i32  res;
   bool grab = false;
#define cheat_x(name) \
   res = check_cheat(&name, ch); \
   if(res == 1) return true; \
   if(res == 2) grab = true;
#include "m_cheat.h"
   return grab;
}

/* EOF */
