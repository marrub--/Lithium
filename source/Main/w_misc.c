// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

stkcall
void Lith_BeginAngles(int x, int y)
{
   world.a_cur = 0;
   for(int i = 0; i < countof(world.a_angles); i++)
   {
      world.a_angles[i].ang = 0;
      world.a_angles[i].dst = 0;
   }
   world.a_x = x;
   world.a_y = y;
}

stkcall
fixed Lith_AddAngle(int x, int y)
{
   if(world.a_cur >= countof(world.a_angles))
      return 0;

   struct polar *p = &world.a_angles[world.a_cur++];
   *p = ctopol(x - world.a_x, y - world.a_y);
   return p->ang;
}

script addr("Lith_AddAngle")
void Lith_AddAngleScript(int x, int y)
{
   Lith_AddAngle(x, y);
}

script ext("ACS")
void Lith_EmitScore(int amount)
{
   Lith_GiveAllScore(amount, false);
}

script ext("ACS")
void Lith_EmitEXP(int amount)
{
   Lith_GiveAllEXP(amount);
}

script ext("ACS") addr("Lith_GiveScore")
void Lith_GiveScoreScript(int score)
{
   Lith_GiveAllScore(score * (fixed64)ACS_RandomFixed(0.7, 1.2), false);
}

script ext("ACS")
void Lith_BoughtItemPickup(int id)
{
   int const chan = CHAN_ITEM|CHAN_NOPAUSE;
   withplayer(LocalPlayer) if(id)
   {
      upgrade_t *upgr = p->getUpgr(id);

      if(!upgr->owned)
      {
         switch(upgr->info->category)
         {
         case UC_Body: ACS_PlaySound(0, "player/pickup/upgrbody", chan, 1, false, ATTN_NONE); break;
         case UC_Weap: ACS_PlaySound(0, "player/pickup/upgrweap", chan, 1, false, ATTN_NONE); break;
         case UC_Extr: ACS_PlaySound(0, "player/pickup/upgrextr", chan, 1, false, ATTN_NONE); break;
         }

         upgr->setOwned(p);
         upgr->toggle(p);
      }
   }
   else
   {
      ACS_PlaySound(0, "player/pickup/item", chan, 1, false, ATTN_NONE);

      p->itemsbought++;
   }
}

// EOF
