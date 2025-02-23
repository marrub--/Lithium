// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Memory allocations.                                                      │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include <GDCC.h>

#define _mem_idn 0xC0FFEE69
#define _mem_beg 0xADEADBED
#define _mem_frg 0x80
#define _mem_siz 0x10000000

#define _pls_siz 0x4000000

#define _dpl_siz 0x40
#define _dpl_bsz 0x2000

struct mem_blk {
   mem_tag_t idn;
   struct mem_blk *prv, *nxt;
   mem_size_t siz;
   mem_tag_t tag;
   char dat[];
};

struct mem_top {
   struct mem_blk beg, *cur;
   char dat[];
};

struct dpl_blk {
   struct dpl_blk *prv, *nxt;
   char dat[_dpl_bsz];
};

noinit static char            mem_dat[_mem_siz];
noinit static struct mem_top *mem_top;
noinit static char            pls_stk[_pls_siz], *pls_cur;
noinit static struct dpl_blk  dpl_dat[_dpl_siz], *dpl_ina, *dpl_act;

stkoff static cstr TagName(mem_tag_t tag) {
   switch(tag) {
   case _tag_free: return "free";
   case _tag_head: return "listhead";
   case _tag_libc: return "libc";
   case _tag_dlgv: return "dialogue vm";
   case _tag_dlgc: return "dialogue compiler";
   case _tag_file: return "file";
   case _tag_item: return "item";
   case _tag_logs: return "log";
   case _tag_plyr: return "player";
   }
   return "unknown";
}

#ifndef NDEBUG
stkoff static struct mem_blk *CheckUsedBlock(struct mem_blk *blk, cstr func) {
   if((void *)blk < (void *)mem_dat ||
      (void *)blk > (void *)(mem_dat + sizeof mem_dat)) {
      PrintErr(_l("out of bounds block "), ACS_PrintHex((intptr_t)blk));
      return nil;
   }
   if(blk->idn != _mem_idn) {
      PrintErr(_l("invalid identifier for "), ACS_PrintHex((intptr_t)blk));
      return nil;
   }
   if(blk->tag == _tag_free) {
      PrintErr(_l("already freed "), ACS_PrintHex((intptr_t)blk));
      return nil;
   }
   return blk;
}
#else
#define CheckUsedBlock(blk, func) blk
#endif

#define GetBlock(p) \
   ((struct mem_blk *)((char *)p - sizeof(struct mem_blk)))

stkoff struct mem_blk *MergeAdjacent(struct mem_blk *lhs, struct mem_blk *rhs) {
   rhs->siz += lhs->siz;
   (rhs->nxt = lhs->nxt)->prv = rhs;
   return lhs == mem_top->cur ? mem_top->cur = rhs : rhs;
}

stkoff void Dalloc(register void *p) {
   register struct mem_blk *blk;
   __asm(
      "Jcnd_Nil(LocReg(Lit(:p)) Lit(:\"done\"))"
      #ifndef NDEBUG
      "Jcnd_Nil(Sta(Lit(:mem_top)) Lit(:error:))"
      #endif
   );
   blk = CheckUsedBlock(GetBlock(p), _f);
   blk->tag = _tag_free;
   if(blk->prv->tag == _tag_free) blk = MergeAdjacent(blk, blk->prv);
   if(blk->nxt->tag == _tag_free)       MergeAdjacent(blk->nxt, blk);
   [[return]] __asm(":\"done\" Rjnk()");
error:
   PrintErr(_l("memory not initialized but freeing pointer"));
}

