/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Info page handling and loading.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "p_bip.h"
#include "m_list.h"
#include "w_world.h"
#include "m_file.h"
#include "m_tokbuf.h"

/* Types ------------------------------------------------------------------- */

struct page_init
{
   i32           pclass;
   bip_name_t    name;
   bip_unlocks_t unlocks;
   bool          isfree;
};

/* Static Functions -------------------------------------------------------- */

static str DecryptBody(char __str_ars const *s)
{
   ACS_BeginPrint();
   for(; *s; s++) ACS_PrintChar(!IsPrint(*s) ? *s : *s ^ 7);
   return ACS_EndStrParam();
}

static void UnlockPage(struct player *p, struct page *page)
{
   struct bip *bip = &p->bip;

   if(!page->unlocked)
   {
      bip->pageavail++;
      bip->categoryavail[page->category]++;
      page->unlocked = true;
   }

   for(i32 i = 0; i < countof(page->unlocks) && page->unlocks[i][0]; i++)
      P_BIP_Unlock(p, page->unlocks[i]);
}

optargs(1)
static void AddToBIP(struct player *p, i32 categ, struct page_init const *pinit, bool isfree)
{
   struct bip *bip = &p->bip;

   str image  = LanguageNull(LANG "INFO_IMAGE_%s", pinit->name);
   i32 height = strtoi_str(Language(LANG "INFO_SSIZE_%s", pinit->name), nil, 0);

   struct page *page = Salloc(struct page);

   memmove(page->name, pinit->name, sizeof page->name);
   page->category = categ;
   page->unlocked = false;
   page->image    = image;
   page->height   = height;
   memmove(page->unlocks, pinit->unlocks, sizeof page->unlocks);

   ListCtor(&page->link, page);
   page->link.link(&bip->infogr[categ]);

   /* we have to pass along the player's class so discriminators don't fuck up */
   if(isfree) UnlockPage(p, page);
}

stkcall
static i32 CatFromStr(str name)
{
   if(name == st_bipc_EXTRA) return BIPC_EXTRA;
   #define LITH_X(n, _) if(name == st_bipc_##n) return BIPC_##n;
   #include "p_bip.h"
   return BIPC_NONE;
}

stkcall
static i32 PClFromStr(str name)
{
   #define LITH_X(n, pc) if(name == st_##n || name == st_##pc) return pc;
   #include "p_player.h"
   return 0;
}

script
static struct token *AddPage(struct player *p, struct tokbuf *tb, struct token *tok, i32 categ, bool catfree)
{
   struct bip *bip = &p->bip;
   struct page_init page = {};

   do
      page.pclass |= PClFromStr(TokStr(tok));
   while(tb->drop(tok_or) && (tok = tb->get()));

   tok = tb->get();
   memmove(page.name, tok->textV, tok->textC);
   page.isfree = tb->drop(tok_mul);

   if(tb->drop(tok_rarrow))
      for(i32 i = 0; i < countof(page.unlocks) && !tb->drop(tok_lnend); i++)
   {
      tok = tb->get();
      memmove(page.unlocks[i], tok->textV, tok->textC);
   }

   if(categ != BIPC_NONE && p->pclass & page.pclass)
      AddToBIP(p, categ, &page, page.isfree || catfree);

   return tok;
}

static i32 LoadBIPInfo(struct player *p, str fname)
{
   struct tokbuf tb = {
      .bbeg = 4, .bend = 10,
      .fp = W_Open(fname, c"r"),
      .tokProcess = TBufProcL
   };
   if(!tb.fp) return 0;

   TBufCtor(&tb);

   bool catfree = false;
   i32 categ = BIPC_NONE;
   i32 total = 0;

   for(struct token *tok; (tok = tb.get())->type != tok_eof;) switch(tok->type)
   {
   case tok_lnend:
      continue;
   case tok_at:
      /* @ Include */
      tok = tb.get();
      total += LoadBIPInfo(p, TokStr(tok));
      break;
   case tok_xor:
      /* ^ Category [*] */
      tok = tb.get();
      categ = CatFromStr(TokStr(tok));
      catfree = tb.drop(tok_mul);
      break;
   case tok_identi:
      /* Classes... Name [*] [-> Unlocks...] */
      tok = AddPage(p, &tb, tok, categ, catfree);
      total++;
      break;
   }

   TBufDtor(&tb);
   fclose(tb.fp);

   return total;
}

static struct page *FindPage(struct bip *bip, char const *name)
{
   if(!name) return nil;
   ForCategoryAndPage() if(faststrcmp(page->name, name) == 0) return page;
   return nil;
}

/* Extern Functions -------------------------------------------------------- */

