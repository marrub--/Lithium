// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

#include <stdio.h>
#include <GDCC.h>

// Extern Objects ------------------------------------------------------------|

__str dbgstat[64], dbgnote[64];
int   dbgstatnum,  dbgnotenum;

// Static Functions ----------------------------------------------------------|

//
// SetInventory
//
[[__call("StkCall")]]
static void SetInventory(__str item, int amount)
{
   int s = ACS_CheckInventory(item) - amount;
        if(s < 0) ACS_TakeInventory(item, -s);
   else if(s > 0) ACS_GiveInventory(item,  s);
}

//
// SetActorInventory
//
[[__call("StkCall")]]
static void SetActorInventory(int tid, __str item, int amount)
{
   int s = ACS_CheckActorInventory(tid, item) - amount;
        if(s < 0) ACS_TakeActorInventory(tid, item, -s);
   else if(s > 0) ACS_GiveActorInventory(tid, item,  s);
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_DumpAlloc
//
[[__call("ScriptS")]]
void Lith_DumpAlloc(void)
{
   __GDCC__alloc_dump();
}

//
// Lith_FadeFlash
//
[[__call("StkCall")]]
void Lith_FadeFlash(int r, int g, int b, fixed amount, fixed seconds)
{
   ACS_FadeTo(r, g, b, amount, 0.0);
   ACS_FadeTo(r, g, b, 0.0, seconds);
}

//
// Lith_GetTID
//
[[__call("ScriptS")]]
int Lith_GetTID(int tid, int ptr)
{
   if(tid || ptr)
      ACS_SetActivator(tid, ptr);
   return ACS_ActivatorTID();
}

//
// Lith_GetPlayerNumber
//
[[__call("ScriptS")]]
int Lith_GetPlayerNumber(int tid, int ptr)
{
   if(tid || ptr)
      ACS_SetActivator(tid, ptr);
   return ACS_PlayerNumber();
}

//
// Lith_ValidPointer
//
[[__call("ScriptS")]]
bool Lith_ValidPointer(int tid, int ptr)
{
   if(tid || ptr)
      return ACS_SetActivator(tid, ptr);
   else
      return true;
}

//
// Lith_SetPointer
//
[[__call("ScriptS")]]
bool Lith_SetPointer(int tid, int ptr, int assign, int tid2, int ptr2, int flags)
{
   if(tid || ptr)
      ACS_SetActivator(tid, ptr);
   return ACS_SetPointer(assign, tid2, ptr2, flags);
}

//
// Lith_CheckActorInventory
//
[[__call("StkCall")]]
int Lith_CheckActorInventory(int tid, __str item)
{
   if(tid == 0) return ACS_CheckInventory(item);
   else         return ACS_CheckActorInventory(tid, item);
}

//
// Lith_GiveActorInventory
//
[[__call("StkCall")]]
void Lith_GiveActorInventory(int tid, __str item, int amount)
{
   if(tid == 0) ACS_GiveInventory(item, amount);
   else         ACS_GiveActorInventory(tid, item, amount);
}

//
// Lith_TakeActorInventory
//
[[__call("StkCall")]]
void Lith_TakeActorInventory(int tid, __str item, int amount)
{
   if(tid == 0) ACS_TakeInventory(item, amount);
   else         ACS_TakeActorInventory(tid, item, amount);
}

//
// Lith_SetActorInventory
//
[[__call("StkCall")]]
void Lith_SetActorInventory(int tid, __str item, int amount)
{
   if(tid == 0) SetInventory(item, amount);
   else         SetActorInventory(tid, item, amount);
}

//
// Lith_DebugStat
//
void Lith_DebugStat(__str fmt, ...)
{
   if(!(world.dbgLevel & log_devh)) return;

   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);

   dbgstat[dbgstatnum++] = ACS_EndStrParam();
}

//
// Lith_DebugNote
//
void Lith_DebugNote(__str fmt, ...)
{
   if(!(world.dbgLevel & log_devh)) return;

   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);

   dbgnote[dbgnotenum++] = ACS_EndStrParam();
}

// EOF

