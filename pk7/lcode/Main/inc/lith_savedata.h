#ifndef LITH_SAVEDATA_H
#define LITH_SAVEDATA_H

#include <stdio.h>
#include <stdint.h>

#define FourCC(a, b, c, d) ((d << 24) | (c << 16) | (b << 8) | (a << 0))

// Lith: First valid chunk in save file. Version is always 7.
#define Ident_Lith FourCC('L', 'i', 't', 'h')
#define SaveV_Lith 7

// Lend: Not required. Could be used to have extra data before EOF.
#define Ident_Lend FourCC('L', 'e', 'n', 'd')
#define SaveV_Lend 0

// agrp: Auto-group information.
// Ver. 1: Initial data set. Stores string index and bit field.
// Ver. 2: Fixed names being packed backwards.
#define Ident_agrp FourCC('a', 'g', 'r', 'p')
#define SaveV_agrp 2

// abuy: Auto-buy information.
// Ver. 1: Initial data set. Stores a single bitfield.
#define Ident_abuy FourCC('a', 'b', 'u', 'y')
#define SaveV_abuy 1

#define Save_VersMask 0x0000007F
#define Save_FlagMask 0xFFFFFF80
#define Save_FlagShft 7


//----------------------------------------------------------------------------
// Types
//

typedef uint64_t crc64_t;
typedef uint32_t ident_t;

//
// saveflag_e
//
enum saveflag_e
{
   SF_SkipHash = 1 << 0, // skips CRC-64 hash check when loading chunk
};

//
// savechunk_t
//
typedef struct savechunk_s
{
   ident_t  iden; // four char identifier
   uint32_t vrfl; // version (lower 7 bits) + flags (upper 24 bits)
   crc64_t  hash; // CRC-64 hash
   uint32_t size; // size of chunk data in words
} savechunk_t;

//
// memchunk_t
//
typedef struct memchunk_s
{
   [[__anonymous]]
   savechunk_t     savechunk;
   uint32_t        vers;
   enum saveflag_e flag;
   void           *data;
} memchunk_t;

//
// savefile_t
//
typedef struct savefile_s
{
   FILE *fp;
   struct player_s *p;
} savefile_t;

//
// save_autogroup_t
//
typedef struct save_autogroup_s
{
   uint32_t name[3];
   uint32_t groups;
} save_autogroup_t;

//
// save_autobuy_t
//
typedef struct save_autobuy_s
{
   uint32_t groups;
} save_autobuy_t;

[[__call("ScriptS")]] typedef void (*loadchunker_t)(savefile_t *save, memchunk_t *chunk);


//----------------------------------------------------------------------------
// Extern Functions
//

[[__optional_args(1)]]
crc64_t Lith_CRC64(void const *data, size_t len, crc64_t result);

[[__optional_args(3)]]
void Lith_SaveWriteChunk(savefile_t *save, ident_t iden, uint32_t vers, void const *data, size_t size);
savefile_t *Lith_SaveBegin(struct player_s *p);
[[__call("ScriptS")]] void Lith_SaveEnd(savefile_t *save);

[[__optional_args(1)]]
int Lith_LoadChunk(savefile_t *save, ident_t iden, uint32_t vers, loadchunker_t chunker);
savefile_t *Lith_LoadBegin(struct player_s *p);
void Lith_LoadEnd(savefile_t *save);

#endif

