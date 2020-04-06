/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Debugging functions.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "w_monster.h"
#include "m_char.h"

#include <stdio.h>
#include <GDCC.h>

/* Extern Objects ---------------------------------------------------------- */

str dbgstat[64], dbgnote[64];
i32 dbgstatnum,  dbgnotenum;

/* Extern Functions -------------------------------------------------------- */

void Dbg_Stat_Impl(cstr fmt, ...)
{
   if(!(dbglevel & log_devh)) return;

   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   dbgstat[dbgstatnum++] = ACS_EndStrParam();
}

void Dbg_Note_Impl(cstr fmt, ...)
{
   if(!(dbglevel & log_devh)) return;

   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   dbgnote[dbgnotenum++] = ACS_EndStrParam();
}

void Dbg_PrintMemC(void const *data, size_t size)
{
   u32 const *d = data;
   i32 pos = 0;

   for(size_t i = 0; i < size * 4; i++) {
      if(pos + 3 > 79) {
         ACS_PrintChar('\n');
         pos = 0;
      }

      byte c = Cps_GetC(d, i);

      __nprintf(IsPrint(c) ? "%c  " : c"%.2X ", c);

      pos += 3;
   }

   __nprintf("\nEOF\n");
}

void Dbg_PrintMem(void const *data, size_t size)
{
   byte const *d = data;
   i32 pos = 0;

   for(size_t i = 0; i < size; i++)
   {
      if(pos + 3 > 79) {
         ACS_PrintChar('\n');
         pos = 0;
      }

      __nprintf(IsPrint(d[i]) ? "%c  " : c"%.2X ", d[i]);

      pos += 3;
   }

   __nprintf("\nEOF\n");
}

void Log(cstr fmt, ...)
{
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   ACS_EndLog();
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr("Lith_GiveEXPToMonster")
void Sc_GiveEXPToMonster(i32 amt)
{
   ifauto(dmon_t *, m, DmonPtr(0, AAPTR_PLAYER_GETTARGET)) m->exp += amt;
}

script_str ext("ACS") addr("Lith_GiveMeAllOfTheScore")
void Sc_DbgGiveScore(void)
{
   with_player(LocalPlayer) P_Scr_Give(p, INT96_MAX, true);
}

script_str ext("ACS") addr("Lith_DumpAlloc")
void Sc_DbgDumpAlloc(void)
{
   __GDCC__alloc_dump();
}

script_str ext("ACS") addr("Lith_PrintMonsterInfo")
void Sc_PrintMonsterInfo(void)
{
   extern void PrintMonsterInfo(void);
   PrintMonsterInfo();
}

/* EOF */
