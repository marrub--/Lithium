// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// w_misc.c: Miscellaneous scripts.

#include "common.h"
#include "p_player.h"
#include "w_world.h"

// Static Functions ----------------------------------------------------------|

stkcall
static void SetInventory(str item, i32 amount)
{
   i32 s = InvNum(item) - amount;
        if(s < 0) InvTake(item, -s);
   else if(s > 0) InvGive(item,  s);
}

stkcall
static void SetActorInventory(i32 tid, str item, i32 amount)
{
   i32 s = ACS_CheckActorInventory(tid, item) - amount;
        if(s < 0) ACS_TakeActorInventory(tid, item, -s);
   else if(s > 0) ACS_GiveActorInventory(tid, item,  s);
}

// Extern Functions ----------------------------------------------------------|

stkcall
void Lith_FadeFlash(i32 r, i32 g, i32 b, k32 amount, k32 seconds)
{
   ACS_FadeTo(r, g, b, amount, 0.0);
   ACS_FadeTo(r, g, b, 0.0, seconds);
}

script
i32 Lith_GetTID(i32 tid, i32 ptr)
{
   if(tid || ptr)
      ACS_SetActivator(tid, ptr);
   return ACS_ActivatorTID();
}

script
i32 Lith_GetPlayerNumber(i32 tid, i32 ptr)
{
   if(tid || ptr)
      ACS_SetActivator(tid, ptr);
   return ACS_PlayerNumber();
}

script
bool Lith_ValidPointer(i32 tid, i32 ptr)
{
   if(tid || ptr)
      return ACS_SetActivator(tid, ptr);
   else
      return true;
}

script
bool Lith_SetPointer(i32 tid, i32 ptr, i32 assign, i32 tid2, i32 ptr2, i32 flags)
{
   if(tid || ptr)
      ACS_SetActivator(tid, ptr);
   return ACS_SetPointer(assign, tid2, ptr2, flags);
}

stkcall
i32 Lith_CheckActorInventory(i32 tid, str item)
{
   if(tid == 0) return InvNum(item);
   else         return ACS_CheckActorInventory(tid, item);
}

stkcall
void Lith_GiveActorInventory(i32 tid, str item, i32 amount)
{
   if(tid == 0) InvGive(item, amount);
   else         ACS_GiveActorInventory(tid, item, amount);
}

stkcall
void Lith_TakeActorInventory(i32 tid, str item, i32 amount)
{
   if(tid == 0) InvTake(item, amount);
   else         ACS_TakeActorInventory(tid, item, amount);
}

stkcall
void Lith_SetActorInventory(i32 tid, str item, i32 amount)
{
   if(tid == 0) SetInventory(item, amount);
   else         SetActorInventory(tid, item, amount);
}

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

// Scripts -------------------------------------------------------------------|

script ext("ACS") addr(lsc_addangle)
void Sc_AddAngle(i32 x, i32 y)
{
   Lith_AddAngle(x, y);
}

script_str ext("ACS") addr("Lith_EmitScore")
void Sc_EmitScore(i32 amount)
{
   Lith_GiveAllScore(amount, false);
}

script_str ext("ACS") addr("Lith_EmitEXP")
void Sc_EmitEXP(i32 amount)
{
   Lith_GiveAllEXP(amount);
}

script_str ext("ACS") addr("Lith_GiveScore")
void Sc_GiveScore(i32 score)
{
   Lith_GiveAllScore(score * (k64)ACS_RandomFixed(0.7, 1.2), false);
}

script_str ext("ACS")
void Lith_BoughtItemPickup(i32 id)
{
   static i32 const chan = CHAN_ITEM|CHAN_NOPAUSE;
   withplayer(LocalPlayer) if(id)
   {
      struct upgrade *upgr = p->getUpgr(id);

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
