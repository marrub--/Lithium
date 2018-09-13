// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_bip.h"
#include "lith_list.h"
#include "lith_world.h"
#include "lith_file.h"
#include "lith_tokbuf.h"

#include <ctype.h>

#define ForCategory() for(int categ = BIPC_NONE + 1; categ < BIPC_MAX; categ++)
#define ForPage() forlist(bippage_t *page, bip->infogr[categ])
#define ForCategoryAndPage() ForCategory() ForPage()

// Types ---------------------------------------------------------------------|

struct page_info
{
   __str shname;
   __str flname;
   __str body;
};

struct page_init
{
   int pclass;
   __str name;
   bip_unlocks_t unlocks;
   bool isfree;
};

// Static Functions ----------------------------------------------------------|

//
// DecryptBody
//
static __str DecryptBody(char __str_ars const *str)
{
   ACS_BeginPrint();
   for(; *str; str++)
      ACS_PrintChar(!isprint(*str) ? *str : *str ^ 7);
   return ACS_EndStrParam();
}

//
// GetPageInfo
//
static struct page_info GetPageInfo(bippage_t const *page)
{
   struct page_info pinf;

   pinf.shname = page->category == BIPC_MAIL
      ? page->name
      : Language("LITH_TXT_INFO_SHORT_%S", page->name);

   pinf.body = page->body
      ? page->body
      : Language("LITH_TXT_INFO_DESCR_%S", page->name);

   pinf.flname = page->title
      ? page->title
      : Language("LITH_TXT_INFO_TITLE_%S", page->name);

   if(page->category == BIPC_EXTRA)
      pinf.body = DecryptBody(pinf.body);

   return pinf;
}

//
// SetCurPage
//
static void SetCurPage(gui_state_t *g, bip_t *bip, bippage_t *page, __str body)
{
   bip->curpage = page;

   Lith_GUI_TypeOn(g, &CBIState(g)->biptypeon, body);
   Lith_GUI_ScrollReset(g, &CBIState(g)->bipinfoscr);
}

//
// UnlockPage
//
static void UnlockPage(bip_t *bip, bippage_t *page, int pclass)
{
   bip->pageavail++;
   bip->categoryavail[page->category]++;
   page->unlocked = true;

   for(int i = 0; i < countof(page->unlocks) && page->unlocks[i]; i++)
      bip->unlock(page->unlocks[i], pclass);
}

