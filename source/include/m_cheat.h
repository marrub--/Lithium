/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Cheats.
 *
 * ---------------------------------------------------------------------------|
 */

#if defined(cheat_x)
cheat_x(cht_discount)
cheat_x(cht_end_game)
cheat_x(cht_give_ammo)
cheat_x(cht_give_exp)
cheat_x(cht_give_exp_to)
cheat_x(cht_give_score)
cheat_x(cht_run_dlg)
cheat_x(cht_run_prg)
cheat_x(cht_run_trm)
cheat_x(cht_summon_boss)

#ifndef NDEBUG
cheat_x(cht_dbg_dlg)
cheat_x(cht_dbg_dump_alloc)
cheat_x(cht_dbg_font_test)
cheat_x(cht_dbg_mons_info)
#endif

#undef cheat_x
#elif !defined(m_cheat_h)
#define m_cheat_h

#define cheat_s(seq, par, cb) {seq, sizeof(seq) - 1, par, cb}

typedef char cheat_params_t[8];

script funcdef bool (*cheat_cb_t)(cheat_params_t const cht);

struct cheat {
   cstr       seq;
   mem_size_t len;
   mem_size_t par;
   cheat_cb_t fun;

   struct {
      mem_size_t     chrnum;
      mem_size_t     parnum;
      cheat_params_t params;
   } state;
};

#define cheat_x(name) extern struct cheat name;
#include "m_cheat.h"

#endif
