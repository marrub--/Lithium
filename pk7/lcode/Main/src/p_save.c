#include "lith_common.h"
#include "lith_player.h"
#include "lith_savedata.h"
#include "lith_netfile.h"


//----------------------------------------------------------------------------
// Static Objects
//

static int const sizeof_agrp = 12 + 1;
static int const sizeof_abuy = 1;


//----------------------------------------------------------------------------
// Static Functions
//

//
// Lith_Save_agrp
//
[[__call("ScriptS")]]
static void Lith_Save_agrp(savefile_t *save)
{
   bool *any = calloc(UPGR_MAX, sizeof(bool));
   int total = 0;
   
   for(int i = 0; i < UPGR_MAX; i++)
   {
      for(int j = 0; j < NUMAUTOGROUPS; j++)
      {
         upgrade_t const *upgr = save->p->getUpgr(i);
         
         if(upgr && upgr->autogroups[j])
         {
            any[i] = true;
            break;
         }
      }
      
      if(any[i])
         total++;
   }
   
   if(total)
   {
      Lith_SaveWriteChunk(save, Ident_agrp, SaveV_agrp, total * sizeof_agrp);
      
      for(int i = 0; i < UPGR_MAX; i++)
         if(any[i])
      {
         upgrade_t const *upgr = save->p->getUpgr(i);
         
         if(upgr)
         {
            char name[12] = {};
            Lith_strcpy_str(name, upgr->info->name);
            
            // Pack the groups into a bitfield.
            unsigned groups = 0;
            for(int j = 0; j < NUMAUTOGROUPS; j++)
               if(upgr->autogroups[j])
                  groups |= 1 << j;
            
            Lith_FWrite32(name,    12, 1, save->fp);
            Lith_FWrite32(&groups, 1,  1, save->fp);
         }
      }
   }
   
   free(any);
}

//
// Lith_Load_agrp
//
[[__call("ScriptS")]]
static void Lith_Load_agrp(savefile_t *save, savechunk_t *chunk)
{
   if((chunk->size % sizeof_agrp) != 0) return;
   
   size_t num = chunk->size / sizeof_agrp;
   
   for(int def = 0; def < num; def++)
   {
      char name[12];
      unsigned groups;
      
      Lith_FRead32(&name,   12, 1, save->fp);
      Lith_FRead32(&groups, 1,  1, save->fp);
      
      // Unpack groups.
      for(int i = 0; i < save->p->upgrmax; i++)
      {
         upgrade_t const *upgr = &save->p->upgrades[i];
         
         if(Lith_strcmp_str(name, upgr->info->name) == 0)
         {
            for(int j = 0; j < NUMAUTOGROUPS; j++)
               upgr->autogroups[j] = groups & (1 << j);
            
            break;
         }
      }
   }
}

//
// Lith_Save_abuy
//
[[__call("ScriptS")]]
static void Lith_Save_abuy(savefile_t *save)
{
   uint32_t groups = 0;
   
   for(int i = 0; i < NUMAUTOGROUPS; i++)
      if(save->p->autobuy[i])
         groups |= 1 << i;
   
   if(groups)
   {
      Lith_FWrite32(&groups, 1, 1, save->fp);
      Lith_SaveWriteChunk(save, Ident_abuy, SaveV_abuy, sizeof_abuy);
   }
}

//
// Lith_Load_abuy
//
[[__call("ScriptS")]]
static void Lith_Load_abuy(savefile_t *save, savechunk_t *chunk)
{
   if(chunk->size != sizeof_abuy) return;
   
   uint32_t groups;
   Lith_FRead32(&groups, 1, 1, save->fp);
   
   for(int i = 0; i < NUMAUTOGROUPS; i++)
      save->p->autobuy[i] = (groups & (1 << i)) != 0;
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_PlayerSaveData
//
[[__call("ScriptS")]]
void Lith_PlayerSaveData(player_t *p)
{
   savefile_t *save;
   if((save = Lith_SaveBegin(p)))
   {
      Lith_Save_agrp(save);
      Lith_Save_abuy(save);
      Lith_SaveEnd(save);
   }
}

//
// Lith_PlayerLoadData
//
[[__call("ScriptS")]]
void Lith_PlayerLoadData(player_t *p)
{
   savefile_t *save;
   if((save = Lith_LoadBegin(p)))
   {
      Lith_LoadChunk(save, Ident_agrp, SaveV_agrp, Lith_Load_agrp);
      Lith_LoadChunk(save, Ident_abuy, SaveV_abuy, Lith_Load_abuy);
      Lith_LoadEnd(save);
   }
}

// EOF