//
// AddToBIP
//
optargs(1)
static void AddToBIP(bip_t *bip, int categ, int pclass, struct page_init const *pinit, bool isfree)
{
   __str image = LanguageNull("LITH_TXT_INFO_IMAGE_%S", pinit->name);
   int height = strtoi_str(Language("LITH_TXT_INFO_CSIZE_%S", pinit->name), null, 0);

   bippage_t *page = Salloc(bippage_t);

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

//
// CatFromStr
//
stkcall
static int CatFromStr(__str name)
{
   if(name == "EXTRA") return BIPC_EXTRA;
#define LITH_X(n, _) if(name == #n) return BIPC_##n;
#include "lith_bip.h"
   return BIPC_NONE;
}

//
// PClFromStr
//
stkcall
static int PClFromStr(__str name)
{
#define LITH_X(n, pc) if(name == #n || name == #pc) return pc;
#include "lith_player.h"
   return 0;
}

//
// LoadBIPInfo
//
static int LoadBIPInfo(__str fname, bip_t *bip, int pclass)
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

//
// Lith_PlayerInitBIP
//
script
void Lith_PlayerInitBIP(struct player *p)
{
   bip_t *bip = &p->bip;

   ForCategory()
      bip->infogr[categ].free(true);

   int total = LoadBIPInfo("lfiles/BIPInfo.txt", bip, p->pclass);
   if(world.dbgLevel) p->logH("> There are %i info pages!", total);

   ForCategory()
      bip->pagemax += bip->categorymax[categ] = bip->infogr[categ].size;

   if(world.dbgBIP)
   {
      bip->pageavail = bip->pagemax;

      ForCategory()        bip->categoryavail[categ] = bip->categorymax[categ];
      ForCategoryAndPage() page->unlocked = true;
   }

   bip->init = true;
}

//
// Lith_DeliverMail
//
script
void Lith_DeliverMail(struct player *p, __str title, int flags)
{
   p->setActivator();

   flags |= strtoi_str(Language("LITH_TXT_MAIL_FLAG_%S", title), null, 0);

   ifauto(__str, discrim, p->discrim) {
      flags |= strtoi_str(Language("LITH_TXT_MAIL_FLAG_%S%S", title, discrim), null, 0);
      if(!(flags & MAILF_AllPlayers))
         title = StrParam("%S%S", title, discrim);
   }

   bip_t *bip = &p->bip;

   bippage_t *page = Salloc(bippage_t);

   __str date = LanguageNull("LITH_TXT_MAIL_TIME_%S", title);
   __str size = LanguageNull("LITH_TXT_MAIL_SIZE_%S", title);
   __str send = LanguageNull("LITH_TXT_MAIL_SEND_%S", title);
   __str name = LanguageNull("LITH_TXT_MAIL_NAME_%S", title);
   __str body = Language    ("LITH_TXT_MAIL_BODY_%S", title);

   if(!send) send = "<internal>";

   page->name  = date ? date : world.canontimeshort;
   page->title = name ? name : "<title omitted>";
   page->body  = StrParam(Language("LITH_TXT_MAIL_TEMPLATE"), send, page->name, body);
   page->category = BIPC_MAIL;
   page->unlocked = true;

   if(size) page->height = strtoi_str(size, null, 0);

   page->link.construct(page);
   page->link.link(&bip->infogr[BIPC_MAIL]);

   bip->mailreceived++;

   if(!(flags & MAILF_NoPrint))
   {
      ACS_Delay(20);

      p->log("> Mail received from <\Cj%S\C->.", send);

      if(ACS_Random(1, 10000) == 1)
      {
         bip->mailtrulyreceived++;
         ACS_LocalAmbientSound("player/YOUVEGOTMAIL", 127);
      }
      else
         ACS_LocalAmbientSound("player/cbi/mail", 127);
   }
}

//
// Lith_FindBIPPage
//
bippage_t *Lith_FindBIPPage(bip_t *bip, __str name)
{
   if(!name)
      return null;

   ForCategoryAndPage()
      if(page->name == name)
         return page;

   return null;
}

//
// Lith_UnlockBIPPage
//
bippage_t *Lith_UnlockBIPPage(bip_t *bip, __str name, int pclass)
{
   bippage_t *page = bip->find(name);

   if(!page && pclass)
      ifauto(__str, discrim, Lith_PlayerDiscriminator(pclass))
   {
      page = bip->find(StrParam("%S%S", name, discrim));
   }

   if(page && !page->unlocked)
      UnlockPage(bip, page, pclass);

   return page;
}

//
// Lith_DeallocateBIP
//
script
void Lith_DeallocateBIP(bip_t *bip)
{
   ForCategory()
      bip->infogr[categ].free(true);
   bip->init = false;
}

//
// CheckMatch
//
script
static bool CheckMatch(struct page_info *pinf, __str query)
{
   return strcasestr_str(pinf->shname, query) ||
          strcasestr_str(pinf->flname, query) ||
          strcasestr_str(pinf->body,   query);
}

//
// Lith_CBITab_BIP
//
void Lith_CBITab_BIP(gui_state_t *g, struct player *p)
{
   bip_t *bip = &p->bip;
   int avail, max;

   if(bip->curcategory == BIPC_MAIN)
   {
      int n = 0;

      PrintTextStr("INFO CATEGORIES");
      PrintText("CBIFONT", CR_PURPLE, 40,1, 70,1);

      bip->lastcategory = BIPC_MAIN;

      static __str const lines[] = {
         "Text search all categories.",
         "Weapons and weapon upgrades.",
         "Enemies and bosses.",
         "Armors and other loot.",
         "Your attributes, abilities and history.",
         "Body upgrades.",
         "Places of interest around the galaxy.",
         "Important companies, historic and current.",
         "Received mail."
      };

      for(int i = 0; i < countof(lines); i++)
      {
         PrintTextStr(lines[i]);
         PrintTextA("CBIFONT", CR_WHITE, 105,1, 85+n + i*10,1, 0.7);
      }

      if(Lith_GUI_Button(g, "Search", 45, 85 + n, Pre(btnbipmain)))
         bip->curcategory = BIPC_SEARCH;
      n += 10;
#define LITH_X(name, capt) \
      if(Lith_GUI_Button_Id(g, BIPC_##name, capt, 45, 85 + n, Pre(btnbipmain))) \
      { \
         bip->curcategory = BIPC_##name; \
         bip->curpage     = null; \
      } \
      n += 10;
#include "lith_bip.h"

      avail = bip->pageavail;
      max   = bip->pagemax;
   }
   else if(bip->curcategory == BIPC_SEARCH)
   {
      gui_txtbox_state_t *st = Lith_GUI_TextBox(g, &CBIState(g)->bipsearch, 23, 65, p->num, p->txtbuf);
      p->clearTextBuf();

      bip->lastcategory = BIPC_MAIN;

      Lith_GUI_TextBox_OnTextEntered(st)
      {
         // That's a lot of numbers...
         u64 const extranames[] = {
            0x5F38B6C56F0A6D84L,
            0x90215131A36573D7L,
            0xC54EC0A7C6836A5BL,
            0xB315B81438717BA6L,
            0x9FD558A2C8C8D163L,
         };

         __str query = StrParam("%.*S", txt_len, txt_buf);
         u64 crc = Lith_CRC64_str(txt_buf, txt_len);

         bip->resnum = bip->rescur = 0;

         for(int i = 0; i < countof(extranames); i++)
         {
            if(crc == extranames[i])
            {
               list_t *link = bip->infogr[BIPC_EXTRA].next;
               for(int j = 0; j < i; j++)
                  link = link->next;
               bip->result[bip->resnum++] = link->object;
            }
         }

         ForCategoryAndPage()
         {
            if(bip->resnum >= countof(bip->result))
               break;

            if(!page->unlocked || page->category == BIPC_EXTRA)
               continue;

            struct page_info pinf = GetPageInfo(page);

            if(CheckMatch(&pinf, query))
               bip->result[bip->resnum++] = page;
         }

         if(bip->resnum == 0)
            ACS_LocalAmbientSound("player/cbi/findfail", 127);
      }

      if(bip->resnum)
      {
         for(int i = 0; i < bip->rescur; i++)
         {
            bippage_t *page = bip->result[i];
            struct page_info pinf = GetPageInfo(page);

            if(Lith_GUI_Button_Id(g, i, pinf.flname, 70, 95 + (i * 10), Pre(btnbipmain)))
            {
               bip->lastcategory = bip->curcategory;
               bip->curcategory = page->category;
               SetCurPage(g, bip, page, pinf.body);
            }
         }

         if((ACS_Timer() % ACS_Random(10, 20)) == 0 &&
            bip->rescur != bip->resnum)
         {
            if(++bip->rescur == bip->resnum)
               ACS_LocalAmbientSound("player/cbi/finddone", 127);
            else
               ACS_LocalAmbientSound("player/cbi/find", 127);
         }
      }
      else
      {
         PrintTextStr("No results");
         PrintText("CBIFONT", CR_DARKGREY, 70,0, 95,0);
      }
   }
   else
   {
      list_t *list = &bip->infogr[bip->curcategory];
      size_t n = list->size;
      size_t i = 0;

      Lith_GUI_ScrollBegin(g, &CBIState(g)->bipscr, 15, 50, guipre.btnlist.w, 170, guipre.btnlist.h * n);

      if(bip->curcategory != BIPC_EXTRA)
         forlistIt(bippage_t *page, *list, i++)
      {
         int y = guipre.btnlist.h * i;

         if(Lith_GUI_ScrollOcclude(g, &CBIState(g)->bipscr, y, guipre.btnlist.h))
            continue;

         struct page_info pinf = GetPageInfo(page);
         __str name = StrParam("%S%S", bip->curpage == page ? "\Ci" : "", pinf.shname);

         if(Lith_GUI_Button_Id(g, i, name, 0, y, !page->unlocked || bip->curpage == page, Pre(btnlist)))
            SetCurPage(g, bip, page, pinf.body);
      }

      Lith_GUI_ScrollEnd(g, &CBIState(g)->bipscr);

      if(bip->curpage)
      {
         bippage_t *page = bip->curpage;
         struct page_info pinf = GetPageInfo(page);

         gui_typeon_state_t const *typeon = Lith_GUI_TypeOnUpdate(g, &CBIState(g)->biptypeon);

         int oy = 0;

         if(page->height)
         {
            Lith_GUI_ScrollBegin(g, &CBIState(g)->bipinfoscr, 100, 40, 200, 180, page->height, 184);
            oy = g->oy - 40;
         }
         else
            SetClipW(111, 40, 200, 180, 184);

         if(page->image)
            PrintSpriteA(page->image, 296,2, 180,2, 0.4);

         PrintSpriteA("lgfx/UI/Background.png", 0,1, 0,1, 0.5);

         PrintTextStr(pinf.flname);
         PrintText("CBIFONT", CR_ORANGE, 200,4, 45 + oy,1);

         #define DrawText(txt, pos, cr, x, y) \
            PrintTextFmt("%.*S%S", pos, txt, pos == typeon->len ? Ticker("\n|", "") : "|"), \
            PrintText("CBIFONT", cr, x,1, y+oy,1)

         // render an outline if the page has an image
         if(page->image)
         {
            __str s = Lith_RemoveTextColors(typeon->txt, typeon->pos);
            int len = ACS_StrLen(s);

            DrawText(s, len, CR_BLACK, 112, 61); DrawText(s, len, CR_BLACK, 110, 61);
            DrawText(s, len, CR_BLACK, 112, 59); DrawText(s, len, CR_BLACK, 110, 59);

            DrawText(s, len, CR_BLACK, 111, 59);
            DrawText(s, len, CR_BLACK, 111, 61);

            DrawText(s, len, CR_BLACK, 112, 60);
            DrawText(s, len, CR_BLACK, 110, 60);
         }

         DrawText(typeon->txt, typeon->pos, CR_WHITE, 111.1, 60.1);

         if(page->height) Lith_GUI_ScrollEnd(g, &CBIState(g)->bipinfoscr);
         else             ClearClip();
      }

      avail = bip->categoryavail[bip->curcategory];
      max   = bip->categorymax[bip->curcategory];
   }

   if(bip->curcategory != BIPC_MAIN)
   {
      if(Lith_GUI_Button(g, "<BACK", 20, 38, false, Pre(btnbipback)))
         bip->curcategory = bip->lastcategory;
   }
   else
   {
      PrintSpriteA("lgfx/UI/bip.png", 20,1, 40,1, 0.6);
      PrintTextStr("BIOTIC INFORMATION PANEL ver2.5");
      PrintText("CBIFONT", CR_WHITE, 35,1, 40,1);
   }

   if(max)
   {
      PrintTextFmt("%i/%i AVAILABLE", avail, max);
      PrintText("CBIFONT", CR_WHITE, 300,2, 30,1);
   }
}

// EOF

