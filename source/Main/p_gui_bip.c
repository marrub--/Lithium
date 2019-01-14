// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// p_gui_bip.c: Info tab GUI.

#include "common.h"
#include "p_player.h"
#include "p_bip.h"
#include "m_list.h"

static void SetCurPage(struct gui_state *g, struct bip *bip, struct page *page, str body)
{
   bip->curpage = page;

   G_TypeOn(g, &CBIState(g)->biptypeon, body);
   G_ScrollReset(g, &CBIState(g)->bipinfoscr);
}

script
static bool CheckMatch(struct page_info *pinf, str query)
{
   return strcasestr_str(pinf->shname, query) ||
          strcasestr_str(pinf->flname, query) ||
          strcasestr_str(pinf->body,   query);
}

static void MainUI(struct gui_state *g, struct player *p, struct bip *bip)
{
   i32 n = 0;

   PrintText_str(st_info_categories, s_cbifont, CR_PURPLE, 40,1, 70,1);

   bip->lastcategory = BIPC_MAIN;

   str const lines[] = {
      L(st_bip_help_search),
      #define LITH_X(name, capt) L(st_bip_help_##name),
      #include "p_bip.h"
   };

   for(i32 i = 0; i < countof(lines); i++)
      PrintTextA_str(lines[i], s_cbifont, CR_WHITE, 105,1, 85+n + i*10,1, 0.7);

   if(G_Button(g, LC(LANG "BIP_NAME_Search"), 45, 85 + n, Pre(btnbipmain)))
      bip->curcategory = BIPC_SEARCH;
   n += 10;
   #define LITH_X(name, capt) \
      if(G_Button_Id(g, BIPC_##name, LC(cLANG "BIP_NAME_" capt), 45, 85 + n, Pre(btnbipmain))) \
      { \
         bip->curcategory = BIPC_##name; \
         bip->curpage     = nil; \
      } \
      n += 10;
   #include "p_bip.h"
}

static void SearchUI(struct gui_state *g, struct player *p, struct bip *bip)
{
   struct gui_txt *st = G_TextBox(g, &CBIState(g)->bipsearch, 23, 65, p);

   bip->lastcategory = BIPC_MAIN;

   G_TextBox_OnTextEntered(st)
   {
      // That's a lot of numbers...
      u64 const extranames[] = {
         0x5F38B6C56F0A6D84L,
         0x90215131A36573D7L,
         0xC54EC0A7C6836A5BL,
         0xB315B81438717BA6L,
         0x9FD558A2C8C8D163L,
      };

      str query = l_strndup(txt_buf, txt_len);
      u64 crc = crc64(txt_buf, txt_len);

      bip->resnum = bip->rescur = 0;

      for(i32 i = 0; i < countof(extranames); i++)
      {
         if(crc == extranames[i])
         {
            list *link = bip->infogr[BIPC_EXTRA].next;
            for(i32 j = 0; j < i; j++)
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

         struct page_info pinf = PageInfo(page);

         if(CheckMatch(&pinf, query))
            bip->result[bip->resnum++] = page;
      }

      if(bip->resnum == 0)
         ACS_LocalAmbientSound(ss_player_cbi_findfail, 127);
   }

   if(bip->resnum)
   {
      for(i32 i = 0; i < bip->rescur; i++)
      {
         struct page *page = bip->result[i];
         struct page_info pinf = PageInfo(page);
         char flname[128]; lstrcpy_str(flname, pinf.flname);

         if(G_Button_Id(g, i, flname, 70, 95 + (i * 10), Pre(btnbipmain)))
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
            ACS_LocalAmbientSound(ss_player_cbi_finddone, 127);
         else
            ACS_LocalAmbientSound(ss_player_cbi_find, 127);
      }
   }
   else
      PrintText_str(st_no_results, s_cbifont, CR_DARKGREY, 70,0, 95,0);
}

static void DrawPage(struct gui_state *g, struct player *p, struct bip *bip)
{
   struct page *page = bip->curpage;
   struct page_info pinf = PageInfo(page);

   struct gui_typ const *typeon = G_TypeOnUpdate(g, &CBIState(g)->biptypeon);

   i32 oy = 0;

   if(page->height)
   {
      G_ScrollBegin(g, &CBIState(g)->bipinfoscr, 100, 40, 200, 180, page->height * 8 + 20, 184);
      oy = g->oy - 40;
   }
   else
      SetClipW(111, 40, 200, 180, 184);

   if(page->image) PrintSpriteA(page->image, 296,2, 180,2, 0.4);

   PrintText_str(pinf.flname, s_cbifont, CR_ORANGE, 200,4, 45 + oy,1);

   #define DrawText(txt, cr, x, y) \
      PrintTextStr(txt); \
      if(typeon->pos == typeon->len) {if(Ticker(true, false)) ACS_PrintString(st_nl_bar);} \
      else                                                    ACS_PrintChar('|'); \
      PrintText(s_cbifont, cr, x,1, y+oy,1)

   // render an outline if the page has an image
   if(page->image)
   {
      char const *txt = RemoveTextColors_str(typeon->txt, typeon->pos);
      str s = l_strdup(txt);

      DrawText(s, CR_BLACK, 112, 61); DrawText(s, CR_BLACK, 110, 61);
      DrawText(s, CR_BLACK, 112, 59); DrawText(s, CR_BLACK, 110, 59);

      DrawText(s, CR_BLACK, 111, 59);
      DrawText(s, CR_BLACK, 111, 61);

      DrawText(s, CR_BLACK, 112, 60);
      DrawText(s, CR_BLACK, 110, 60);
   }

   ACS_BeginPrint();
   __nprintf("%.*S", typeon->pos, typeon->txt);
   DrawText(ACS_EndStrParam(), CR_WHITE, 111, 60);

   if(page->height) G_ScrollEnd(g, &CBIState(g)->bipinfoscr);
   else             ClearClip();
}

static void CategoryUI(struct gui_state *g, struct player *p, struct bip *bip)
{
   list *ls = &bip->infogr[bip->curcategory];
   size_t n = ls->size();
   size_t i = 0;

   G_ScrollBegin(g, &CBIState(g)->bipscr, 15, 50, gui_p.btnlist.w, 170, gui_p.btnlist.h * n);

   if(bip->curcategory != BIPC_EXTRA)
      for_list_it(struct page *page, *ls, i++)
   {
      i32 y = gui_p.btnlist.h * i;

      if(G_ScrollOcclude(g, &CBIState(g)->bipscr, y, gui_p.btnlist.h))
         continue;

      struct page_info pinf = PageInfo(page);
      char name[128] = "\Ci";
      lstrcpy_str(bip->curpage == page ? &name[2] : name, pinf.shname);

      if(G_Button_Id(g, i, name, 0, y, !page->unlocked || bip->curpage == page, Pre(btnlist)))
         SetCurPage(g, bip, page, pinf.body);
   }

   G_ScrollEnd(g, &CBIState(g)->bipscr);

   if(bip->curpage) DrawPage(g, p, bip);
}

void P_CBI_TabBIP(struct gui_state *g, struct player *p)
{
   struct bip *bip = &p->bip;
   i32 avail, max = 0;

   if(bip->curcategory == BIPC_MAIN)
   {
      MainUI(g, p, bip);
      avail = bip->pageavail;
      max   = bip->pagemax;
   }
   else if(bip->curcategory == BIPC_SEARCH)
      SearchUI(g, p, bip);
   else
   {
      CategoryUI(g, p, bip);
      avail = bip->categoryavail[bip->curcategory];
      max   = bip->categorymax[bip->curcategory];
   }

   if(bip->curcategory != BIPC_MAIN)
   {
      if(G_Button(g, "<BACK", 20, 38, false, Pre(btnbipback)))
         bip->curcategory = bip->lastcategory;
   }
   else
   {
      PrintSpriteA(sp_UI_bip, 20,1, 40,1, 0.1);
      PrintText_str(st_bip_header, s_cbifont, CR_WHITE, 35,1, 40,1);
   }

   if(max)
   {
      PrintTextFmt("%i/%i AVAILABLE", avail, max);
      PrintText(s_cbifont, CR_WHITE, 300,2, 30,1);
   }
}

// EOF