stkoff void *Malloc(register mem_size_t rs, register mem_tag_t tag) {
   register struct mem_blk *cur, *blk, *end;
   register mem_size_t s = rs + sizeof(struct mem_blk);
   if(!mem_top) {
      mem_top = (void *)mem_dat;
      mem_top->beg.prv = mem_top->beg.nxt = mem_top->cur =
         (void *)mem_top->dat;
      mem_top->beg.tag = _tag_head;
      mem_top->beg.idn = _mem_beg;
      mem_top->cur->prv = mem_top->cur->nxt = &mem_top->beg;
      mem_top->cur->tag = _tag_free;
      mem_top->cur->idn = _mem_idn;
      mem_top->cur->siz = sizeof(mem_dat) - sizeof(struct mem_top);
   }
   end = (cur = blk = mem_top->cur)->prv;
   do {
      if(cur == end) {
         /* well, no more memory to check...
          * don't worry, this won't ever really happen, right?
          */
         PrintErr(_l("out of memory - couldn't allocate "), _p(s));
         return nil;
      } else if(cur->tag != _tag_free) {
         blk = cur = cur->nxt;
      } else {
         cur = cur->nxt;
      }
   } while(blk->tag != _tag_free || blk->siz < s);
   register mem_size_t rest = blk->siz - s;
   if(rest > _mem_frg) {
      register struct mem_blk *nxt = (void *)&blk->dat[rs];
      nxt->siz = rest;
      nxt->tag = _tag_free;
      nxt->idn = _mem_idn;
      (nxt->nxt = (nxt->prv = blk)->nxt)->prv = nxt;
      blk->nxt = nxt;
      blk->siz = s;
   }
   blk->tag = tag;
   blk->idn = _mem_idn;
   fastmemset(blk->dat, 0, rs);
   mem_top->cur = blk->nxt;
   return blk->dat;
}

stkoff void *Ralloc(register void *p, register mem_size_t s, register mem_tag_t tag) {
   register mem_size_t os;
   register char *nxt;
   if(!p) {
      return Malloc(s, tag);
   }
   os = CheckUsedBlock(GetBlock(p), _f)->siz - sizeof(struct mem_blk);
   nxt = Malloc(s, tag);
   if(!nxt) {
      return nil;
   }
   fastmemmove(nxt, p, mini(os, s));
   Dalloc(p);
   if(os < s) {
      fastmemset(&nxt[os], 0, s - os);
   }
   return nxt;
}

stkoff void Xalloc(mem_tag_t tag) {
   struct mem_blk *cur, *end;
   cur = (end = mem_top->cur)->nxt;
   while(cur != end) {
      struct mem_blk *nxt = cur->nxt;
      if(cur->tag == tag) {
         Dalloc(cur->dat);
      }
      cur = nxt;
   }
}

stkoff void __sta *__GDCC__alloc(register void __sta *p, register mem_size_t s) {
   if(!p) {
      return Malloc(s, _tag_libc);
   } else if(s == 0) {
      Dalloc(p);
      return 0;
   } else {
      return Ralloc(p, s, _tag_libc);
   }
}

stkoff static
void PrintDplBlks(struct dpl_blk *cur) {
   mem_size_t i = 0;
   do {
      PrintStrL("cur:");
      ACS_PrintHex((intptr_t)cur);
      PrintStrL(" prv:");
      ACS_PrintHex((intptr_t)cur->prv);
      PrintStrL(" nxt:");
      ACS_PrintHex((intptr_t)cur->nxt);
      ACS_PrintChar('\n');
      i++;
   } while(cur = cur->nxt);
   ACS_PrintChar('(');
   ACS_PrintInt(i);
   PrintStrL(" in list)");
}

stkoff void __GDCC__alloc_dump(void) {
   static mem_size_t tagsizes[_tag_max];
   struct mem_blk *blk = mem_top->cur;
   for(mem_tag_t i = 0; i < _tag_max; i++) {
      tagsizes[i] = 0;
   }
   do {
      ACS_BeginPrint();
      PrintStrL("blk:");
      ACS_PrintHex((intptr_t)blk);
      PrintStrL(" prv:");
      ACS_PrintHex((intptr_t)blk->prv);
      PrintStrL(" nxt:");
      ACS_PrintHex((intptr_t)blk->nxt);
      PrintStrL(" siz:");
      ACS_PrintHex(blk->siz);
      PrintStrL(" tag:");
      PrintStr(TagName(blk->tag));
      ACS_PrintChar('(');
      ACS_PrintInt(blk->tag);
      ACS_PrintChar(')');
      PrintStrL(" idn:");
      ACS_PrintHex(blk->idn);
      EndLogEx(_pri_bold|_pri_nonotify);

      tagsizes[blk->tag] += blk->siz;
      blk = blk->nxt;
   } while(blk != mem_top->cur);
   ACS_BeginPrint();
   PrintStrL("total sizes");
   for(mem_tag_t i = 0; i < _tag_max; i++) {
      ACS_PrintChar('\n');
      PrintStr(TagName(i));
      PrintStrL(":\t\t");
      ACS_PrintInt(tagsizes[i]);
   }
   EndLogEx(_pri_bold|_pri_nonotify);
   ACS_BeginPrint();
   if(dpl_act) {
      PrintStrL("active freelist\n");
      PrintDplBlks(dpl_act);
   }
   EndLogEx(_pri_bold|_pri_nonotify);
   ACS_BeginPrint();
   if(dpl_ina) {
      PrintStrL("inactive freelist\n");
      PrintDplBlks(dpl_ina);
   }
   EndLogEx(_pri_bold|_pri_nonotify);
   ACS_BeginPrint();
   PrintStrL("mem_dat=");
   ACS_PrintHex((intptr_t)mem_dat);
   PrintStrL("\nmem_top=");
   ACS_PrintHex((intptr_t)mem_top);
   PrintStrL("\npls_stk=");
   ACS_PrintHex((intptr_t)pls_stk);
   PrintStrL("\npls_cur=");
   ACS_PrintHex((intptr_t)pls_cur);
   PrintStrL("\ndpl_dat=");
   ACS_PrintHex((intptr_t)dpl_dat);
   PrintStrL("\ndpl_ina=");
   ACS_PrintHex((intptr_t)dpl_ina);
   PrintStrL("\ndpl_act=");
   ACS_PrintHex((intptr_t)dpl_act);
   EndLogEx(_pri_bold|_pri_nonotify);
}

