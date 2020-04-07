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

void Save_WriteChunk(struct savefile *save, u32 iden, u32 vers, size_t size)
{
   if(get_bit(dbgflag, dbgf_save))
      Log("Save_WriteChunk: writing %u version %u size %zu", iden, vers, size);

   struct savechunk chunk = {iden, vers & Save_VersMask, size};
   FWrite32(&chunk, sizeof chunk, 4, save->fp);
}

struct savefile *Save_BeginSave(struct player *p)
{
   struct savefile *save = Salloc(struct savefile);

   if((save->fp = NFOpen(p->num, sc_psave, 'w')))
   {
      save->p = p;
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

i32 Save_ReadChunk(struct savefile *save, u32 iden, u32 vers, loadchunker_t chunker)
{
   rewind(save->fp);

   if(get_bit(dbgflag, dbgf_save))
      Log("Save_ReadChunk: Finding chunk %.4X ver%u", iden, vers);

   for(i32 i = 0;; i++)
   {
      struct savechunk chunk;
      FRead32(&chunk, sizeof chunk, 4, save->fp);

      /* End of file reached, or we reached the EOF chunk. */
      /* Otherwise, if the chunk description matches, process it. */
      /* Or, the chunk is not correct, and we skip the data. */
      if(chunk.iden == Ident_Lend || feof(save->fp))
         break;
      else if(chunk.iden == iden && (chunk.vrfl & Save_VersMask) == vers)
      {
         if(chunker) chunker(save, &chunk);

         if(get_bit(dbgflag, dbgf_save)) Log("Save_ReadChunk: Found valid chunk at %i", i);

         return i;
      }
      else
         fseek(save->fp, chunk.size, SEEK_CUR);
   }

   if(get_bit(dbgflag, dbgf_save))
      Log("Save_ReadChunk: Couldn't find anything");

   return -1;
}

struct savefile *Save_BeginLoad(struct player *p)
{
   struct savefile *save = Salloc(struct savefile);

   if((save->fp = NFOpen(p->num, sc_psave, 'r')))
   {
      save->p = p;

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

void Save_EndLoad(struct savefile *save)
{
   fclose(save->fp);
   Dalloc(save);
}

/* EOF */
