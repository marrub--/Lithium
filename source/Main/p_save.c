// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_savedata.h"
#include "lith_file.h"

// Chunk "note" --------------------------------------------------------------|

#define note_Len(s) __with(int len = s ? strlen(s) : 0; len = min(len, 255);)

script
static void Lith_Save_note(savefile_t *save)
{
   u32 chunklen = 0;

   for(int i = 0; i < countof(save->p->notes); i++)
      note_Len(save->p->notes[i]) chunklen += len + 1;

   Lith_SaveWriteChunk(save, Ident_note, SaveV_note, chunklen);

   for(int i = 0; i < countof(save->p->notes); i++)
      note_Len(save->p->notes[i])
   {
      fputc(len, save->fp);
      if(len) Lith_FWrite(save->p->notes[i], len, save->fp);
   }
}

script
static void Lith_Load_note(savefile_t *save, savechunk_t *chunk)
{
   for(int i = 0; i < countof(save->p->notes); i++)
   {
      u32 len = fgetc(save->fp);
      if(!len) continue;

      Dalloc(save->p->notes[i]);
      char *n = save->p->notes[i] = Nalloc(len + 1);
      for(int j = 0; j < len; j++) n[j] = fgetc(save->fp) & 0xFF;
   }
}

// Chunk "fun0" --------------------------------------------------------------|

script
static void Lith_Save_fun0(savefile_t *save)
{
   Lith_SaveWriteChunk(save, Ident_fun0, SaveV_fun0, 1);

   fputc(save->p->fun, save->fp);
}

script
static void Lith_Load_fun0(savefile_t *save, savechunk_t *chunk)
{
   save->p->fun = fgetc(save->fp);
}

// Extern Functions ----------------------------------------------------------|

script ext("ACS")
void Lith_SetFun(int fun)
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
   savefile_t *save;

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
   savefile_t *save;

   if((save = Lith_LoadBegin(p)))
   {
      Lith_LoadChunk(save, Ident_note, SaveV_note, Lith_Load_note);
      Lith_LoadChunk(save, Ident_fun0, SaveV_fun0, Lith_Load_fun0);
      Lith_LoadEnd(save);
   }
}

// EOF

