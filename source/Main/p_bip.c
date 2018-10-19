// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_bip.h"
#include "lith_list.h"
#include "lith_world.h"
#include "lith_file.h"
#include "lith_tokbuf.h"

#include <ctype.h>

// Types ---------------------------------------------------------------------|

struct page_init
{
   int pclass;
   __str name;
   bip_unlocks_t unlocks;
   bool isfree;
};

// Static Functions ----------------------------------------------------------|

static __str DecryptBody(char __str_ars const *str)
{
   ACS_BeginPrint();
   for(; *str; str++)
      ACS_PrintChar(!isprint(*str) ? *str : *str ^ 7);
   return ACS_EndStrParam();
}

static void UnlockPage(struct bip *bip, struct page *page, int pclass)
{
   bip->pageavail++;
   bip->categoryavail[page->category]++;
   page->unlocked = true;

   for(int i = 0; i < countof(page->unlocks) && page->unlocks[i]; i++)
      bip->unlock(page->unlocks[i], pclass);
}

optargs(1)
static void AddToBIP(struct bip *bip, int categ, int pclass, struct page_init const *pinit, bool isfree)
{
   __str image = LanguageNull("LITH_INFO_IMAGE_%S", pinit->name);
   int height = strtoi_str(Language("LITH_INFO_CSIZE_%S", pinit->name), null, 0);

   struct page *page = Salloc(struct page);

   page->name     = pinit->name;
   page->category = categ;
   page->unlocked = false;
   page->image    = image;
   page->height   = height;
   memmove(page->unlocks, pinit->unlocks, sizeof(page->unlocks));

   page->link.construct(page);
   page->link.link(&bip->infogr[categ]);

   // we have to pass along the player's class so discriminators don't fuck up
   if(isfree) UnlockPage(bip, page, pclass);
}

stkcall
static int CatFromStr(__str name)
{
   if(name == "EXTRA") return BIPC_EXTRA;
#define LITH_X(n, _) if(name == #n) return BIPC_##n;
#include "lith_bip.h"
   return BIPC_NONE;
}

stkcall
static int PClFromStr(__str name)
{
#define LITH_X(n, pc) if(name == #n || name == #pc) return pc;
#include "lith_player.h"
   return 0;
}

static int LoadBIPInfo(__str fname, struct bip *bip, int pclass)
{
   struct tokbuf tb = {
      .bbeg = 4, .bend = 10,
      .fp = W_Open(fname, c"r"),
      .tokProcess = Lith_TBufProcL
   };
   if(!tb.fp) return 0;

   tb.ctor();

   bool catfree = false;
   int categ = BIPC_NONE;
   int total = 0;
   struct page_init page;

   for(struct token *tok; (tok = tb.get())->type != tok_eof;) switch(tok->type)
   {
   case tok_lnend:
      continue;
   case tok_at:
      // @ Include
      tok = tb.get();
      total += LoadBIPInfo(Lith_TokStr(tok), bip, pclass);
      break;
   case tok_xor:
      // ^ Category [*]
      tok = tb.get();
      categ = CatFromStr(Lith_TokStr(tok));
      catfree = tb.drop(tok_mul);
      break;
   case tok_identi:
      // Classes... Name [*] [-> Unlocks...]
      memset(&page, 0, sizeof page);

      do
         page.pclass |= PClFromStr(Lith_TokStr(tok));
      while(tb.drop(tok_or) && (tok = tb.get()));

      tok = tb.get();
      page.name = Lith_TokStr(tok);
      page.isfree = tb.drop(tok_mul);

      if(tb.drop(tok_rarrow))
         for(int i = 0; i < countof(page.unlocks) && !tb.drop(tok_lnend); i++)
      {
         tok = tb.get();
         page.unlocks[i] = Lith_TokStr(tok);
      }

      if(categ != BIPC_NONE && pclass & page.pclass)
         AddToBIP(bip, categ, pclass, &page, page.isfree || catfree);
      total++;
      break;
   }

   tb.dtor();
   fclose(tb.fp);

   return total;
}

// Extern Functions ----------------------------------------------------------|

