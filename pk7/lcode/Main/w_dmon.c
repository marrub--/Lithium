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
// DmonDebugInfo
//
void DmonDebugInfo(void)
{
   static int lmvar idprev;
   
   if(world.dbgLevel < log_dmon)
      return;
   
   if(idprev < dmonid) {
      int hilvl = 0, lolvl = MAXLEVEL;
      int hirnk = 0, lornk = MAXRANK;
      
      for(int i = idprev; i < dmonid; i++) {
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
