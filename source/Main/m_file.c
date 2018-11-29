// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#define _GNU_SOURCE // Needed for fopencookie. See: man 7 feature_test_macros

#include "lith_common.h"
#include "lith_file.h"
#include "lith_world.h"

#include "lith_base64.h"

#include <ctype.h>

#define SAVE_BLOCK_SIZE 230

// Type Definitions ----------------------------------------------------------|

typedef struct memfile_t
{
   byte  *mem;
   size_t len;
   size_t pos;
} memfile_t;

typedef struct netfile_s
{
   anonymous
   memfile_t memfile;
   __str     pcvar;
   int       pnum;
} netfile_t;

// Static Functions ----------------------------------------------------------|

void PrintMem(byte const *data, size_t size)
{
   int termpos = 0;

   for(size_t i = 0; i < size; i++)
   {
      if(termpos + 3 > 79)
      {
         printf(c"\n");
         termpos = 0;
      }

      if(isprint(data[i])) printf(c"%c  ",  data[i]);
      else                 printf(c"%.2X ", data[i]);

      termpos += 3;
   }

   printf(c"\nEOF\n\n");
}

// fclose for netfiles.
// Output to the CVar with a Base64 representation of the output buffer.
static int NetClose(void *nfdata)
{
   netfile_t *nf = nfdata;

   // If debugging, print out information about the buffer being written.
   if(world.dbgSave)
   {
      printf(c"NetClose: Writing netfile \"%S\" (%zub)\n", nf->pcvar, nf->pos);
      printf(c"Data follows\n");
      PrintMem((void *)nf->mem, nf->pos);
   }

   // Base64 encode the buffer.
   size_t outsize;
   byte *coded = base64_encode((void *)nf->mem, nf->pos, &outsize);

   if(coded)
   {
      int cvarnum = 0;

      for(byte const *itr = coded; outsize; cvarnum++)
      {
         size_t itrsize;

         if(outsize <= SAVE_BLOCK_SIZE)
            itrsize = outsize;
         else
            itrsize = SAVE_BLOCK_SIZE;

         ACS_SetUserCVarString(nf->pnum, StrParam("%S_%i", nf->pcvar, cvarnum), l_strndup(itr, itrsize));

         itr     += itrsize;
         outsize -= itrsize;
      }

      ACS_SetUserCVarString(nf->pnum, StrParam("%S_%i", nf->pcvar, cvarnum), "");

      Dalloc(coded);
   }

   Dalloc(nf->mem);
   Dalloc(nf);

   return 0;
}

static ssize_t MemRead(void *memdata, char *buf, size_t size)
{
   memfile_t *mem   = memdata;
   size_t     avail = mem->len - mem->pos;

   if(size > avail)
      size = avail;

   memmove(buf, mem->mem + mem->pos, size);
   mem->pos += size;
   return size;
}

static ssize_t MemWrite(void *memdata, char const *buf, size_t size)
{
   memfile_t *mem = memdata;
   size_t avail = mem->len - mem->pos;

   if(size >= avail)
   {
      size_t len = mem->len + mem->len / 2 + size + 1;
      void  *newmem = Ralloc(mem->mem, len);

      if(!mem)
         return 0;

      mem->len = len;
      mem->mem = newmem;
   }

   memmove(mem->mem + mem->pos, buf, size);
   mem->mem[mem->pos += size] = '\0';
   return size;
}

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

static int MemClose(void *memdata)
{
   memfile_t *mem = memdata;
   Dalloc(mem->mem);
   Dalloc(mem);

   return 0;
}

// Extern Functions ----------------------------------------------------------|

FILE *W_Open(__str fname, char const *rw)
{
   __str f;
   ifw(int lmp = W_Find(fname), lmp == -1)
      return null;
   else
      f = W_Read(lmp);
   return __fmemopen_str(f, ACS_StrLen(f), rw);
}

// fopen() equivalent for netfiles.
FILE *Lith_NFOpen(int pnum, __str pcvar, char rw)
{
   FILE *fp = null;

   if(rw == 'w')
   {
      netfile_t *nf = Salloc(netfile_t);

      nf->pcvar = pcvar;
      nf->pnum  = pnum;

      fp = fopencookie(nf, c"w", (cookie_io_functions_t){
         .write = MemWrite,
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
            input = Ralloc(input, inputsz + inlen + 1);
            lstrcpy_str(input + inputsz, cvar);

            inputsz += inlen;
         }
         else
            break;
      }

      if(input)
      {
         // Decode the base64 input.
         size_t size;
         byte *data = base64_decode((void *)input, inputsz, &size);

         Dalloc(input);

         // If debugging, print out information about the buffer being read.
         if(world.dbgSave)
         {
            printf(c"Lith_NFOpen: Opening memfile \"%S\" (%zub)\n", pcvar, size);
            printf(c"Data follows\n");
            PrintMem(data, size);
         }

         if(data)
         {
            memfile_t *mem = Salloc(memfile_t);

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

// Unpacks integers into a file stream.
size_t Lith_FWrite32(void const *restrict ptr, size_t count, size_t bytes, FILE *restrict fp)
{
   size_t res = 0;

   for(byte const *itr = ptr; count--; res += bytes)
   {
      u32 c = *itr++;
      for(int i = 0; i < bytes; i++)
         if(fputc((c & (0xFF << (i * 8))) >> (i * 8), fp) == EOF)
            return res;
   }

   return res;
}

size_t Lith_FWrite(void const *restrict ptr, size_t count, FILE *restrict fp)
{
   size_t res = 0;

   for(char const *itr = ptr; count--; res++)
      if(fputc(*itr++, fp) == EOF) return res;

   return res;
}

size_t Lith_FWrite_str(void const __str_ars *restrict ptr, size_t count, FILE *restrict fp)
{
   size_t res = 0;

   for(char const __str_ars *itr = ptr; count--; res++)
      if(fputc(*itr++, fp) == EOF) return res;

   return res;
}

// Reads packed integers from a file stream.
size_t Lith_FRead32(void *restrict buf, size_t count, size_t bytes, FILE *restrict fp)
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