script
void Lith_PlayerInitBIP(struct player *p)
{
   struct bip *bip = &p->bip;

   ForCategory()
      bip->infogr[categ].free(true);

   int total = LoadBIPInfo("lfiles/BIPInfo.txt", bip, p->pclass);
   if(world.dbgLevel) p->logH(1, "There are %i info pages!", total);

   ForCategory()
      bip->pagemax += bip->categorymax[categ] = bip->infogr[categ].size();

   if(world.dbgBIP)
   {
      bip->pageavail = bip->pagemax;

      ForCategory()        bip->categoryavail[categ] = bip->categorymax[categ];
      ForCategoryAndPage() page->unlocked = true;
   }

   bip->init = true;
}

script
void Lith_DeliverMail(struct player *p, __str title, int flags)
{
   p->setActivator();

   flags |= strtoi_str(Language("LITH_MAIL_FLAG_%S", title), null, 0);

   ifauto(__str, discrim, p->discrim) {
      flags |= strtoi_str(Language("LITH_MAIL_FLAG_%S%S", title, discrim), null, 0);
      if(!(flags & MAILF_AllPlayers))
         title = StrParam("%S%S", title, discrim);
   }

   struct bip *bip = &p->bip;

   struct page *page = Salloc(struct page);

   __str date = LanguageNull("LITH_MAIL_TIME_%S", title);
   __str size = LanguageNull("LITH_MAIL_SIZE_%S", title);
   __str send = LanguageNull("LITH_MAIL_SEND_%S", title);
   __str name = LanguageNull("LITH_MAIL_NAME_%S", title);
   __str body = Language    ("LITH_MAIL_BODY_%S", title);

   if(!send) send = L("LITH_MAIL_INTERNAL");

   page->name  = date ? date : world.canontimeshort;
   page->title = name ? name : L("LITH_MAIL_NOTITLE");
   page->body  = StrParam(L("LITH_MAIL_TEMPLATE"), send, page->name, body);
   page->category = BIPC_MAIL;
   page->unlocked = true;

   if(size) page->height = strtoi_str(size, null, 0);

   page->link.construct(page);
   page->link.link(&bip->infogr[BIPC_MAIL]);

   bip->mailreceived++;

   if(!(flags & MAILF_NoPrint))
   {
      ACS_Delay(20);

      p->logB(1, L("LITH_LOG_MailRecv"), send);

      if(ACS_Random(1, 10000) == 1)
      {
         bip->mailtrulyreceived++;
         ACS_LocalAmbientSound("player/YOUVEGOTMAIL", 127);
      }
      else
         ACS_LocalAmbientSound("player/cbi/mail", 127);
   }
}

struct page *Lith_FindBIPPage(struct bip *bip, __str name)
{
   if(!name)
      return null;

   ForCategoryAndPage()
      if(page->name == name)
         return page;

   return null;
}

struct page *Lith_UnlockBIPPage(struct bip *bip, __str name, int pclass)
{
   struct page *page = bip->find(name);

   if(!page && pclass) ifauto(__str, discrim, Lith_PlayerDiscriminator(pclass))
   {
      page = bip->find(StrParam("%S%S", name, discrim));
   }

   if(page && !page->unlocked)
      UnlockPage(bip, page, pclass);

   return page;
}

script ext("ACS")
void Lith_BIPUnlock(int pnum)
{
   withplayer(&players[pnum]) p->bipUnlock(getmems(0, "m_infopage"));
}

script
void Lith_DeallocateBIP(struct bip *bip)
{
   ForCategory()
      bip->infogr[categ].free(true);
   bip->init = false;
}

struct page_info Lith_GetPageInfo(struct page const *page)
{
   struct page_info pinf;

   pinf.shname = page->category == BIPC_MAIL
      ? page->name
      : Language("LITH_INFO_SHORT_%S", page->name);

   pinf.body = page->body
      ? page->body
      : Language("LITH_INFO_DESCR_%S", page->name);

   pinf.flname = page->title
      ? page->title
      : Language("LITH_INFO_TITLE_%S", page->name);

   if(page->category == BIPC_EXTRA)
      pinf.body = DecryptBody(pinf.body);

   return pinf;
}

script ext("ACS")
void Lith_GiveMail(int num)
{
   static __str const names[] = {
      "Intro",
      "Cluster1",
      "Cluster2",
      "Cluster3",
      "Phantom",
      "JamesDefeated",
      "MakarovDefeated",
      "IsaacDefeated"
   };

   num %= countof(names);

   withplayer(LocalPlayer)
      p->deliverMail(names[num]);
}

// EOF