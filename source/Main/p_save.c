// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// p_save.c: Save file loading/saving.

#if LITHIUM
#include "common.h"
#include "p_player.h"
#include "p_savedata.h"
#include "m_file.h"

// Chunk "note" --------------------------------------------------------------|

#define note_Len(s) __with(i32 len = s ? strlen(s) : 0; len = min(len, 255);)

script
static void Lith_Save_note(struct savefile *save)
{
   u32 chunklen = 0;

   for(i32 i = 0; i < countof(save->p->notes); i++)
      note_Len(save->p->notes[i]) chunklen += len + 1;

   Lith_SaveWriteChunk(save, Ident_note, SaveV_note, chunklen);

   for(i32 i = 0; i < countof(save->p->notes); i++)
      note_Len(save->p->notes[i])
   {
      fputc(len, save->fp);
      if(len) Lith_FWrite(save->p->notes[i], len, save->fp);
   }
}

script
static void Lith_Load_note(struct savefile *save, struct savechunk *chunk)
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

// Chunk "fun0" --------------------------------------------------------------|

script
static void Lith_Save_fun0(struct savefile *save)
{
   Lith_SaveWriteChunk(save, Ident_fun0, SaveV_fun0, 1);

   fputc(save->p->fun, save->fp);
}

script
static void Lith_Load_fun0(struct savefile *save, struct savechunk *chunk)
{
   save->p->fun = fgetc(save->fp);
}

// Extern Functions ----------------------------------------------------------|

script_str ext("ACS")
void Lith_SetFun(i32 fun)
{
   withplayer(LocalPlayer)
   {
      p->fun = fun;
      p->saveData();
   }
}

script
void Lith_PlayerSaveData(struct player *p)
{
   struct savefile *save;

   if((save = Lith_SaveBegin(p)))
   {
      Lith_Save_note(save);
      Lith_Save_fun0(save);
      Lith_SaveEnd(save);
   }
}

script
void Lith_PlayerLoadData(struct player *p)
{
   struct savefile *save;

   if((save = Lith_LoadBegin(p)))
   {
      Lith_LoadChunk(save, Ident_note, SaveV_note, Lith_Load_note);
      Lith_LoadChunk(save, Ident_fun0, SaveV_fun0, Lith_Load_fun0);
      Lith_LoadEnd(save);
   }
}
#endif

// EOF
