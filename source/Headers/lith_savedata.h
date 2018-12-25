// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifndef lith_savedata_h
#define lith_savedata_h

#include <stdio.h>
#include <stdint.h>

#define FourCC(a, b, c, d) ((d << 24) | (c << 16) | (b << 8) | (a << 0))

// Lith: First valid chunk in save file.
// Ver. 7:  Initial version.
// Ver. 14: Rewrite of save system.
// Ver. 15: Chunk format change.
#define Ident_Lith FourCC('L', 'i', 't', 'h')
#define SaveV_Lith 15

// Lend: Optional. Last valid chunk in save file.
#define Ident_Lend FourCC('L', 'e', 'n', 'd')
#define SaveV_Lend 0

// note: Player's notes.
#define Ident_note FourCC('n', 'o', 't', 'e')
#define SaveV_note 0

// fun0
#define Ident_fun0 FourCC('f', 'u', 'n', '\0')
#define SaveV_fun0 0

#define Save_VersMask 0x000000FF
#define Save_FlagMask 0xFFFFFF00
#define Save_FlagShft 8

// Types ---------------------------------------------------------------------|

typedef uint32_t ident_t;

typedef struct savechunk_s
{
   ident_t  iden; // four char identifier
   uint32_t vrfl; // version (lower 8 bits) + flags (upper 24 bits)
   uint32_t size; // size of chunk data in bytes
} savechunk_t;

typedef struct savefile_s
{
   FILE *fp;
   struct player *p;
} savefile_t;

script typedef void (*loadchunker_t)(savefile_t *save, savechunk_t *chunk);

// Extern Functions ----------------------------------------------------------|

void Lith_SaveWriteChunk(savefile_t *save, ident_t iden, uint32_t vers, size_t size);
savefile_t *Lith_SaveBegin(struct player *p);
script void Lith_SaveEnd(savefile_t *save);

optargs(1)
int Lith_LoadChunk(savefile_t *save, ident_t iden, uint32_t vers, loadchunker_t chunker);
savefile_t *Lith_LoadBegin(struct player *p);
void Lith_LoadEnd(savefile_t *save);

#endif

