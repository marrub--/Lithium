// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_bip.h"
#include "lith_list.h"

//
// SetCurPage
//
static void SetCurPage(gui_state_t *g, struct bip *bip, struct page *page, __str body)
{
   bip->curpage = page;

   Lith_GUI_TypeOn(g, &CBIState(g)->biptypeon, body);
   Lith_GUI_ScrollReset(g, &CBIState(g)->bipinfoscr);
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
   struct bip *bip = &p->bip;
   int avail, max;

   if(bip->curcategory == BIPC_MAIN)
   {
      int n = 0;

      PrintTextStr("INFO CATEGORIES");
      PrintText("CBIFONT", CR_PURPLE, 40,1, 70,1);

      bip->lastcategory = BIPC_MAIN;

      __str const lines[] = {
         L("LITH_BIP_HELP_Search"),
#define LITH_X(name, capt) L("LITH_BIP_HELP_" capt),
#include "lith_bip.h"
      };

      for(int i = 0; i < countof(lines); i++)
      {
         PrintTextStr(lines[i]);
         PrintTextA("CBIFONT", CR_WHITE, 105,1, 85+n + i*10,1, 0.7);
      }

      if(Lith_GUI_Button(g, L("LITH_BIP_NAME_Search"), 45, 85 + n, Pre(btnbipmain)))
         bip->curcategory = BIPC_SEARCH;
      n += 10;
#define LITH_X(name, capt) \
      if(Lith_GUI_Button_Id(g, BIPC_##name, L("LITH_BIP_NAME_" capt), 45, 85 + n, Pre(btnbipmain))) \
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
      gui_txtbox_state_t *st = Lith_GUI_TextBox(g, &CBIState(g)->bipsearch, 23, 65, p);

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
         u64 crc = crc64_str(txt_buf, txt_len);

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

            struct page_info pinf = Lith_GetPageInfo(page);

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
            struct page *page = bip->result[i];
            struct page_info pinf = Lith_GetPageInfo(page);

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
         forlistIt(struct page *page, *list, i++)
      {
         int y = guipre.btnlist.h * i;

         if(Lith_GUI_ScrollOcclude(g, &CBIState(g)->bipscr, y, guipre.btnlist.h))
            continue;

         struct page_info pinf = Lith_GetPageInfo(page);
         __str name = StrParam("%S%S", bip->curpage == page ? "\Ci" : "", pinf.shname);

         if(Lith_GUI_Button_Id(g, i, name, 0, y, !page->unlocked || bip->curpage == page, Pre(btnlist)))
            SetCurPage(g, bip, page, pinf.body);
      }

      Lith_GUI_ScrollEnd(g, &CBIState(g)->bipscr);

      if(bip->curpage)
      {
         struct page *page = bip->curpage;
         struct page_info pinf = Lith_GetPageInfo(page);

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

         PrintSpriteA(":UI:Background", 0,1, 0,1, 0.5);

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
      PrintSpriteA(":UI:bip", 20,1, 40,1, 0.1);
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
