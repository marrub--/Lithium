// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_savedata.h"
#include "lith_file.h"

// Chunk "note" --------------------------------------------------------------|

static u32 note_Len(__str s)
{
   u32 len = ACS_StrLen(s);
   return len > 255 ? 255 : len;
}

script
static void Lith_Save_note(savefile_t *save)
{
   u32 chunklen = 0;

   for(int i = 0; i < countof(save->p->notes); i++)
      chunklen += note_Len(save->p->notes[i]) + 1;

   Lith_SaveWriteChunk(save, Ident_note, SaveV_note, chunklen);

   for(int i = 0; i < countof(save->p->notes); i++)
   {
      u32 len = note_Len(save->p->notes[i]);

      fputc(len, save->fp);
      Lith_FWrite_str(save->p->notes[i], len, save->fp);
   }
}

script
static void Lith_Load_note(savefile_t *save, savechunk_t *chunk)
{
   for(int i = 0; i < countof(save->p->notes); i++)
   {
      u32 len = fgetc(save->fp);

      ACS_BeginPrint();
      for(u32 j = 0; j < len; j++) ACS_PrintChar(fgetc(save->fp) & 0xFF);
      save->p->notes[i] = ACS_EndStrParam();
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