script
void P_BIP_PInit(struct player *p)
{
   struct bip *bip = &p->bip;

   ForCategory()
      ListDtor(&bip->infogr[categ], true);

   i32 total = LoadBIPInfo(p, sp_lfiles_BIPInfo);
   if(dbglevel) p->logH(1, "There are %i info pages!", total);

   ForCategory()
      bip->pagemax += bip->categorymax[categ] = bip->infogr[categ].size();

   if(dbgflag & dbgf_bip) ForCategoryAndPage() UnlockPage(p, page);

   bip->init = true;
}

script
void P_BIP_GiveMail(struct player *p, str title, i32 flags)
{
   /* Note: Due to the way this code works, if you switch languages at runtime, */
   /* mail won't be updated. I provide a lore answer (excuse) for this: When */
   /* you switch languages, it switches the preferred translation service for */
   /* delivered mail, but it's done when the mail is delivered due to a bug in */
   /* the BIP software. All of the mail in the game is delivered to the player */
   /* in Sce, a south-eastern sector language, but the player themself can't */
   /* read this language and so it is translated for them into the actual */
   /* language that the person behind the screen reads. */

   p->setActivator();

   flags |= strtoi_str(Language(LANG "MAIL_FLAG_%S", title), nil, 0);
   flags |= strtoi_str(Language(LANG "MAIL_FLAG_%S%s", title, p->discrim), nil, 0);

   if(!(flags & MAILF_AllPlayers)) title = StrParam("%S%s", title, p->discrim);

   struct bip *bip = &p->bip;

   struct page *page = Salloc(struct page);

   str date = LanguageNull(LANG "MAIL_TIME_%S", title);
   str size = LanguageNull(LANG "MAIL_SIZE_%S", title);
   str send = LanguageNull(LANG "MAIL_SEND_%S", title);
   str name = LanguageNull(LANG "MAIL_NAME_%S", title);
   str body = Language    (LANG "MAIL_BODY_%S", title);

   if(!send) send = L(st_mail_internal);

   page->shname   = date ? date : l_strdup(CanonTime(ct_short));
   page->title    = name ? name : L(st_mail_notitle);
   page->body     = StrParam(LC(LANG "MAIL_TEMPLATE"), send, CanonTime(ct_full), body);
   page->category = BIPC_MAIL;
   page->unlocked = true;

   if(size) page->height = strtoi_str(size, nil, 0);

   ListCtor(&page->link, page);
   page->link.link(&bip->infogr[BIPC_MAIL]);

   bip->mailreceived++;

   if(!(flags & MAILF_NoPrint))
   {
      ACS_Delay(20);

      p->logB(1, LC(LANG "LOG_MailRecv"), send);

      if(ACS_Random(1, 10000) == 1)
      {
         bip->mailtrulyreceived++;
         ACS_LocalAmbientSound(ss_player_YOUVEGOTMAIL, 127);
      }
      else
         ACS_LocalAmbientSound(ss_player_cbi_mail, 127);
   }
}

struct page *P_BIP_Unlock(struct player *p, char const *name)
{
   struct bip  *bip  = &p->bip;
   struct page *page = FindPage(bip, name);

   if(!page)
   {
      bip_name_t tag = {}; strcpy(tag, name); strcat(tag, p->discrim);
      page = FindPage(bip, tag);
   }

   if(page && !page->unlocked) UnlockPage(p, page);

   if(!page) Dbg_Log(log_bip, "no page '%s' found", name);

   return page;
}

stkcall
void P_BIP_PQuit(struct player *p)
{
   struct bip *bip = &p->bip;
   ForCategory() ListDtor(&bip->infogr[categ], true);
   bip->init = false;
}

struct page_info PageInfo(struct page const *page)
{
   struct page_info pinf;

   pinf.shname = page->shname
      ? page->shname
      : Language(LANG "INFO_SHORT_%s", page->name);

   pinf.body = page->body
      ? page->body
      : Language(LANG "INFO_DESCR_%s", page->name);

   pinf.flname = page->title
      ? page->title
      : Language(LANG "INFO_TITLE_%s", page->name);

   if(page->category == BIPC_EXTRA)
      pinf.body = DecryptBody(pinf.body);

   if(pinf.body[0] == '#')
   {
      bool top = false;

      ACS_BeginPrint();
      ACS_BeginPrint();

      for(i32 i = 1, l = ACS_StrLen(pinf.body); i < l; i++)
      {
         if(!top && pinf.body[i] == '\n') {top = true; ACS_PrintString(L(ACS_EndStrParam()));}
         ACS_PrintChar(pinf.body[i]);
      }

      pinf.body = ACS_EndStrParam();
   }

   return pinf;
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr("Lith_BIPUnlock")
void Sc_UnlockPage(i32 pnum)
{
   with_player(&players[pnum])
   {
      bip_name_t tag; lstrcpy_str(tag, GetMembS(0, sm_InfoPage));
      P_BIP_Unlock(p, tag);
   }
}

/* EOF */
