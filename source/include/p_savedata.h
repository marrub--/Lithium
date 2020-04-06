/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Save file format.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef p_savedata_h
#define p_savedata_h

#include <stdio.h>
#include <stdint.h>

/* Lith: First valid chunk in save file. */
/* Ver. 7:  Initial version. */
/* Ver. 14: Rewrite of save system. */
/* Ver. 15: Chunk format change. */
#define Ident_Lith FourCC('L', 'i', 't', 'h')
#define SaveV_Lith 15

/* Lend: Optional. Last valid chunk in save file. */
#define Ident_Lend FourCC('L', 'e', 'n', 'd')
#define SaveV_Lend 0

/* note: Notes. */
#define Ident_note FourCC('n', 'o', 't', 'e')
#define SaveV_note 0

/* agrp: Auto-groups. */
#define Ident_agrp FourCC('a', 'g', 'r', 'p')
#define SaveV_agrp 0

#define Save_VersMask 0x000000FF
#define Save_FlagMask 0xFFFFFF00
#define Save_FlagShft 8

/* Types ------------------------------------------------------------------- */

struct savechunk {
   u32 iden; /* four char identifier */
   u32 vrfl; /* version (lower 8 bits) + flags (upper 24 bits) */
   u32 size; /* size of chunk data in bytes */
};

struct savefile {
   FILE *fp;
   struct player *p;
};

script typedef void (*loadchunker_t)(struct savefile *save, struct savechunk *chunk);

/* Extern Functions -------------------------------------------------------- */

void Save_WriteChunk(struct savefile *save, u32 iden, u32 vers, size_t size);
struct savefile *Save_BeginSave(struct player *p);
script void Save_EndSave(struct savefile *save);

optargs(1)
i32 Save_ReadChunk(struct savefile *save, u32 iden, u32 vers, loadchunker_t chunker);
struct savefile *Save_BeginLoad(struct player *p);
void Save_EndLoad(struct savefile *save);

#endif
