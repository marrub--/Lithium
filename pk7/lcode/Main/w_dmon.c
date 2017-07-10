#include "lith_monster.h"
#include "lith_world.h"

#define DMON_MAX 0x7FFF


//----------------------------------------------------------------------------
// Static Objects
//

__addrdef __gbl_arr dmonarr;

// This is lazy-allocated. Don't touch or GDCC will break your computer's face.
[[__no_init]] dmon_t dmonalloc[DMON_MAX];
int dmonarr dmonid;


//----------------------------------------------------------------------------
// Extern Functions
//

//
// DmonPtr
//
[[__call("ScriptS")]]
dmon_t *DmonPtr(int tid, int ptr)
{
   if(tid || ptr)
      ACS_SetActivator(tid, ptr);

   ifauto(int, id, ACS_CheckInventory("Lith_MonsterID"))
      return Dmon(id - 1);
   else
      return null;
}

//
// Dmon
//
dmon_t *Dmon(int id)
{
   if(dmonalloc[id].active)
      return &dmonalloc[id];
   else
      return null;
}

//
// AllocDmon
//
dmon_t *AllocDmon(void)
{
   dmon_t *m = &dmonalloc[dmonid];
   *m = (struct dmon){};
   
   m->active = true;
   m->id = dmonid;
   
   dmonid++;
   
   return m;
}

// EOF
