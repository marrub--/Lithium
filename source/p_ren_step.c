// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "p_hudid.h"

#define step_snd_def(tex, snd, delay) {s"" tex, s"*step-" snd, delay}

struct step_snd {
   str nam, snd;
   i32 nxt;
};

static struct step_snd const sounds[] = {
   step_snd_def("",        "normal", 10),
   step_snd_def("FWATER1", "water",  11),
   step_snd_def("FWATER2", "water",  11),
   step_snd_def("FWATER3", "water",  11),
   step_snd_def("FWATER4", "water",  11),
   step_snd_def("BLOOD1",  "water",  15),
   step_snd_def("BLOOD2",  "water",  15),
   step_snd_def("BLOOD3",  "water",  15),
   step_snd_def("NUKAGE1", "slime",  15),
   step_snd_def("NUKAGE2", "slime",  15),
   step_snd_def("NUKAGE3", "slime",  15),
   step_snd_def("SLIME01", "slime",  12),
   step_snd_def("SLIME02", "slime",  12),
   step_snd_def("SLIME03", "slime",  12),
   step_snd_def("SLIME04", "slime",  12),
   step_snd_def("SLIME05", "slime",  12),
   step_snd_def("SLIME06", "slime",  12),
   step_snd_def("SLIME07", "slime",  12),
   step_snd_def("SLIME08", "slime",  12),
   step_snd_def("LAVA1",   "lava",   17),
   step_snd_def("LAVA2",   "lava",   17),
   step_snd_def("LAVA3",   "lava",   17),
   step_snd_def("LAVA4",   "lava",   17),
};

script void P_Ren_Step(void) {
   if(Paused) {
      return;
   }
   if(!pl.nextstep) {
      str flr = ACS_GetActorFloorTexture(0);
      struct step_snd const *snd = &sounds[0];
      for(i32 i = 1; i < countof(sounds); ++i) {
         if(flr == sounds[i].nam) {
            snd = &sounds[i];
            break;
         }
      }
      pl.nextstep = snd->nxt;
      k32 vel = fastabsk(pl.getVel());
      k32 vol = CVarGetK(sc_player_footstepvol) * mink(vel / 24.0k, 1);
      if(vol > 0.07k && pl.onground) {
         if(GetMembI(0, sm_Sprinting) && vel >= 8) {
            StartSound(ss_player_step_sprint, lch_step2, 0, vol * 2.0k);
         }
         StartSound(snd->snd, lch_step1, 0, vol);
      }
   } else {
      pl.nextstep--;
   }
}

/* EOF */
