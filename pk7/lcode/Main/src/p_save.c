#include "lith_common.h"
#include "lith_player.h"
#include "lith_savedata.h"


//----------------------------------------------------------------------------
// Types
//

typedef struct autogroupdef_s
{
   uint32_t name[3];
   uint32_t groups;
} autogroupdef_t;


//----------------------------------------------------------------------------
// Static Functions
//

//
// Lith_SaveAutoGroups
//
[[__call("ScriptS")]]
static void Lith_SaveAutoGroups(savefile_t *save)
{
   bool any[UPGR_MAX] = {};
   int total = 0;
   
   for(int i = 0; i < UPGR_MAX; i++)
   {
      for(int j = 0; j < NUMAUTOGROUPS; j++)
         if(save->p->upgrades[i].autogroups[j]) {any[i] = true; break;}
      
      if(any[i]) total++;
   }
   
   if(total)
   {
      autogroupdef_t *data = calloc(total,  sizeof(autogroupdef_t));
      size_t          size =        total * sizeof(autogroupdef_t);
      
      for(int i = 0, def = 0; i < UPGR_MAX; i++)
         if(any[i])
      {
         upgrade_t const *upgr = &save->p->upgrades[i];
         
         // Pack the name into three 4-byte values.
         for(int j = 0; j < 3; j++)
         {
            data[def].name[j] |= upgr->info->name[j * 4 + 0] << 24;
            data[def].name[j] |= upgr->info->name[j * 4 + 1] << 16;
            data[def].name[j] |= upgr->info->name[j * 4 + 2] <<  8;
            data[def].name[j] |= upgr->info->name[j * 4 + 3] <<  0;
         }
         
         // Pack the groups into a bitfield.
         for(int j = 0; j < NUMAUTOGROUPS; j++)
            if(upgr->autogroups[j])
               data[def].groups |= 1 << j;
         
         def++;
      }
      
      Lith_SaveWriteChunk(save, Ident_agrp, SaveV_agrp, data, size);
      free(data);
   }
}

//
// Lith_LoadAutoGroups
//
[[__call("ScriptS")]]
static void Lith_LoadAutoGroups(savefile_t *save, memchunk_t *chunk)
{
   autogroupdef_t *data = chunk->data;
   size_t size = chunk->size / sizeof(autogroupdef_t);
   
   for(int def = 0; def < size; def++)
   {
      char unpackedname[12];
      
      // Unpack name.
      for(int i = 0; i < 3; i++)
      {
         unpackedname[i * 4 + 0] = (data[def].name[i] & 0xFF000000) >> 24;
         unpackedname[i * 4 + 1] = (data[def].name[i] & 0x00FF0000) >> 16;
         unpackedname[i * 4 + 2] = (data[def].name[i] & 0x0000FF00) >>  8;
         unpackedname[i * 4 + 3] = (data[def].name[i] & 0x000000FF) >>  0;
      }
      
      // Unpack groups.
      for(int i = 0; i < UPGR_MAX; i++)
      {
         upgrade_t const *upgr = &save->p->upgrades[i];
         
         if(Lith_strcmp_str(unpackedname, upgr->info->name) == 0)
         {
            for(int j = 0; j < NUMAUTOGROUPS; j++)
               upgr->autogroups[j] = data[def].groups & (1 << j);
            
            break;
         }
      }
   }
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
      Lith_SaveAutoGroups(save); ACS_Delay(1);
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
      Lith_LoadChunk(save, Ident_agrp, SaveV_agrp, Lith_LoadAutoGroups);
      Lith_LoadEnd(save);
   }
}

// EOF

