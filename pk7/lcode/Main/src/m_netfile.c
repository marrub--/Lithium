#define _GNU_SOURCE // Needed for fopencookie. See: man 7 feature_test_macros

#include "lith_common.h"
#include "lith_netfile.h"

#include "base64.h"

#define SAVE_BLOCK_SIZE 200


//----------------------------------------------------------------------------
// Type Definitions
//

//
// netfile_t
//
typedef struct netfile_s
{
   __str pcvar;
   int   pnum;
   char  *mem;
   size_t len;
   size_t pos;
} netfile_t;

//
// memfile_t
//
typedef struct memfile_t
{
   unsigned char *mem;
   size_t         len;
   size_t         pos;
} memfile_t;


//----------------------------------------------------------------------------
// Static Functions
//

//
// NetWrite
//
// fwrite for netfiles.
// Basic buffer management.
//
static ssize_t NetWrite(void *nfdata, char const *buf, size_t size)
{
   netfile_t *nf = nfdata;
   
   size_t avail = nf->len - nf->pos;
   
   if(size >= avail)
   {
      size_t len = nf->len + nf->len / 2 + size + 1;
      char *mem = realloc(nf->mem, len);
      
      if(!mem)
         return 0;
      
      nf->len = len;
      nf->mem = mem;
   }
   
   memcpy(nf->mem + nf->pos, buf, size);
   nf->mem[nf->pos += size] = '\0';
   return size;
}

//
// NetClose
//
// fclose for netfiles.
// Output to the CVar with a Base64 representation of the output buffer.
//
static int NetClose(void *nfdata)
{
   netfile_t *nf = nfdata;
   
   // Base64 encode the buffer.
   size_t outsize;
   unsigned char *coded = base64_encode((unsigned char const *)nf->mem, nf->pos, &outsize);
   
   if(coded)
   {
      int cvarnum = 0;
      
      for(unsigned char const *itr = coded; outsize; cvarnum++)
      {
         size_t itrsize;
         
         if(outsize <= SAVE_BLOCK_SIZE)
            itrsize = outsize;
         else
            itrsize = SAVE_BLOCK_SIZE;
         
         ACS_SetUserCVarString(nf->pnum, StrParam("%S_%i", nf->pcvar, cvarnum), StrParam("%.*s", itrsize, itr));
         
         itr     += itrsize;
         outsize -= itrsize;
      }
      
      ACS_SetUserCVarString(nf->pnum, StrParam("%S_%i", nf->pcvar, cvarnum), "");
      
      free(coded);
   }
   else
      ACS_SetUserCVarString(nf->pnum, nf->pcvar, s""); // Oh well!
   
   free(nf->mem);
   free(nf);
   
   return 0;
}

//
// MemRead
//
static ssize_t MemRead(void *memdata, char *buf, size_t size)
{
   memfile_t *mem   = memdata;
   size_t     avail = mem->len - mem->pos;

   if(size > avail)
      size = avail;

   memcpy(buf, mem->mem + mem->pos, size);
   mem->pos += size;
   return size;
}

//
// MemSeek
//
static int MemSeek(void *memdata, off_t *offset, int whence)
{
   memfile_t *mem = memdata;
   size_t     pos;

   switch(whence)
   {
   case SEEK_SET: pos = *offset; break;
   case SEEK_CUR: pos = *offset + mem->pos; break;
   case SEEK_END: pos = *offset + mem->len; break;
   default: return EOF;
   }

   if(pos > mem->len)
      return EOF;

   *offset = mem->pos = pos;

   return 0;
}

//
// MemClose
//
static int MemClose(void *memdata)
{
   memfile_t *mem = memdata;
   free(mem->mem);
   free(mem);
   
   return 0;
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_NFOpen
//
// fopen() equivalent for netfiles.
//
FILE *Lith_NFOpen(int pnum, __str pcvar, char rw)
{
   FILE *fp = null;
   
   if(rw == 'w')
   {
      netfile_t *nf = calloc(1, sizeof(netfile_t));
      
      nf->pcvar = pcvar;
      nf->pnum  = pnum;
      
      fp = fopencookie(nf, c"w", (cookie_io_functions_t){
         .write = NetWrite,
         .close = NetClose
      });
   }
   else if(rw == 'r')
   {
      // Get inputs from all possible CVars.
      char  *input   = null;
      size_t inputsz = 0;
      
      for(int cvarnum;; cvarnum++)
      {
         __str  cvar  = ACS_GetUserCVarString(pnum, StrParam("%S_%i", pcvar, cvarnum));
         size_t inlen = ACS_StrLen(cvar);
         
         if(inlen)
         {
            input = realloc(input, inputsz + inlen + 1);
            Lith_strcpy_str(input + inputsz, cvar);
            
            inputsz += inlen;
         }
         else
            break;
      }
      
      if(input)
      {
         // Decode the base64 input.
         size_t size;
         unsigned char *data = base64_decode((void *)input, inputsz, &size);
         
         free(input);
         
         if(data)
         {
            memfile_t *mem = calloc(1, sizeof(memfile_t));
            
            mem->mem = data;
            mem->len = size;
            
            fp = fopencookie(mem, c"r", (cookie_io_functions_t){
               .read  = MemRead,
               .seek  = MemSeek,
               .close = MemClose,
            });
         }
      }
   }
   
   return fp;
}

//
// Lith_FWrite32
//
// Unpacks integers into a file stream.
//
size_t Lith_FWrite32(void const *ptr, size_t count, size_t bytes, FILE *fp)
{
   size_t res = 0;

   for(unsigned char const *itr = ptr; count--; res += bytes)
   {
      unsigned c = *itr++;
      for(int i = 0; i < bytes; i++)
         fputc((c & (0xFF << (i * 8))) >> (i * 8), fp);
   }

   return res;
}

//
// Lith_FRead32
//
// Reads packed integers from a file stream.
//
size_t Lith_FRead32(void *buf, size_t count, size_t bytes, FILE *fp)
{
   size_t res = 0;

   for(char *itr = buf; count--;)
   {
      int c = 0, t;
      
      for(int i = 0; i < bytes; i++, res++)
      {
         if((t = fgetc(fp)) == EOF)
         {
            *itr = c;
            return res;
         }
         
         c |= (t & 0xFF) << (i * 8);
      }
      
      *itr++ = c;
   }
   
   return res;
}

// EOF

