// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#if LITHIUM
#include "common.h"
#include "p_player.h"
#include "m_file.h"
#include "p_savedata.h"
#include "w_world.h"

// Extern Functions ----------------------------------------------------------|

void Lith_SaveWriteChunk(struct savefile *save, u32 iden, u32 vers, size_t size)
{
   if(world.dbgSave)
      Log("Lith_SaveWriteChunk: writing %u version %u size %zu", iden, vers, size);

   struct savechunk chunk = {iden, vers & Save_VersMask, size};
   Lith_FWrite32(&chunk, sizeof chunk, 4, save->fp);
}

struct savefile *Lith_SaveBegin(struct player *p)
{
   struct savefile *save = Salloc(struct savefile);

   if((save->fp = Lith_NFOpen(p->num, sc_psave, 'w')))
   {
      save->p = p;
      Lith_SaveWriteChunk(save, Ident_Lith, SaveV_Lith, 0);
      return save;
   }

   Dalloc(save);
   return nil;
}

script
void Lith_SaveEnd(struct savefile *save)
{
   Lith_SaveWriteChunk(save, Ident_Lend, SaveV_Lend, 0);
   fclose(save->fp);
   Dalloc(save);
}

i32 Lith_LoadChunk(struct savefile *save, u32 iden, u32 vers, loadchunker_t chunker)
{
   rewind(save->fp);

   if(world.dbgSave)
      Log("Lith_LoadChunk: Finding chunk %.4X ver%u", iden, vers);

   for(i32 i = 0;; i++)
   {
      struct savechunk chunk;
      Lith_FRead32(&chunk, sizeof chunk, 4, save->fp);

      // End of file reached, or we reached the EOF chunk.
      // Otherwise, if the chunk description matches, process it.
      // Or, the chunk is not correct, and we skip the data.
      if(chunk.iden == Ident_Lend || feof(save->fp))
         break;
      else if(chunk.iden == iden && (chunk.vrfl & Save_VersMask) == vers)
      {
         if(chunker) chunker(save, &chunk);

         if(world.dbgSave) Log("Lith_LoadChunk: Found valid chunk at %i", i);

         return i;
      }
      else
         fseek(save->fp, chunk.size, SEEK_CUR);
   }

   if(world.dbgSave)
      Log("Lith_LoadChunk: Couldn't find anything");

   return -1;
}

struct savefile *Lith_LoadBegin(struct player *p)
{
   struct savefile *save = Salloc(struct savefile);

   if((save->fp = Lith_NFOpen(p->num, sc_psave, 'r')))
   {
      save->p = p;

      // The Lith chunk must always be the first valid chunk.
      if(Lith_LoadChunk(save, Ident_Lith, SaveV_Lith) != 0)
      {
         Lith_LoadEnd(save);
         return nil;
      }

      return save;
   }

   Dalloc(save);
   return nil;
}

void Lith_LoadEnd(struct savefile *save)
{
   fclose(save->fp);
   Dalloc(save);
}
#endif

// EOF
