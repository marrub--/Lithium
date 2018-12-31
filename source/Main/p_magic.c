// Copyright © 2017-2018 Alison Sanderson, all rights reserved.
#if LITHIUM
#include "p_player.h"
#include "p_hudid.h"
#include "w_world.h"

// Extern Functions ----------------------------------------------------------|

script ext("ACS")
void Lith_Blade(bool hit)
{
   ACS_SetHudSize(800, 600);
   DrawSpriteX(hit ? sp_Weapon_BladeHit : sp_Weapon_Blade, HUDMSG_FADEOUT|HUDMSG_ADDBLEND, hid_blade, 0.1, 0.1, TS * 3, 0.15);

   withplayer(LocalPlayer)
      if(!p->onground) p->setVel(p->velx / 2, p->vely / 2, 0);
}

script ext("ACS")
void Lith_Rend(bool hit, i32 set)
{
   static i32 num;

   if(!hit) num = set;

   str graphic = hit ? StrParam(":Weapon:RendHit%i", num)
                     : StrParam(":Weapon:Rend%i",    num);

   ACS_SetHudSize(800, 600);
   DrawSpriteX(graphic, HUDMSG_FADEOUT|HUDMSG_ADDBLEND, hid_blade, 0.1, 0.1, TS * 2, 0.1);

   withplayer(LocalPlayer)
      if(!p->onground) p->setVel(p->velx / 2, p->vely / 2, 0);
}

script ext("ACS")
void Lith_Feuer(bool left, bool fire)
{
   withplayer(LocalPlayer)
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
         struct vec2i v = qbezieri(sx, sy, cx, cy, ex, ey, i / max);
         i32 tid;
         ACS_SpawnForced(actor, v.x, v.y, lerpk(sz, ez, i / max), tid = ACS_UniqueTID());
         if(fire) {
            Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
            ACS_Delay(1);
         }
      }

      if(fire) {
         i32 tid;
         ACS_SpawnForced(so_FeuerFinal, ex, ey, ez, tid = ACS_UniqueTID());
         Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
      }
   }
}

script ext("ACS")
void Lith_Cercle(void)
{
   withplayer(LocalPlayer)
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

      world.freeze(true);
      ACS_Delay(2); // necessary so sounds may play

      ACS_AmbientSound(ss_weapons_cercle_begin, 127);

      i32 fxtid = ACS_UniqueTID();
      for(i32 i = 0; i < 100; i++)
      {
         k32 px = ACS_Cos(i / 100.0) * 77;
         k32 py = ACS_Sin(i / 100.0) * 77;
         i32 tid;

         ACS_SpawnForced(so_CircleParticle, ax + px, ay + py, az + 7, tid = ACS_UniqueTID());

         ACS_SetActorAngle(tid, i / 100.0);
         Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
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
         Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
         ACS_Thing_ChangeTID(tid, fxtid3);

         ACS_Delay(7);
      }

      ACS_Delay(10);

      // NB: The projectiles take the TIDs of the throwers, so this is actually triggering them.
      SetMembI(fxtid3, sm_Trigger, true);

      // Just in case.
      ACS_Thing_Remove(fxtid);
      ACS_Thing_Remove(fxtid2);

      world.freeze(false);
   }
}

script ext("ACS")
void Lith_MagicSelect(i32 num)
{
   withplayer(LocalPlayer)
   {
      if(!p->getCVarI(sc_weapons_magicselanims)) return;

      switch(num) {
      case 1: case 4: ACS_FadeTo(255, 255, 0, 0.3, 0.0); break;
      case 2: case 3: ACS_FadeTo(255, 155, 0, 0.3, 0.0); break;
      case 6: case 7: ACS_FadeTo(255, 0,   0, 0.3, 0.0); break;
      }

      ACS_FadeTo(0, 0, 0, 0.0, 0.3);

      ACS_SetHudSize(64, 64);
      for(i32 i = 0; i < 4; i++) {
         DrawSpriteAlpha(StrParam(":MagicSel:Slot%i_%i", num, i + 1),
            hid_magicsel, 0.1, 0.1, TS*3, 0.5);
         ACS_Delay(3);
      }
   }
}
#endif

// EOF
