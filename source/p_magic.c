// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Magic weapon functions.                                                  │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "p_player.h"
#include "p_hudid.h"
#include "w_world.h"

script_str ext("ACS") addr(OBJ "Blade")
void Z_Blade(bool hit) {
   if(!pl.onground) P_SetVel(pl.velx / 2, pl.vely / 2, 0);
   pl.bladehit = hit;
   SetFade(fid_blade, 3, 24);
}

script_str ext("ACS") addr(OBJ "Rend")
void Z_Rend(bool hit, i32 set) {
   if(!pl.onground) P_SetVel(pl.velx / 2, pl.vely / 2, 0);
   pl.rendhit = hit;
   SetFade(fid_rendS + set, 3, 48);
}

dynam_aut script_str ext("ACS") addr(OBJ "Feuer")
void Z_Feuer(bool left, bool fire) {
   str actor = fire ? so_FeuerExplosion : so_FeuerTest;
   struct k32v3 t = trace_from(pl.yaw, pl.pitch, 1024, pl.attackheight);

   i32 sx = pl.x;
   i32 sy = pl.y;
   i32 sz = pl.z + 32;

   struct polar cpp = ctopol(t.x - sx, t.y - sy);
   cpp.dst /= 4;
   if(left) cpp.ang += 0.07;
   else     cpp.ang -= 0.07;

   i32 cx = sx + ACS_Cos(cpp.ang) * cpp.dst;
   i32 cy = sy + ACS_Sin(cpp.ang) * cpp.dst;
   k32 max = fire ? 20 : 70;

   for(i32 i = 0; i < max; i++) {
      struct i32v2 v = qbezieri(sx, sy, cx, cy, t.x, t.y, i / max);
      i32 tid;
      ACS_SpawnForced(actor, v.x, v.y, lerpk(sz, t.z, i / max), tid = ACS_UniqueTID());
      if(fire) {
         PtrSet(tid, AAPTR_DEFAULT, AAPTR_TARGET, pl.tid);
         ACS_Delay(1);
      }
   }

   if(fire) {
      i32 tid;
      ACS_SpawnForced(so_FeuerFinal, t.x, t.y, t.z, tid = ACS_UniqueTID());
      PtrSet(tid, AAPTR_DEFAULT, AAPTR_TARGET, pl.tid);
   }
}

dynam_aut script_str ext("ACS") addr(OBJ "Cercle")
void Z_Cercle(void) {
   struct k32v3 v = trace_from(pl.yaw, pl.pitch, 1024, pl.attackheight, true);

   FreezeTime();
   ACS_Delay(2); /* necessary so sounds may play */

   AmbientSound(ss_weapons_cercle_begin, 0.5k);

   i32 fxtid = ACS_UniqueTID();
   for(i32 i = 0; i < 100; i++) {
      k32 px = ACS_Cos(i / 100.0) * 77;
      k32 py = ACS_Sin(i / 100.0) * 77;
      i32 tid;

      ACS_SpawnForced(so_CircleParticle, v.x + px, v.y + py, v.z + 7, tid = ACS_UniqueTID());

      ACS_SetActorAngle(tid, i / 100.0);
      PtrSet(tid, AAPTR_DEFAULT, AAPTR_TARGET, pl.tid);
      ACS_Thing_ChangeTID(tid, fxtid);

      ACS_Delay((i & 1) * (i / 30.0));
   }

   ACS_Delay(35);

   i32 fxtid2 = ACS_UniqueTID();
   SetMembI(fxtid, sm_Trigger, true);
   SetMembI(fxtid, sm_FxTID, fxtid2);

   AmbientSound(ss_weapons_cercle_attack, 0.5k);

   ACS_Delay(35);
   SetMembI(fxtid2, sm_Trigger, true);

   ACS_Delay(7);
   SetMembI(fxtid2, sm_Trigger, true);

   ACS_Delay(35);

   i32 fxtid3 = ACS_UniqueTID();
   for(i32 i = 0; i < 3; i++) {
      k32 px = ACS_Cos(i / 3.0) * 60;
      k32 py = ACS_Sin(i / 3.0) * 60;
      i32 tid;

      ACS_SpawnForced(so_CircleSpearThrower, v.x + px, v.y + py, v.z + 24, tid = ACS_UniqueTID());

      ACS_SetActorAngle(tid, i / 3.0);
      PtrSet(tid, AAPTR_DEFAULT, AAPTR_TARGET, pl.tid);
      ACS_Thing_ChangeTID(tid, fxtid3);

      ACS_Delay(7);
   }

   ACS_Delay(10);

   /* NB: The projectiles take the TIDs of the throwers, so this is actually triggering them. */
   SetMembI(fxtid3, sm_Trigger, true);

   /* Just in case. */
   ACS_Thing_Remove(fxtid);
   ACS_Thing_Remove(fxtid2);

   UnfreezeTime();
}

dynam_aut script_str ext("ACS") addr(OBJ "MagicSelect")
void Z_MagicSelect(i32 num) {
   if(!CVarGetI(sc_weapons_magicselanims)) return;

   switch(num) {
   case 1: case 4: FadeFlash(255, 255, 0, 0.3, 0.3); break;
   case 2: case 3: FadeFlash(255, 155, 0, 0.3, 0.3); break;
   case 6: case 7: FadeFlash(255, 0,   0, 0.3, 0.3); break;
   }

   for(i32 i = 0; i < 4; i++) {
      for(i32 j = 0; j < 3; j++) {
         SetSize(320, 240);
         PrintSprite(sp_MagicSel_BG, 160,0, 120,0);
         SetSize(64, 64);
         PrintSprite(strp(_l(":MagicSel:Slot"), _p(num), _c('_'), _p(i + 1)), 0,1, 0,1, _u_alpha, 0.5);
         ACS_Delay(1);
      }
   }
}

/* EOF */
