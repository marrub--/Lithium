// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

script ext("ACS")
int Lith_Timer(void)
{
   return ACS_Timer();
}

char const *Lith_CanonTime(int type)
{
   noinit static char ft[64], st[64], dt[64];

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

#pragma GDCC STRENT_LITERAL OFF
   switch(type) {
   case CANONTIME_FULL:  sprintf(ft, LC(LANG "TIME_FMT_LONG"),  h, m, s, d, M, Y); return ft;
   case CANONTIME_SHORT: sprintf(st, LC(LANG "TIME_FMT_SHORT"), h, m,    d, M, Y); return st;
   case CANONTIME_DATE:  sprintf(dt, LC(LANG "TIME_FMT_DATE"),           d, M, Y); return dt;
   }

   return "";
#pragma GDCC STRENT_LITERAL ON
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
            Lith_GiveActorInventory(p->tid, OBJ "TimeHax", 1);
            Lith_GiveActorInventory(p->tid, OBJ "TimeHax2", 1);
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
            Lith_TakeActorInventory(p->tid, OBJ "TimeHax2", 1);
            break;
         }
      }
   }
}

// EOF
