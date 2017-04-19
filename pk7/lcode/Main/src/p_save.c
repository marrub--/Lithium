#include "lith_common.h"
#include "lith_player.h"
#include "lith_savedata.h"


//----------------------------------------------------------------------------
// Static Functions
//

//
// Lith_Save_agrp
//
[[__call("ScriptS")]]
static void Lith_Save_agrp(savefile_t *save)
{
   bool *any = calloc(save->p->upgrmax, sizeof(bool));
   int total = 0;
   
   for(int i = 0; i < save->p->upgrmax; i++)
   {
      for(int j = 0; j < NUMAUTOGROUPS; j++)
         if(save->p->upgrades[i].autogroups[j]) {any[i] = true; break;}
      
      if(any[i]) total++;
   }
   
   if(total)
   {
      save_autogroup_t *data = calloc(total,  sizeof(save_autogroup_t));
      size_t            size =        total * sizeof(save_autogroup_t);
      
      for(int i = 0, def = 0; i < save->p->upgrmax; i++)
         if(any[i])
      {
         upgrade_t const *upgr = &save->p->upgrades[i];
         
         // Pack the name into three 4-byte values.
         for(int j = 0; j < 3; j++)
         {
            data[def].name[j] |= upgr->info->name[j * 4 + 0] <<  0;
            data[def].name[j] |= upgr->info->name[j * 4 + 1] <<  8;
            data[def].name[j] |= upgr->info->name[j * 4 + 2] << 16;
            data[def].name[j] |= upgr->info->name[j * 4 + 3] << 24;
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
   
   free(any);
}

//
// Lith_Load_agrp
//
[[__call("ScriptS")]]
static void Lith_Load_agrp(savefile_t *save, memchunk_t *chunk)
{
   save_autogroup_t *data = chunk->data;
   size_t size = chunk->size / sizeof(save_autogroup_t);
   
   for(int def = 0; def < size; def++)
   {
      char unpackedname[12];
      
      // Unpack name.
      for(int i = 0; i < 3; i++)
      {
         unpackedname[i * 4 + 0] = (data[def].name[i] & 0x000000FF) >>  0;
         unpackedname[i * 4 + 1] = (data[def].name[i] & 0x0000FF00) >>  8;
         unpackedname[i * 4 + 2] = (data[def].name[i] & 0x00FF0000) >> 16;
         unpackedname[i * 4 + 3] = (data[def].name[i] & 0xFF000000) >> 24;
      }
      
      // Unpack groups.
      for(int i = 0; i < save->p->upgrmax; i++)
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
      Lith_SaveWriteChunk(save, Ident_abuy, SaveV_abuy, &(save_autobuy_t){groups}, sizeof(save_autobuy_t));
}

//
// Lith_Load_abuy
//
[[__call("ScriptS")]]
static void Lith_Load_abuy(savefile_t *save, memchunk_t *chunk)
{
   if(chunk->size != sizeof(save_autobuy_t)) return;
   
   save_autobuy_t *data = chunk->data;
   
   for(int i = 0; i < NUMAUTOGROUPS; i++)
      save->p->autobuy[i] = (data->groups & (1 << i)) != 0;
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

