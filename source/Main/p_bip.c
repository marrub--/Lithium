// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_bip.h"
#include "lith_list.h"
#include "lith_world.h"
#include "lith_file.h"
#include "lith_tokbuf.h"

StrEntON

// Types ---------------------------------------------------------------------|

struct page_init
{
   int           pclass;
   bip_name_t    name;
   bip_unlocks_t unlocks;
   bool          isfree;
};

// Static Functions ----------------------------------------------------------|

static __str DecryptBody(char __str_ars const *str)
{
   ACS_BeginPrint();
   for(; *str; str++)
      ACS_PrintChar(!IsPrint(*str) ? *str : *str ^ 7);
   return ACS_EndStrParam();
}

static void UnlockPage(struct bip *bip, struct page *page, int pclass)
{
   if(!page->unlocked)
   {
      bip->pageavail++;
      bip->categoryavail[page->category]++;
      page->unlocked = true;
   }

   for(int i = 0; i < countof(page->unlocks) && page->unlocks[i][0]; i++)
      bip->unlock(page->unlocks[i], pclass);
}

optargs(1)
static void AddToBIP(struct bip *bip, int categ, int pclass, struct page_init const *pinit, bool isfree)
{
   __str image = LanguageNull(LANG "INFO_IMAGE_%s", pinit->name);
   int height = strtoi_str(Language(cLANG "INFO_CSIZE_%s", pinit->name), null, 0);

   struct page *page = Salloc(struct page);

   memmove(page->name, pinit->name, sizeof page->name);
   page->category = categ;
   page->unlocked = false;
   page->image    = image;
   page->height   = height;
   memmove(page->unlocks, pinit->unlocks, sizeof page->unlocks);

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

script
static struct token *AddPage(struct bip *bip, struct tokbuf *tb, struct token *tok, int categ, int pclass, bool catfree)
{
   struct page_init page = {};

   do
      page.pclass |= PClFromStr(Lith_TokStr(tok));
   while(tb->drop(tok_or) && (tok = tb->get()));

   tok = tb->get();
   memmove(page.name, tok->textV, tok->textC);
   page.isfree = tb->drop(tok_mul);

   if(tb->drop(tok_rarrow))
      for(int i = 0; i < countof(page.unlocks) && !tb->drop(tok_lnend); i++)
   {
      tok = tb->get();
      memmove(page.unlocks[i], tok->textV, tok->textC);
   }

   if(categ != BIPC_NONE && pclass & page.pclass)
      AddToBIP(bip, categ, pclass, &page, page.isfree || catfree);

   return tok;
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
      tok = AddPage(bip, &tb, tok, categ, pclass, catfree);
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

   if(world.dbgBIP) ForCategoryAndPage() UnlockPage(bip, page, p->pclass);

   bip->init = true;
}

script
void Lith_DeliverMail(struct player *p, __str title, int flags)
{
   p->setActivator();

   flags |= strtoi_str(Language(cLANG "MAIL_FLAG_%S", title), null, 0);
   flags |= strtoi_str(Language(cLANG "MAIL_FLAG_%S%s", title, p->discrim), null, 0);

   if(!(flags & MAILF_AllPlayers)) title = StrParam(c"%S%s", title, p->discrim);

   struct bip *bip = &p->bip;

   struct page *page = Salloc(struct page);

   __str date = LanguageNull(LANG "MAIL_TIME_%S", title);
   __str size = LanguageNull(LANG "MAIL_SIZE_%S", title);
   __str send = LanguageNull(LANG "MAIL_SEND_%S", title);
   __str name = LanguageNull(LANG "MAIL_NAME_%S", title);
   __str body = Language   (cLANG "MAIL_BODY_%S", title);

   if(!send) send = L(LANG "MAIL_INTERNAL");

   lstrcpy_str(page->name, date ? date : l_strdup(world.canontimeshort));
   page->title    = name ? name : L(LANG "MAIL_NOTITLE");
   page->body     = StrParam(LC(cLANG "MAIL_TEMPLATE"), send, page->name, body);
   page->category = BIPC_MAIL;
   page->unlocked = true;

   if(size) page->height = strtoi_str(size, null, 0);

   page->link.construct(page);
   page->link.link(&bip->infogr[BIPC_MAIL]);

   bip->mailreceived++;

   if(!(flags & MAILF_NoPrint))
   {
      ACS_Delay(20);

      p->logB(1, L(LANG "LOG_MailRecv"), send);

      if(ACS_Random(1, 10000) == 1)
      {
         bip->mailtrulyreceived++;
         ACS_LocalAmbientSound("player/YOUVEGOTMAIL", 127);
      }
      else
         ACS_LocalAmbientSound("player/cbi/mail", 127);
   }
}

struct page *Lith_FindBIPPage(struct bip *bip, char const *name)
{
   if(!name) return null;
   ForCategoryAndPage() if(strcmp(page->name, name) == 0) return page;
   return null;
}

struct page *Lith_UnlockBIPPage(struct bip *bip, char const *name, int pclass)
{
   struct page *page = bip->find(name);

   if(!page && pclass) ifauto(char const *, discrim, Lith_PlayerDiscriminator(pclass))
   {
      bip_name_t tag = {}; strcpy(tag, name); strcat(tag, discrim);
      page = bip->find(tag);
   }

   if(page && !page->unlocked) UnlockPage(bip, page, pclass);

   if(!page) LogDebug(log_bip, "no page '%s' found", name);

   return page;
}

script ext("ACS")
void Lith_BIPUnlock(int pnum)
{
   withplayer(&players[pnum])
   {
      bip_name_t tag; lstrcpy_str(tag, GetMembS(0, "m_infopage"));
      p->bipUnlock(tag);
   }
}

script
void Lith_DeallocateBIP(struct bip *bip)
{
   ForCategory() bip->infogr[categ].free(true);
   bip->init = false;
}

struct page_info Lith_GetPageInfo(struct page const *page)
{
   struct page_info pinf;

   pinf.shname = page->category == BIPC_MAIL
      ? l_strdup(page->name)
      : Language(cLANG "INFO_SHORT_%s", page->name);

   pinf.body = page->body
      ? page->body
      : Language(cLANG "INFO_DESCR_%s", page->name);

   pinf.flname = page->title
      ? page->title
      : Language(cLANG "INFO_TITLE_%s", page->name);

   if(page->category == BIPC_EXTRA)
      pinf.body = DecryptBody(pinf.body);

   if(pinf.body[0] == '#')
   {
      bool top = false;

      ACS_BeginPrint();
      ACS_BeginPrint();

      for(int i = 1, l = ACS_StrLen(pinf.body); i < l; i++)
      {
         if(!top && pinf.body[i] == '\n') {top = true; ACS_PrintString(L(ACS_EndStrParam()));}
         ACS_PrintChar(pinf.body[i]);
      }

      pinf.body = ACS_EndStrParam();
   }

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
