// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#if LITHIUM
#include "lith_monster.h"
#include "lith_world.h"
#include "lith_player.h"

StrEntON

#define DMON_MAX 0x7FFF

// Static Objects ------------------------------------------------------------|

// This is lazy-allocated. Don't touch or GDCC will break your computer's face.
noinit
static dmon_t dmonalloc[DMON_MAX];
static int    dmonid;

// Extern Functions ----------------------------------------------------------|

void PrintDmonAllocSize(struct player *p)
{
   p->logH(1, "dmonalloc is %.2k megabytes!", sizeof dmonalloc * 4 / 1024 / 1024.0);
}

void DmonDebugInfo(void)
{
   static int lmvar idprev;

   if(world.dbgLevel < log_dmon)
      return;

   if(idprev < dmonid)
   {
      int hilvl = 0, lolvl = MAXLEVEL;
      int hirnk = 0, lornk = MAXRANK;

      for(int i = idprev; i < dmonid; i++)
      {
         dmon_t *m = &dmonalloc[i];
         if(m->level < lolvl) lolvl = m->level;
         if(m->level > hilvl) hilvl = m->level;
         if(m->rank  < lornk) lornk = m->rank;
         if(m->rank  > hirnk) hirnk = m->rank;
      }

      Log(c"\Cghighest\C- level enemy: lv.%i", hilvl);
      Log(c"\Chlowest \C- level enemy: lv.%i", lolvl);
      Log(c"\Cghighest\C- rank enemy:  r%i", hirnk);
      Log(c"\Chlowest \C- rank enemy:  r%i", lornk);
   }

   idprev = dmonid;
}

void DmonInit()
{
   dmonid = 0;
}

script
dmon_t *DmonPtr(int tid, int ptr)
{
   if(tid || ptr) ACS_SetActivator(tid, ptr);
   return DmonSelf();
}

stkcall
dmon_t *DmonSelf(void)
{
   ifauto(u32, id, InvNum(OBJ "MonsterID")) return Dmon(id - 1);
   else                                     return null;
}

stkcall
dmon_t *Dmon(u32 id)
{
   if(dmonalloc[id].active) return &dmonalloc[id];
   else                     return null;
}

dmon_t *AllocDmon(void)
{
   dmon_t *m = &dmonalloc[dmonid];
   *m = (dmon_t){.active = true, .id = dmonid++};
   return m;
}
#endif

// EOF
