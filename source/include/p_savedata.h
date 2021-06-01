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
#define Ident_Lith FourCCPtr("Lith")
#define SaveV_Lith 15

/* Lend: Optional. Last valid chunk in save file. */
#define Ident_Lend FourCCPtr("Lend")
#define SaveV_Lend 0

/* note: Notes. */
#define Ident_note FourCCPtr("note")
#define SaveV_note 0

/* agrp: Auto-groups. */
#define Ident_agrp FourCCPtr("agrp")
#define SaveV_agrp 0

/* intr: Intros. */
#define Ident_intr FourCCPtr("intr")
#define SaveV_intr 0

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
};

script typedef void (*loadchunker_t)(struct savefile *save, struct savechunk *chunk);

/* Extern Functions -------------------------------------------------------- */

void Save_WriteChunk(struct savefile *save, u32 iden, u32 vers, size_t size);
struct savefile *Save_BeginSave(void);
script void Save_EndSave(struct savefile *save);

optargs(1) script
i32 Save_ReadChunk(struct savefile *save, u32 iden, u32 vers, loadchunker_t chunker);
script
struct savefile *Save_BeginLoad(void);
script
void Save_EndLoad(struct savefile *save);

#endif
