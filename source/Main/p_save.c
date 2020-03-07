/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Save file loading/saving.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "p_savedata.h"
#include "m_file.h"

/* Chunk "note" ------------------------------------------------------------ */

#define note_Len(s) __with(i32 len = s ? strlen(s) : 0; len = min(len, 255);)

script
static void Save_note(struct savefile *save)
{
   u32 chunklen = 0;

   for(i32 i = 0; i < countof(save->p->notes); i++)
      note_Len(save->p->notes[i]) chunklen += len + 1;

   Save_WriteChunk(save, Ident_note, SaveV_note, chunklen);

   for(i32 i = 0; i < countof(save->p->notes); i++)
      note_Len(save->p->notes[i])
   {
      fputc(len, save->fp);
      if(len) FWrite(save->p->notes[i], len, save->fp);
   }
}

script
static void Load_note(struct savefile *save, struct savechunk *chunk)
{
   for(i32 i = 0; i < countof(save->p->notes); i++)
   {
      u32 len = fgetc(save->fp);
      if(!len) continue;

      Dalloc(save->p->notes[i]);
      char *n = save->p->notes[i] = Nalloc(len + 1);
      for(i32 j = 0; j < len; j++) n[j] = fgetc(save->fp) & 0xFF;
   }
}

/* Chunk "fun0" ------------------------------------------------------------ */

script
static void Save_fun0(struct savefile *save)
{
   Save_WriteChunk(save, Ident_fun0, SaveV_fun0, 1);

   fputc(save->p->fun, save->fp);
}

script
static void Load_fun0(struct savefile *save, struct savechunk *chunk)
{
   save->p->fun = fgetc(save->fp);
}

/* Extern Functions -------------------------------------------------------- */

script
void P_Data_Save(struct player *p)
{
   struct savefile *save;

   if((save = Save_BeginSave(p)))
   {
      Save_note(save);
      Save_fun0(save);
      Save_EndSave(save);
   }
}

script
void P_Data_Load(struct player *p)
{
   struct savefile *save;

   if((save = Save_BeginLoad(p)))
   {
      Save_ReadChunk(save, Ident_note, SaveV_note, Load_note);
      Save_ReadChunk(save, Ident_fun0, SaveV_fun0, Load_fun0);
      Save_EndLoad(save);
   }
}

/* EOF */
