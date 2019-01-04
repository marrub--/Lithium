// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#include "common.h"
#include "p_player.h"
#include "w_world.h"

#include <stdio.h>
#include <GDCC.h>

// Extern Objects ------------------------------------------------------------|

str dbgstat[64], dbgnote[64];
i32 dbgstatnum,  dbgnotenum;

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

script
void Lith_DumpAlloc(void)
{
   __GDCC__alloc_dump();
}

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

void Lith_DebugStat(char const *fmt, ...)
{
   if(!(world.dbgLevel & log_devh)) return;

   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   dbgstat[dbgstatnum++] = ACS_EndStrParam();
}

void Lith_DebugNote(char const *fmt, ...)
{
   if(!(world.dbgLevel & log_devh)) return;

   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   dbgnote[dbgnotenum++] = ACS_EndStrParam();
}

// EOF
