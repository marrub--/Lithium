// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#if LITHIUM
#include "w_monster.h"
#include "w_world.h"
#include "p_player.h"

#define DMON_MAX 0x7FFF

// Static Objects ------------------------------------------------------------|

// This is lazy-allocated. Don't touch or GDCC will break your computer's face.
noinit
static dmon_t dmonalloc[DMON_MAX];
static i32    dmonid;

// Extern Functions ----------------------------------------------------------|

void PrintDmonAllocSize(struct player *p)
{
   p->logH(1, "dmonalloc is %.2k megabytes!", sizeof dmonalloc * 4 / 1024 / 1024.0);
}

void DmonDebugInfo(void)
{
   static i32 lmvar idprev;

   if(world.dbgLevel < log_dmon)
      return;

   if(idprev < dmonid)
   {
      i32 hilvl = 0, lolvl = MAXLEVEL;
      i32 hirnk = 0, lornk = MAXRANK;

      for(i32 i = idprev; i < dmonid; i++)
      {
         dmon_t *m = &dmonalloc[i];
         if(m->level < lolvl) lolvl = m->level;
         if(m->level > hilvl) hilvl = m->level;
         if(m->rank  < lornk) lornk = m->rank;
         if(m->rank  > hirnk) hirnk = m->rank;
      }

      Log("\Cghighest\C- level enemy: lv.%i", hilvl);
      Log("\Chlowest \C- level enemy: lv.%i", lolvl);
      Log("\Cghighest\C- rank enemy:  r%i", hirnk);
      Log("\Chlowest \C- rank enemy:  r%i", lornk);
   }

   idprev = dmonid;
}

void DmonInit()
{
   dmonid = 0;
}

script
dmon_t *DmonPtr(i32 tid, i32 ptr)
{
   if(tid || ptr) ACS_SetActivator(tid, ptr);
   return DmonSelf();
}

stkcall
dmon_t *DmonSelf(void)
{
   ifauto(u32, id, InvNum(so_MonsterID)) return Dmon(id - 1);
   else                                  return nil;
}

stkcall
dmon_t *Dmon(u32 id)
{
   if(dmonalloc[id].active) return &dmonalloc[id];
   else                     return nil;
}

dmon_t *AllocDmon(void)
{
   dmon_t *m = &dmonalloc[dmonid];
   *m = (dmon_t){.active = true, .id = dmonid++};
   return m;
}
#endif

// EOF
