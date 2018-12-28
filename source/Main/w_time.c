// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "common.h"
#include "p_player.h"
#include "w_world.h"

script ext("ACS")
i32 Lith_Timer(void)
{
   return ACS_Timer();
}

char const *Lith_CanonTime(i32 type)
{
   noinit static char ft[64], st[64], dt[64];

   i32 s = 53 + (world.ticks / 35);
   i32 m = 30 + (s           / 60);
   i32 h = 14 + (m           / 60);
   i32 d = 25 + (h           / 24);
   i32 M =  6 + (d           / 30);
   i32 Y = 48 + (M           / 11);

   s %= 60;
   m %= 60;
   h %= 24;

   d = d % 31 + 1;
   M = M % 12 + 1;
   Y = Y      + 1;

   switch(type) {
   case CANONTIME_FULL:  sprintf(ft, LC(LANG "TIME_FMT_LONG"),  h, m, s, d, M, Y); return ft;
   case CANONTIME_SHORT: sprintf(st, LC(LANG "TIME_FMT_SHORT"), h, m,    d, M, Y); return st;
   case CANONTIME_DATE:  sprintf(dt, LC(LANG "TIME_FMT_DATE"),           d, M, Y); return dt;
   }

   return nil;
}

stkcall
void Lith_FreezeTime(bool on)
{
   StrEntON
   static i32 lmvar frozen;

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
            Lith_GiveActorInventory(p->tid, so_TimeHax, 1);
            Lith_GiveActorInventory(p->tid, so_TimeHax2, 1);
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
            Lith_TakeActorInventory(p->tid, so_PowerTimeFreezer, 1);
            Lith_TakeActorInventory(p->tid, so_TimeHax2, 1);
            break;
         }
      }
   }
}

// EOF
