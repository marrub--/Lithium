// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_file.h"
#include "lith_savedata.h"
#include "lith_world.h"

// Extern Functions ----------------------------------------------------------|

void Lith_SaveWriteChunk(savefile_t *save, ident_t iden, uint32_t vers, size_t size)
{
   if(world.dbgSave)
      Log("Lith_SaveWriteChunk: writing %u version %u size %zu", iden, vers, size);

   savechunk_t chunk = {iden, vers & Save_VersMask, size};
   Lith_FWrite32(&chunk, sizeof chunk, 4, save->fp);
}

savefile_t *Lith_SaveBegin(struct player *p)
{
   savefile_t *save = Salloc(savefile_t);

   if((save->fp = Lith_NFOpen(p->num, "lith_psave", 'w')))
   {
      save->p = p;
      Lith_SaveWriteChunk(save, Ident_Lith, SaveV_Lith, 0);
      return save;
   }

   Dalloc(save);
   return null;
}

script
void Lith_SaveEnd(savefile_t *save)
{
   Lith_SaveWriteChunk(save, Ident_Lend, SaveV_Lend, 0);
   fclose(save->fp);
   Dalloc(save);
}

int Lith_LoadChunk(savefile_t *save, ident_t iden, uint32_t vers, loadchunker_t chunker)
{
   rewind(save->fp);

   if(world.dbgSave)
      Log("Lith_LoadChunk: Finding chunk %.4X ver%u", iden, vers);

   for(int i = 0;; i++)
   {
      savechunk_t chunk;
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

savefile_t *Lith_LoadBegin(struct player *p)
{
   savefile_t *save = Salloc(savefile_t);

   if((save->fp = Lith_NFOpen(p->num, "lith_psave", 'r')))
   {
      save->p = p;

      // The Lith chunk must always be the first valid chunk.
      if(Lith_LoadChunk(save, Ident_Lith, SaveV_Lith) != 0)
      {
         Lith_LoadEnd(save);
         return null;
      }

      return save;
   }

   Dalloc(save);
   return null;
}

void Lith_LoadEnd(savefile_t *save)
{
   fclose(save->fp);
   Dalloc(save);
}

// EOF

