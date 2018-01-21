// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_savedata.h"
#include "lith_file.h"

// Static Functions ----------------------------------------------------------|

//
// note_Len
//
static unsigned note_Len(__str s)
{
   unsigned len = ACS_StrLen(s);
   return len > 255 ? 255 : len;
}

//
// Lith_Save_note
//
[[__call("ScriptS")]]
static void Lith_Save_note(savefile_t *save)
{
   unsigned chunklen = 0;

   for(int i = 0; i < countof(save->p->notes); i++)
      chunklen += note_Len(save->p->notes[i]) + 1;

   Lith_SaveWriteChunk(save, Ident_note, SaveV_note, chunklen);

   for(int i = 0; i < countof(save->p->notes); i++)
   {
      unsigned len = note_Len(save->p->notes[i]);

      fputc(len, save->fp);
      Lith_FWrite_str(save->p->notes[i], len, save->fp);
   }
}

//
// Lith_Load_note
//
[[__call("ScriptS")]]
static void Lith_Load_note(savefile_t *save, savechunk_t *chunk)
{
   for(int i = 0; i < countof(save->p->notes); i++)
   {
      unsigned len = fgetc(save->fp);

      ACS_BeginPrint();
      for(unsigned j = 0; j < len; j++) ACS_PrintChar(fgetc(save->fp) & 0xFF);
      save->p->notes[i] = ACS_EndStrParam();

      Log("note %i: %S", i, save->p->notes[i]);
   }
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_PlayerSaveData
//
[[__call("ScriptS")]]
void Lith_PlayerSaveData(player_t *p)
{
   savefile_t *save;

   if((save = Lith_SaveBegin(p)))
   {
      Lith_Save_note(save);
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
      Lith_LoadChunk(save, Ident_note, SaveV_note, Lith_Load_note);
      Lith_LoadEnd(save);
   }
}

// EOF

