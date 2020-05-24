/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
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

#define note_len(s) i32 len = s ? strlen(s) : 0; len = min(len, 255)

script static void Save_note(struct savefile *save) {
   u32 chunklen = 0;

   for(i32 i = 0; i < countof(save->p->notes); i++) {
      note_len(save->p->notes[i]);
      chunklen += len + 1;
   }

   Save_WriteChunk(save, Ident_note, SaveV_note, chunklen);

   for(i32 i = 0; i < countof(save->p->notes); i++) {
      note_len(save->p->notes[i]);
      fputc(len, save->fp);
      if(len) fwrite(save->p->notes[i], 1, len, save->fp);
   }
}

script static void Load_note(struct savefile *save, struct savechunk *chunk) {
   for(i32 i = 0; i < countof(save->p->notes); i++) {
      u32 len = fgetc(save->fp);
      if(!len) continue;

      Dalloc(save->p->notes[i]);
      char *n = save->p->notes[i] = Malloc(len + 1, _tag_file);
      for(i32 j = 0; j < len; j++) n[j] = fgetc(save->fp) & 0xFF;
   }
}

/* Chunk "agrp" ------------------------------------------------------------ */

script static void Save_agrp(struct savefile *save) {
   u32 groupnum = 0;

   for(i32 i = 0; i < UPGR_MAX; i++) {
      if(save->p->upgrades[i].agroups) {
         groupnum++;
      }
   }

   Save_WriteChunk(save, Ident_agrp, SaveV_agrp, 2 + groupnum * 13);

   fputc(save->p->autobuy & 0xFF, save->fp);
   fputc(groupnum         & 0xFF, save->fp);

   for(i32 i = 0; i < UPGR_MAX; i++) {
      u32 groups = save->p->upgrades[i].agroups;
      if(groups) {
         fwrite(Upgr_EnumToStr(i), 1, 12, save->fp);
         fputc(groups & 0xFF, save->fp);
      }
   }
}

script static void Load_agrp(struct savefile *save, struct savechunk *chunk) {
   save->p->autobuy = fgetc(save->fp);
   u32 groupnum     = fgetc(save->fp);

   for(i32 i = 0; i < groupnum; i++) {
      char name[12];
      i32  groups;

      fread(name, 1, 12, save->fp);
      groups = fgetc(save->fp);

      u32 num = Upgr_StrToEnum(name);
      if(num < UPGR_MAX && groups != EOF) {
         save->p->upgrades[num].agroups = groups;
      }
   }
}

/* Chunk "intr" ------------------------------------------------------------ */

script static void Save_intr(struct savefile *save) {
   Save_WriteChunk(save, Ident_intr, SaveV_intr, 2);
   fputc(save->p->done_intro      & 0xFF, save->fp);
   fputc(save->p->done_intro >> 8 & 0xFF, save->fp);
}

script static void Load_intr(struct savefile *save, struct savechunk *chunk) {
   i32 n0 = fgetc(save->fp);
   i32 n1 = fgetc(save->fp);
   save->p->done_intro = n0 | n1 << 8;
}

/* Extern Functions -------------------------------------------------------- */

script void P_Data_Save(struct player *p) {
   struct savefile *save;

   if((save = Save_BeginSave(p))) {
      Save_note(save);
      Save_agrp(save);
      Save_intr(save);
      Save_EndSave(save);
   }
}

script void P_Data_Load(struct player *p) {
   struct savefile *save;

   if((save = Save_BeginLoad(p))) {
      Save_ReadChunk(save, Ident_note, SaveV_note, Load_note);
      Save_ReadChunk(save, Ident_agrp, SaveV_agrp, Load_agrp);
      Save_ReadChunk(save, Ident_intr, SaveV_intr, Load_intr);
      Save_EndLoad(save);
   }
}

/* EOF */
