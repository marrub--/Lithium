/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Archived player data handling.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "m_file.h"
#include "p_savedata.h"
#include "w_world.h"

/* Extern Functions -------------------------------------------------------- */

void Save_WriteChunk(struct savefile *save, u32 iden, u32 vers, mem_size_t size)
{
   Dbg_Log(log_save, "Save_WriteChunk: writing %c%c%c%c version %u size %zu",
           (iden >>  0) & 0xFF,
           (iden >>  8) & 0xFF,
           (iden >> 16) & 0xFF,
           (iden >> 24) & 0xFF,
           vers, size);

   struct savechunk chunk = {iden, vers & Save_VersMask, size};
   FWrite32(&chunk, sizeof chunk, 4, save->fp);
}

struct savefile *Save_BeginSave()
{
   struct savefile *save = Salloc(struct savefile, _tag_file);

   if((save->fp = NFOpen(sc_psave, 'w')))
   {
      Save_WriteChunk(save, Ident_Lith, SaveV_Lith, 0);
      return save;
   }

   Dalloc(save);
   return nil;
}

script
void Save_EndSave(struct savefile *save)
{
   Save_WriteChunk(save, Ident_Lend, SaveV_Lend, 0);
   fclose(save->fp);
   Dalloc(save);
}

script
i32 Save_ReadChunk(struct savefile *save, u32 iden, u32 vers, loadchunker_t chunker)
{
   rewind(save->fp);

   Dbg_Log(log_save, "Save_ReadChunk: Finding chunk %c%c%c%c ver%u",
           (iden >>  0) & 0xFF,
           (iden >>  8) & 0xFF,
           (iden >> 16) & 0xFF,
           (iden >> 24) & 0xFF,
           vers);

   for(i32 i = 0;; i++)
   {
      struct savechunk chunk;
      FRead32(&chunk, sizeof chunk, 4, save->fp);

      /* End of file reached, or we reached the EOF chunk. */
      /* Otherwise, if the chunk description matches, process it. */
      /* Or, the chunk is not correct, and we skip the data. */
      if(chunk.iden == Ident_Lend || FEOF(save->fp))
         break;
      else if(chunk.iden == iden && (chunk.vrfl & Save_VersMask) == vers)
      {
         if(chunker) chunker(save, &chunk);

         Dbg_Log(log_save, "Save_ReadChunk: Found valid chunk at %i", i);

         return i;
      }
      else
         fseek(save->fp, chunk.size, SEEK_CUR);
   }

   Dbg_Log(log_save, "Save_ReadChunk: Couldn't find anything");

   return -1;
}

script
struct savefile *Save_BeginLoad()
{
   struct savefile *save = Salloc(struct savefile, _tag_file);

   if((save->fp = NFOpen(sc_psave, 'r')))
   {
      /* The Lith chunk must always be the first valid chunk. */
      if(Save_ReadChunk(save, Ident_Lith, SaveV_Lith) != 0)
      {
         Save_EndLoad(save);
         return nil;
      }

      return save;
   }

   Dalloc(save);
   return nil;
}

script
void Save_EndLoad(struct savefile *save)
{
   fclose(save->fp);
   Dalloc(save);
}

/* EOF */