#define GetDplBlk(p) \
   ((struct dpl_blk *)((char *)(p) - (sizeof(struct dpl_blk) - _dpl_bsz)))

stkoff void *__GDCC__Plsa(mem_size_t s) {
   [[return]] __asm(
      "BAnd    (Stk() LocReg(Lit(:s)) Lit(0x80000000_s31.0))"
      "Jcnd_Tru(Stk() Lit(:dyn:))"
      "Jcnd_Tru(Sta(Lit(:pls_cur)) Lit(:\"ok\"))"
      "Move    (Sta(Lit(:pls_cur)) Lit(:pls_stk))"
   ":\"ok\""
      "Move (Stk() Sta(Lit(:pls_cur)))"
      "Add:I(Sta(Lit(:pls_cur)) Sta(Lit(:pls_cur)) LocReg(Lit(:s)))"
      #ifndef NDEBUG
      "CmpGT:I (Stk() Sta(Lit(:pls_cur))"
      "         Lit(+ :pls_stk " XStringify(_pls_siz) "_s31.0))"
      "Jcnd_Nil(Stk() Lit(:overflow:))"
      #endif
      "Retn(Stk())"
   );
dyn:
   struct dpl_blk *p;
   if(!dpl_ina) {
      dpl_act = nil;
      dpl_ina = dpl_dat;
      p = nil;
      for(mem_size_t i = 0; i < _dpl_siz; i++) {
         dpl_dat[i].nxt = (i != _dpl_siz - 1 ? &dpl_dat[i + 1] : nil);
         dpl_dat[i].prv = p;
         p = &dpl_dat[i];
      }
   }
   Dbg_Log(log_dpl, _l("Plsa "), ACS_PrintHex((intptr_t)dpl_ina));
   dpl_ina = (p = dpl_ina)->nxt;
   p->nxt = dpl_act;
   return (dpl_act = p)->dat;
overflow:
   PrintErr(_l("stack overflow "), ACS_PrintHex((intptr_t)pls_stk),
            _c(' '),               ACS_PrintHex((intptr_t)pls_cur),
            _c(' '),               _p(s));
   return nil;
}

stkoff void __GDCC__Plsf(void *p) {
   if(p >= (void *)dpl_dat && p < (void *)&dpl_dat[_dpl_siz]) {
      struct dpl_blk *blk = GetDplBlk(p);
      Dbg_Log(log_dpl, _l("Plsf "), ACS_PrintHex((intptr_t)blk));
      if(blk->prv) blk->prv->nxt = blk->nxt;
      else         dpl_act       = blk->nxt;
      blk->nxt = dpl_ina;
      dpl_ina  = blk;
      return;
   }
   [[return]] __asm(
      #ifndef NDEBUG
      "CmpGE:I (Stk() LocReg(Lit(:p)) Sta(Lit(:pls_cur)))"
      "Jcnd_Tru(Stk() Lit(:invalid:))"
      #endif
      "Move(Sta(Lit(:pls_cur)) LocReg(Lit(:p)))"
      "Rjnk()"
   );
invalid:
   PrintErr(_l("incorrect stack pointer "), ACS_PrintHex((intptr_t)p),
            _c(' '),                        ACS_PrintHex((intptr_t)pls_cur));
}

/* EOF */
