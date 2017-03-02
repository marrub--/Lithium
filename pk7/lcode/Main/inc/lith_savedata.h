#ifndef LITH_SAVEDATA_H
#define LITH_SAVEDATA_H

#include <stdio.h>
#include <stdint.h>

#define FourCC(a, b, c, d) ((d << 24) | (c << 16) | (b << 8) | (a << 0))

#define Ident_Lith FourCC('L', 'i', 't', 'h')
#define SaveV_Lith 7

#define Ident_Lend FourCC('L', 'e', 'n', 'd')
#define SaveV_Lend 0

#define Ident_agrp FourCC('a', 'g', 'r', 'p')
#define SaveV_agrp 1


//----------------------------------------------------------------------------
// Types
//

typedef uint64_t crc64_t;
typedef uint32_t ident_t;

typedef struct savechunk_s
{
   ident_t  iden;
   uint32_t vers;
   crc64_t  hash;
   size_t   size;
} savechunk_t;

typedef struct memchunk_s
{
   [[__anonymous]] savechunk_t savechunk;
   void *data;
} memchunk_t;

typedef struct savefile_s
{
   FILE *fp;
   struct player_s *p;
} savefile_t;

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

