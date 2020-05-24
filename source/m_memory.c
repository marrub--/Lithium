/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Memory allocations.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"

#define _mem_idn 0x71711177
#define _mem_beg 0xADEADBED
#define _mem_frg 64
#define _mem_siz 268435456

#define _pls_siz 67108864

enum {
   _tag_free,
   _tag_static,
};

struct mem_blk {
   struct mem_blk *prv, *nxt;
   size_t siz;
   u32 tag, idn;
   char dat[];
};

struct mem_top {
   struct mem_blk beg, *cur;
   char dat[];
};

noinit static
char mem_dat[_mem_siz];

static
struct mem_top *mem_top;

noinit static
char pls_stk[_pls_siz], *pls_cur;

stkcall static
void AllocInit(void) {
   mem_top = (void *)mem_dat;

   mem_top->beg.prv = mem_top->beg.nxt = mem_top->cur = (void *)mem_top->dat;
   mem_top->beg.tag = _tag_static;
   mem_top->beg.idn = _mem_beg;

   mem_top->cur->prv = mem_top->cur->nxt = &mem_top->beg;
   mem_top->cur->tag = _tag_free;
   mem_top->cur->siz = sizeof(mem_dat) - sizeof(struct mem_top);
}

stkcall static
struct mem_blk *MergeAdjacent(struct mem_blk *blk, struct mem_blk *adj) {
   adj->siz += blk->siz;
   adj->nxt = blk->nxt;
   adj->nxt->prv = adj;

   if(blk == mem_top->cur) {
      mem_top->cur = adj;
   }

   return adj;
}

#ifndef NDEBUG
stkcall static
struct mem_blk *CheckUsedBlock(struct mem_blk *blk, cstr func) {
   if((void *)blk < (void *)mem_dat ||
      (void *)blk > (void *)(mem_dat + sizeof mem_dat)) {
      Str(err, s" ERROR: out of bounds block ");
      ACS_BeginPrint();
      PrintChrSt(func);
      ACS_PrintString(err);
      ACS_PrintHex((uintptr_t)blk);
      ACS_EndLog();
      return nil;
   }

   if(blk->idn != _mem_idn) {
      Str(err, s" ERROR: invalid identifier for ");
      ACS_BeginPrint();
      PrintChrSt(func);
      ACS_PrintString(err);
      ACS_PrintHex((uintptr_t)blk);
      ACS_EndLog();
      return nil;
   }

   if(blk->tag == _tag_free) {
      Str(err, s" ERROR: already freed ");
      ACS_BeginPrint();
      PrintChrSt(func);
      ACS_PrintString(err);
      ACS_PrintHex((uintptr_t)blk);
      ACS_EndLog();
      return nil;
   }

   return blk;
}
#else
#define CheckUsedBlock(blk, func) blk
#endif

#define GetBlock(p) \
   ((struct mem_blk *)((char *)p - sizeof(struct mem_blk)))

stkcall
void Dalloc(register void *p) {
   register struct mem_blk *blk;

   if(!p) {
      return;
   }

   #ifndef NDEBUG
   if(!mem_top) {
      Str(err, s"Dalloc ERROR: memory not initialized but freeing pointer");
      ACS_BeginPrint();
      ACS_PrintString(err);
      ACS_EndLog();
      return;
   }
   #endif

   blk = CheckUsedBlock(GetBlock(p), __func__);
   blk->tag = _tag_free;

   if(blk->prv->tag == _tag_free) blk = MergeAdjacent(blk, blk->prv);
   if(blk->nxt->tag == _tag_free)       MergeAdjacent(blk->nxt, blk);
}

