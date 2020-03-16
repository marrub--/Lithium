/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Implying upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

/* Extern Functions -------------------------------------------------------- */

script
void Upgr_Implying_Update(struct player *p, struct upgrade *upgr)
{
   StrAry(strings,
      s"\Cd>implying",
      s"\Cd>doombabbies",
      s"\Cd>implying",
      s"\Cd>doom shitters",
      s"\Cd>>>>>>>clip",
      s"\Cd>implying",
      s"\CjReport and ignore.",
      s"\Cjcaleb when?",
      s"\Cd>implying",
      s"\Cd>",
      s"\Cd>>>",
      s"\Cd>>>>>>",
      s"\Cjis this compatible with brutal doom?",
      s"\Cd>>>>>>>>>",
      s"\Cd>>>>>>>>>>>>",
      s"\Cd>>>>>>>>>>>>>>>",
      s"\Cd>doom >2012",
      s"\Cq<",
   );

   srand(p->ticks / 7);

   for(i32 i = 0; i < 40; i++)
      PrintTextX_str(strings[rand() % countof(strings)], s_bigupper, 0, rand() % 320,0, rand() % 240,0, ptf_no_utf);
}

/* EOF */
