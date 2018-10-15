// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

script ext("ACS")
int Lith_Timer(void)
{
   return ACS_Timer();
}

__str Lith_CanonTime(int type)
{
   int s = 53 + (world.ticks / 35);
   int m = 30 + (s           / 60);
   int h = 14 + (m           / 60);
   int d = 25 + (h           / 24);
   int M =  6 + (d           / 30);
   int Y = 48 + (M           / 11);

   s %= 60;
   m %= 60;
   h %= 24;

   d = d % 31 + 1;
   M = M % 12 + 1;
   Y = Y      + 1;

   switch(type)
   {
   case CANONTIME_FULL:  return StrParam(L("LITH_TIME_FMT_LONG"),  h, m, s, d, M, Y);
   case CANONTIME_SHORT: return StrParam(L("LITH_TIME_FMT_SHORT"), h, m,    d, M, Y);
   case CANONTIME_DATE:  return StrParam(L("LITH_TIME_FMT_DATE"),           d, M, Y);
   }

   return "invalid";
}

stkcall
void Lith_FreezeTime(bool on)
{
   static int lmvar frozen;

   if(on)
   {
      if(!frozen++)
      {
         Lith_ForPlayer()
         {
            p->frozen++;
            p->setVel(0, 0, 0);
         }

         Lith_ForPlayer()
         {
            Lith_GiveActorInventory(p->tid, "Lith_TimeHax", 1);
            Lith_GiveActorInventory(p->tid, "Lith_TimeHax2", 1);
            break;
         }
      }
   }
   else
   {
      if(!--frozen)
      {
         Lith_ForPlayer() p->frozen--;

         Lith_ForPlayer()
         {
            Lith_TakeActorInventory(p->tid, "PowerTimeFreezer", 1);
            Lith_TakeActorInventory(p->tid, "Lith_TimeHax2", 1);
            break;
         }
      }
   }
}

// EOF
