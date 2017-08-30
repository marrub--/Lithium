// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

#define UData UData_Implying(upgr)

// Extern Functions ----------------------------------------------------------|

//
// Update
//
[[__call("ScriptS")]]
void Upgr_Implying_Update(player_t *p, upgrade_t *upgr)
{
   static __str strings[] = {
      "\Cd" ">implying",
      "\Cd" ">doombabbies",
      "\Cd" ">implying",
      "\Cd" ">doom shitters",
      "\Cd" ">>>>>>>clip",
      "\Cd" ">implying",
      "\Cj" "Report and ignore.",
      "\Cj" "caleb when?",
      "\Cd" ">implying",
      "\Cd" ">",
      "\Cd" ">>>",
      "\Cd" ">>>>>>",
      "\Cj" "is this compatible with brutal doom?",
      "\Cd" ">>>>>>>>>",
      "\Cd" ">>>>>>>>>>>>",
      "\Cd" ">>>>>>>>>>>>>>>",
      "\Cq" "<",
   };

   static int const num_strings = countof(strings);
   static int const id_max = hid_implyingE - hid_implyingS;

   int id = UData.hudid;
   for(int i = 0, n = ACS_Random(0, 40); i < n; i++)
   {
      id = ++id % id_max;

      HudMessageF("DBIGFONT", "%S", strings[ACS_Random(0, num_strings - 1)]);
      HudMessageFade(hid_implyingE + id,
                     ACS_RandomFixed(0.0, 1.0),
                     ACS_RandomFixed(0.0, 1.0),
                     ACS_RandomFixed(0.1, 0.4),
                     0.1);
   }

   UData.hudid = id;
}

// EOF