stkcall
void *Malloc(register size_t rs) {
   /* TODO: tagging */
   register struct mem_blk *cur, *blk, *end;
   register size_t s = rs + sizeof(struct mem_blk);

   if(!mem_top) {
      AllocInit();
   }

   end = (cur = blk = mem_top->cur)->prv;

   do {
      if(cur == end) {
         /* well, no more memory to check...
          * don't worry, this won't ever really happen, right?
          */
         #ifndef NDEBUG
         Str(err, s"Malloc ERROR: out of memory - couldn't allocate ");
         ACS_BeginPrint();
         ACS_PrintString(err);
         ACS_PrintInt(s);
         ACS_EndLog();
         #endif
         return nil;
      } else if(cur->tag != _tag_free) {
         blk = cur = cur->nxt;
      } else {
         cur = cur->nxt;
      }
   } while(blk->tag != _tag_free || blk->siz < s);

   register size_t rest = blk->siz - s;

   if(rest > _mem_frg) {
      register struct mem_blk *nxt = (void *)&blk->dat[rs];

      nxt->prv = blk;
      nxt->nxt = blk->nxt;
      nxt->nxt->prv = nxt;
      nxt->siz = rest;
      nxt->tag = _tag_free;
      nxt->idn = _mem_idn;

      blk->nxt = nxt;
      blk->siz = s;
   }

   blk->tag = _tag_static; /* TODO */
   blk->idn = _mem_idn;
   fastmemset(blk->dat, 0, rs);

   mem_top->cur = blk->nxt;

   return blk->dat;
}

stkcall
void *Ralloc(register void *p, register size_t s) {
   register size_t os;
   register char  *nxt;

   if(!p) {
      return Malloc(s);
   }

   os = CheckUsedBlock(GetBlock(p), __func__)->siz - sizeof(struct mem_blk);

   nxt = Malloc(s);

   if(!nxt) {
      return nil;
   }

   fastmemmove(nxt, p, min(os, s));
   Dalloc(p);

   if(os < s) {
      fastmemset(&nxt[os], 0, s - os);
   }

   return nxt;
}

stkcall
void __sta *__GDCC__alloc(register void __sta *p, register size_t s) {
   if(!p) {
      return Malloc(s);
   } else if(s == 0) {
      Dalloc(p);
      return 0;
   } else {
      return Ralloc(p, s);
   }
}

stkcall
void __GDCC__alloc_dump(void) {
   struct mem_blk *blk = mem_top->cur;

   do {
      ACS_BeginPrint();
      PrintChars("blk:", 4);
      ACS_PrintHex((uintptr_t)blk);
      PrintChars(" prv:", 5);
      ACS_PrintHex((uintptr_t)blk->prv);
      PrintChars(" nxt:", 5);
      ACS_PrintHex((uintptr_t)blk->nxt);
      PrintChars(" siz:", 5);
      ACS_PrintHex(blk->siz);
      PrintChars(" tag:", 5);
      ACS_PrintInt(blk->tag);
      PrintChars(" idn:", 5);
      ACS_PrintHex(blk->idn);
      ACS_EndLog();

      blk = blk->nxt;
   } while(blk != mem_top->cur);
}

stkcall
void *__GDCC__Plsa(u32 s) {
   [[return]] __asm(
      "Jcnd_Tru(Sta(Lit(:pls_cur)) Lit(:\"ok\"))"
      "Move    (Sta(Lit(:pls_cur)) Lit(:pls_stk))"
   ":\"ok\""
      "Move (Stk() Sta(Lit(:pls_cur)))"
      "Add:U(Sta(Lit(:pls_cur)) Sta(Lit(:pls_cur)) LocReg(Lit(:s)))"
      #ifndef NDEBUG
      "CmpGT:U (Stk() Sta(Lit(:pls_cur))"
      "         Lit(+ :pls_stk " XStringify(_pls_siz) "_32.0))"
      "Jcnd_Nil(Stk() Lit(:\"overflow\"))"
      #endif
      "Retn (Stk())"
   ":\"overflow\""
   );

   #ifndef NDEBUG
   Str(err, s"Plsa ERROR: stack overflow ");
   ACS_BeginPrint();
   ACS_PrintString(err);
   ACS_PrintHex((uintptr_t)pls_cur);
   ACS_PrintChar(' ');
   ACS_PrintInt(s);
   ACS_EndLog();
   return nil;
   #endif
}

stkcall
void __GDCC__Plsf(void *p) {
   #ifndef NDEBUG
   if(p >= (void *)pls_cur) {
      Str(err, s"Plsf ERROR: incorrect stack pointer ");
      ACS_BeginPrint();
      ACS_PrintString(err);
      ACS_PrintHex((uintptr_t)p);
      ACS_PrintChar(' ');
      ACS_PrintHex((uintptr_t)pls_cur);
      ACS_EndLog();
      return;
   }
   #endif

   pls_cur = p;
}

/* EOF */
