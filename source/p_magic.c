/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Magic weapon functions.
 *
 * ---------------------------------------------------------------------------|
 */

#include "p_player.h"
#include "p_hudid.h"
#include "w_world.h"

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr("Lith_Blade")
void Sc_Blade(bool hit)
{
   with_player(LocalPlayer) {
      if(!p->onground) P_SetVel(p, p->velx / 2, p->vely / 2, 0);
      p->bladehit = hit;
      SetFade(fid_blade, 3, 24);
   }
}

script_str ext("ACS") addr("Lith_Rend")
void Sc_Rend(bool hit, i32 set)
{
   with_player(LocalPlayer) {
      if(!p->onground) P_SetVel(p, p->velx / 2, p->vely / 2, 0);
      p->rendhit = hit;
      SetFade(fid_rendS + set, 3, 48);
   }
}

script_str ext("ACS") addr("Lith_Feuer")
void Sc_Feuer(bool left, bool fire)
{
   with_player(LocalPlayer)
   {
      str actor = fire ? so_FeuerExplosion : so_FeuerTest;
      i32 pufftid;
      ACS_LineAttack(0, p->yaw, p->pitch, 0, so_Dummy, so_NoDamage, 1024, FHF_NORANDOMPUFFZ|FHF_NOIMPACTDECAL, pufftid = ACS_UniqueTID());

      i32 sx = p->x;
      i32 sy = p->y;
      i32 sz = p->z + 32;
      i32 ex = GetX(pufftid);
      i32 ey = GetY(pufftid);
      i32 ez = GetZ(pufftid);

      struct polar cpp = ctopol(ex - sx, ey - sy);
      cpp.dst /= 4;
      if(left) cpp.ang += 0.07;
      else     cpp.ang -= 0.07;

      i32 cx = sx + ACS_Cos(cpp.ang) * cpp.dst;
      i32 cy = sy + ACS_Sin(cpp.ang) * cpp.dst;
      k32 max = fire ? 20 : 70;

      for(i32 i = 0; i < max; i++) {
         struct i32v2 v = qbezieri(sx, sy, cx, cy, ex, ey, i / max);
         i32 tid;
         ACS_SpawnForced(actor, v.x, v.y, lerpk(sz, ez, i / max), tid = ACS_UniqueTID());
         if(fire) {
            PtrSet(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
            ACS_Delay(1);
         }
      }

      if(fire) {
         i32 tid;
         ACS_SpawnForced(so_FeuerFinal, ex, ey, ez, tid = ACS_UniqueTID());
         PtrSet(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
      }
   }
}

script_str ext("ACS") addr("Lith_Cercle")
void Sc_Cercle(void)
{
   with_player(LocalPlayer)
   {
      k32 ax, ay, az;

      __with(i32 pufftid;)
      {
         ACS_LineAttack(0, p->yaw, p->pitch, 0, so_Dummy, so_NoDamage, 1024,
            FHF_NORANDOMPUFFZ|FHF_NOIMPACTDECAL, pufftid = ACS_UniqueTID());

         ax = GetX(pufftid);
         ay = GetY(pufftid);
         az = ACS_GetActorFloorZ(pufftid);
      }

      FreezeTime(true);
      ACS_Delay(2); /* necessary so sounds may play */

      ACS_AmbientSound(ss_weapons_cercle_begin, 127);

      i32 fxtid = ACS_UniqueTID();
      for(i32 i = 0; i < 100; i++)
      {
         k32 px = ACS_Cos(i / 100.0) * 77;
         k32 py = ACS_Sin(i / 100.0) * 77;
         i32 tid;

         ACS_SpawnForced(so_CircleParticle, ax + px, ay + py, az + 7, tid = ACS_UniqueTID());

         ACS_SetActorAngle(tid, i / 100.0);
         PtrSet(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
         ACS_Thing_ChangeTID(tid, fxtid);

         ACS_Delay(i % 2 * (i / 30.0));
      }

      ACS_Delay(35);

      i32 fxtid2 = ACS_UniqueTID();
      SetMembI(fxtid, sm_Trigger, true);
      SetMembI(fxtid, sm_FxTID, fxtid2);

      ACS_AmbientSound(ss_weapons_cercle_attack, 127);

      ACS_Delay(35);
      SetMembI(fxtid2, sm_Trigger, true);

      ACS_Delay(7);
      SetMembI(fxtid2, sm_Trigger, true);

      ACS_Delay(35);

      i32 fxtid3 = ACS_UniqueTID();
      for(i32 i = 0; i < 3; i++)
      {
         k32 px = ACS_Cos(i / 3.0) * 60;
         k32 py = ACS_Sin(i / 3.0) * 60;
         i32 tid;

         ACS_SpawnForced(so_CircleSpearThrower, ax + px, ay + py, az + 24, tid = ACS_UniqueTID());

         ACS_SetActorAngle(tid, i / 3.0);
         PtrSet(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
         ACS_Thing_ChangeTID(tid, fxtid3);

         ACS_Delay(7);
      }

      ACS_Delay(10);

      /* NB: The projectiles take the TIDs of the throwers, so this is actually triggering them. */
      SetMembI(fxtid3, sm_Trigger, true);

      /* Just in case. */
      ACS_Thing_Remove(fxtid);
      ACS_Thing_Remove(fxtid2);

      FreezeTime(false);
   }
}

script_str ext("ACS") addr("Lith_MagicSelect")
void Sc_MagicSelect(i32 num)
{
   with_player(LocalPlayer)
   {
      if(!p->getCVarI(sc_weapons_magicselanims)) return;

      switch(num) {
      case 1: case 4: ACS_FadeTo(255, 255, 0, 0.3, 0.0); break;
      case 2: case 3: ACS_FadeTo(255, 155, 0, 0.3, 0.0); break;
      case 6: case 7: ACS_FadeTo(255, 0,   0, 0.3, 0.0); break;
      }

      ACS_FadeTo(0, 0, 0, 0.0, 0.3);

      for(i32 i = 0; i < 4; i++) {
         for(i32 j = 0; j < 3; j++) {
            SetSize(64, 64);
            PrintSpriteA(StrParam(":MagicSel:Slot%i_%i", num, i + 1), 0,1, 0,1, 0.5);
            ACS_Delay(1);
         }
      }
   }
}

/* EOF */
