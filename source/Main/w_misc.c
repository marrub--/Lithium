// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "common.h"
#include "p_player.h"
#include "w_world.h"

stkcall
void Lith_BeginAngles(i32 x, i32 y)
{
   world.a_cur = 0;
   for(i32 i = 0; i < countof(world.a_angles); i++)
   {
      world.a_angles[i].ang = 0;
      world.a_angles[i].dst = 0;
   }
   world.a_x = x;
   world.a_y = y;
}

stkcall
k32 Lith_AddAngle(i32 x, i32 y)
{
   if(world.a_cur >= countof(world.a_angles))
      return 0;

   struct polar *p = &world.a_angles[world.a_cur++];
   *p = ctopol(x - world.a_x, y - world.a_y);
   return p->ang;
}

script addr("Lith_AddAngle")
void Lith_AddAngleScript(i32 x, i32 y)
{
   Lith_AddAngle(x, y);
}

script ext("ACS")
void Lith_EmitScore(i32 amount)
{
   Lith_GiveAllScore(amount, false);
}

script ext("ACS")
void Lith_EmitEXP(i32 amount)
{
   Lith_GiveAllEXP(amount);
}

script ext("ACS") addr("Lith_GiveScore")
void Lith_GiveScoreScript(i32 score)
{
   Lith_GiveAllScore(score * (k64)ACS_RandomFixed(0.7, 1.2), false);
}

script ext("ACS")
void Lith_BoughtItemPickup(i32 id)
{
   static i32 const chan = CHAN_ITEM|CHAN_NOPAUSE;
   withplayer(LocalPlayer) if(id)
   {
      upgrade_t *upgr = p->getUpgr(id);

      if(!upgr->owned)
      {
         switch(upgr->info->category) {
         case UC_Body: ACS_PlaySound(0, ss_player_pickup_upgrbody, chan, 1, false, ATTN_NONE); break;
         case UC_Weap: ACS_PlaySound(0, ss_player_pickup_upgrweap, chan, 1, false, ATTN_NONE); break;
         case UC_Extr: ACS_PlaySound(0, ss_player_pickup_upgrextr, chan, 1, false, ATTN_NONE); break;
         }

         upgr->setOwned(p);
         upgr->toggle(p);
      }
   }
   else
   {
      ACS_PlaySound(0, ss_player_pickup_item, chan, 1, false, ATTN_NONE);

      p->itemsbought++;
   }
}

// EOF
