// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// debug.c: Debugging functions.

#include "common.h"
#include "p_player.h"

#include <stdio.h>
#include <GDCC.h>

// Extern Objects ------------------------------------------------------------|

str dbgstat[64], dbgnote[64];
i32 dbgstatnum,  dbgnotenum;

// Extern Functions ----------------------------------------------------------|

void Lith_DebugStat(char const *fmt, ...)
{
   if(!(dbglevel & log_devh)) return;

   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   dbgstat[dbgstatnum++] = ACS_EndStrParam();
}

void Lith_DebugNote(char const *fmt, ...)
{
   if(!(dbglevel & log_devh)) return;

   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   dbgnote[dbgnotenum++] = ACS_EndStrParam();
}

void Log(char const *fmt, ...)
{
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf(fmt, vl);
   va_end(vl);

   ACS_EndLog();
}

script
void Lith_PrintMem(void const *data, size_t size)
{
   byte const *d = data;
   i32 pos = 0;

   for(size_t i = 0; i < size; i++)
   {
      if(pos + 3 > 79)
      {
         puts("");
         pos = 0;
      }

      printf(IsPrint(d[i]) ? "%c  " : c"%.2X ",  d[i]);

      pos += 3;
   }

   puts("\nEOF\n\n");
}

// Scripts -------------------------------------------------------------------|

script_str ext("ACS") addr("Lith_GiveMail")
void Sc_DbgGiveMail(i32 num)
{
   static str const names[] = {
      s"Intro",
      s"Cluster1",
      s"Cluster2",
      s"Cluster3",
      s"Phantom",
      s"JamesDefeated",
      s"MakarovDefeated",
      s"IsaacDefeated"
   };

   num %= countof(names);

   withplayer(LocalPlayer)
      p->deliverMail(names[num]);
}

script_str ext("ACS") addr("Lith_GiveMeAllOfTheScore")
void Sc_DbgGiveScore(void)
{
   withplayer(LocalPlayer) p->giveScore(0x7FFFFFFFFFFFFFFFFFFFFFFFLL, true);
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

// EOF
