#include "lith_common.h"
#include "lith_player.h"
#include "lith_netfile.h"
#include "lith_savedata.h"


//----------------------------------------------------------------------------
// Static Objects
//

// Most CRC tables are 256 in length because they're on plebeian platforms
// that don't have the glorious 32-bit byte, but we have better!
// ;_;
static crc64_t crctable[512];


//----------------------------------------------------------------------------
// Static Functions
//

//
// InitCRC64
//
static void InitCRC64()
{
   crc64_t const polynomial = 0xC96C5795D7870F42; // ECMA 182
   
   for(crc64_t i = 0; i < 512; i++)
   {
      crc64_t remainder = i;
      
      for(int j = 0; j < 8; j++)
         if(remainder & 1)
            remainder = (remainder >> 1) ^ polynomial;
         else
            remainder >>= 1;
      
      crctable[i] = remainder;
   }
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_CRC64
//
crc64_t Lith_CRC64(void const *data, size_t len, crc64_t result)
{
   static bool crcinit;
   unsigned char const *ptr = data;
   
   if(!crcinit)
      crcinit = true, InitCRC64();
   
   result = ~result;
   
   for(size_t i = 0; i < len; i++)
      result = crctable[(result ^ ptr[i]) & 0x1FF] ^ (result >> 8);
   
   return ~result;
}

//
// Lith_SaveWriteChunk
//
void Lith_SaveWriteChunk(savefile_t *save, ident_t iden, uint32_t vers,
   void const *data, size_t size)
{
   savechunk_t chunk = {iden, vers & 0x7F};
   
   if(data)
   {
      chunk.size = size;
      chunk.hash = Lith_CRC64(data, chunk.size);
   }
   else
      chunk.hash = ~chunk.hash;
   
            Lith_FWrite32(&chunk, sizeof(chunk), 4, save->fp);
   if(data) Lith_FWrite32(data,   chunk.size,    4, save->fp);
}

//
// Lith_SaveBegin
//
savefile_t *Lith_SaveBegin(player_t *p)
{
   savefile_t *save = calloc(1, sizeof(savefile_t));
   
   if((save->fp = Lith_NFOpen(p->number, "lith_player_savedata", 'w')))
   {
      save->p = p;
      
      Lith_SaveWriteChunk(save, Ident_Lith, SaveV_Lith);
      
      return save;
   }
   
   free(save);
   return null;
}

//
// Lith_SaveEnd
//
[[__call("ScriptS")]]
void Lith_SaveEnd(savefile_t *save)
{
   Lith_SaveWriteChunk(save, Ident_Lend, SaveV_Lend);
   
   fclose(save->fp);
   free(save);
}

//
// Lith_LoadChunk
//
int Lith_LoadChunk(savefile_t *save, ident_t iden, uint32_t vers, loadchunker_t chunker)
{
   rewind(save->fp);
   
   if(ACS_GetCVar("__lith_debug_save"))
      Log("Lith_LoadChunk: Finding chunk %.4X ver%u", iden, vers);
   
   for(int i = 0;; i++)
   {
      savechunk_t chunk;
      Lith_FRead32(&chunk, sizeof(chunk), 4, save->fp);
      
      // End of file reached, or we reached the EOF chunk.
      if(chunk.iden == Ident_Lend || feof(save->fp))
         break;
      
      // If the chunk description matches, process it.
      else if(chunk.iden == iden && (chunk.vrfl & Save_VersMask) == vers)
      {
         if(chunk.size)
         {
            unsigned char *data;
            
            data = calloc(1, chunk.size);
            Lith_FRead32(data, chunk.size, 4, save->fp);
            
            memchunk_t mchnk = {.savechunk = chunk, .data = data};
            
            mchnk.vers = (chunk.vrfl & Save_VersMask);
            mchnk.flag = (chunk.vrfl & Save_FlagMask) >> Save_FlagShft;
            
            if(!(mchnk.flag & SF_SkipHash))
            {
               // Check the hash validity. If it isn't valid, skip the chunk.
               crc64_t hash = Lith_CRC64(data, chunk.size);
               
               if(chunk.hash != hash)
               {
                  if(ACS_GetCVar("__lith_debug_save"))
                     Log("Lith_LoadChunk: Hash check failed");
                  
                  free(data);
                  continue;
               }
            }
            
            else if(ACS_GetCVar("__lith_debug_save"))
               Log("Lith_LoadChunk: Skipping hash check");
            
            if(chunker)
               chunker(save, &mchnk);
            
            free(data);
         }
         
         if(ACS_GetCVar("__lith_debug_save"))
            Log("Lith_LoadChunk: Found valid chunk at %i", i);
         
         return i;
      }
      
      // Chunk not correct, skip data
      else
         fseek(save->fp, chunk.size * 4, SEEK_CUR);
   }
   
   if(ACS_GetCVar("__lith_debug_save"))
      Log("Lith_LoadChunk: Couldn't find anything");
   
   return -1;
}

//
// Lith_LoadBegin
//
savefile_t *Lith_LoadBegin(player_t *p)
{
   savefile_t *save = calloc(1, sizeof(savefile_t));
   
   if((save->fp = Lith_NFOpen(p->number, "lith_player_savedata", 'r')))
   {
      save->p = p;
      
      // The Lith chunk must always be the first valid chunk.
      if(Lith_LoadChunk(save, Ident_Lith, SaveV_Lith) != 0)
      {
         Lith_LoadEnd(save);
         return null;
      }
      
      return save;
   }
   
   free(save);
   return null;
}

//
// Lith_LoadEnd
//
void Lith_LoadEnd(savefile_t *save)
{
   fclose(save->fp);
   free(save);
}

// EOF

