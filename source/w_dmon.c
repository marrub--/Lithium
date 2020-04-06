/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Monster tracker functions.
 *
 * ---------------------------------------------------------------------------|
 */

#include "w_monster.h"
#include "w_world.h"
#include "p_player.h"

#define DMON_MAX 0x7FFF

/* Static Objects ---------------------------------------------------------- */

/* This is lazy-allocated. Don't touch or GDCC will break your computer's face. */
noinit
static dmon_t dmonalloc[DMON_MAX];
static i32    dmonid;

/* Extern Functions -------------------------------------------------------- */

void PrintDmonAllocSize(struct player *p)
{
   p->logH(1, "dmonalloc is %.2k megabytes!", sizeof dmonalloc * 4 / 1024 / 1024.0);
}

void DmonInit(void)
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
   ifauto(i32, id, InvNum(so_MonsterID)) return Dmon(id - 1);
   else                                  return nil;
}

stkcall
dmon_t *Dmon(i32 id)
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

/* EOF */
