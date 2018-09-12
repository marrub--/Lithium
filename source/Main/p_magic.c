// Copyright © 2017 Graham Sanderson, all rights reserved.
// vim: columns=110
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_world.h"

// Extern Functions ----------------------------------------------------------|

//
// Lith_Blade
//
script ext("ACS")
void Lith_Blade(bool hit)
{
   ACS_SetHudSize(800, 600);
   DrawSpriteX(hit ? "lgfx/Weapon/BladeHit.png" : "lgfx/Weapon/Blade.png", HUDMSG_FADEOUT|HUDMSG_ADDBLEND, hid_blade, 0.1, 0.1, TICSECOND * 3, 0.15);

   withplayer(LocalPlayer)
      if(p->z > p->floorz)
         p->setVel(p->velx / 2, p->vely / 2, 0);
}

//
// Lith_Rend
//
script ext("ACS")
void Lith_Rend(bool hit, int set)
{
   static int num;

   if(!hit) num = set;

   __str graphic = hit ? StrParam("lgfx/Weapon/RendHit%i.png", num)
                       : StrParam("lgfx/Weapon/Rend%i.png", num);

   ACS_SetHudSize(800, 600);
   DrawSpriteX(graphic, HUDMSG_FADEOUT|HUDMSG_ADDBLEND, hid_blade, 0.1, 0.1, TICSECOND * 2, 0.1);

   withplayer(LocalPlayer)
      if(p->z > p->floorz)
         p->setVel(p->velx / 2, p->vely / 2, 0);
}

//
// Lith_Feuer
//
script ext("ACS")
void Lith_Feuer(bool left, bool fire)
{
   withplayer(LocalPlayer)
   {
      __str actor = fire ? "Lith_FeuerExplosion" : "Lith_FeuerTest";
      int pufftid;
      ACS_LineAttack(0, p->yaw, p->pitch, 0, "Lith_Dummy", "Lith_NoDamage", 1024, FHF_NORANDOMPUFFZ|FHF_NOIMPACTDECAL, pufftid = ACS_UniqueTID());

      int sx = p->x;
      int sy = p->y;
      int sz = p->z + 32;
      int ex = ACS_GetActorX(pufftid);
      int ey = ACS_GetActorY(pufftid);
      int ez = ACS_GetActorZ(pufftid);

      struct polar cpp = ctopol(ex - sx, ey - sy);
      cpp.dst /= 4;
      if(left) cpp.ang += 0.07;
      else     cpp.ang -= 0.07;

      int cx = sx + ACS_Cos(cpp.ang) * cpp.dst;
      int cy = sy + ACS_Sin(cpp.ang) * cpp.dst;
      fixed max = fire ? 20 : 70;

      for(int i = 0; i < max; i++) {
         struct vec2i v = qbezieri(sx, sy, cx, cy, ex, ey, i / max);
         int tid;
         ACS_SpawnForced(actor, v.x, v.y, lerpk(sz, ez, i / max), tid = ACS_UniqueTID());
         if(fire) {
            Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
            ACS_Delay(1);
         }
      }

      if(fire) {
         int tid;
         ACS_SpawnForced("Lith_FeuerFinal", ex, ey, ez, tid = ACS_UniqueTID());
         Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
      }
   }
}

//
// Lith_Cercle
//
script ext("ACS")
void Lith_Cercle(void)
{
   withplayer(LocalPlayer)
   {
      fixed ax, ay, az;

      __with(int pufftid;)
      {
         ACS_LineAttack(0, p->yaw, p->pitch, 0, "Lith_Dummy", "Lith_NoDamage", 1024,
            FHF_NORANDOMPUFFZ|FHF_NOIMPACTDECAL, pufftid = ACS_UniqueTID());

         ax = ACS_GetActorX(pufftid);
         ay = ACS_GetActorY(pufftid);
         az = ACS_GetActorFloorZ(pufftid);
      }

      world.freeze(true);
      ACS_Delay(2); // necessary so sounds may play

      ACS_AmbientSound("weapons/cercle/begin", 127);

      int fxtid = ACS_UniqueTID();
      for(int i = 0; i < 100; i++)
      {
         fixed px = ACS_Cos(i / 100.0) * 77;
         fixed py = ACS_Sin(i / 100.0) * 77;
         int tid;

         ACS_SpawnForced("Lith_CircleParticle", ax + px, ay + py, az + 7, tid = ACS_UniqueTID());

         ACS_SetActorAngle(tid, i / 100.0);
         Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
         ACS_Thing_ChangeTID(tid, fxtid);

         ACS_Delay(i % 2 * (i / 30.0));
      }

      ACS_Delay(35);

      int fxtid2 = ACS_UniqueTID();
      ACS_SetUserVariable(fxtid, "user_trigger", true);
      ACS_SetUserVariable(fxtid, "user_fxtid", fxtid2);

      ACS_AmbientSound("weapons/cercle/attack", 127);

      ACS_Delay(35);
      ACS_SetUserVariable(fxtid2, "user_trigger", true);

      ACS_Delay(7);
      ACS_SetUserVariable(fxtid2, "user_trigger", true);

      ACS_Delay(35);

      int fxtid3 = ACS_UniqueTID();
      for(int i = 0; i < 3; i++)
      {
         fixed px = ACS_Cos(i / 3.0) * 60;
         fixed py = ACS_Sin(i / 3.0) * 60;
         int tid;

         ACS_SpawnForced("Lith_CircleSpearThrower", ax + px, ay + py, az + 24, tid = ACS_UniqueTID());

         ACS_SetActorAngle(tid, i / 3.0);
         Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
         ACS_Thing_ChangeTID(tid, fxtid3);

         ACS_Delay(7);
      }

      ACS_Delay(10);

      // NB: The projectiles take the TIDs of the throwers, so this is actually triggering them.
      ACS_SetUserVariable(fxtid3, "user_trigger", true);

      // Just in case.
      ACS_Thing_Remove(fxtid);
      ACS_Thing_Remove(fxtid2);

      world.freeze(false);
   }
}

//
// Lith_MagicSelect
//
script ext("ACS")
void Lith_MagicSelect(int num)
{
   withplayer(LocalPlayer)
   {
      if(!p->getCVarI("lith_weapons_magicselanims")) return;

      switch(num)
      {
      case 1:
      case 4: ACS_FadeTo(255, 255, 0, 0.3, 0.0); break;
      case 2:
      case 3: ACS_FadeTo(255, 155, 0, 0.3, 0.0); break;
      case 6:
      case 7: ACS_FadeTo(255, 0,   0, 0.3, 0.0); break;
      }

      ACS_FadeTo(0, 0, 0, 0.0, 0.3);

      ACS_SetHudSize(64, 64);
      for(int i = 0; i < 4; i++) {
         DrawSpriteAlpha(StrParam("lgfx/MagicSel/Slot%i_%i.png", num, i + 1),
            hid_magicsel, 0.1, 0.1, TICSECOND*3, 0.5);
         ACS_Delay(3);
      }
   }
}

// EOF
