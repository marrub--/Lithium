/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Info tab GUI.
 *
 * ---------------------------------------------------------------------------|
 */

#define _GNU_SOURCE /* Needed for strcasestr. See: man 7 feature_test_macros */

#include "common.h"
#include "p_player.h"
#include "p_bip.h"
#include "m_list.h"
#include "m_char.h"

/* Static Functions -------------------------------------------------------- */

static void EncryptedBody(struct page *p, char *bodytext) {
   char *s = LanguageCV(bodytext, LANG "INFO_DESCR_%s", p->info->name);
   for(; *s; s++) *s = !IsPrint(*s) ? *s : *s ^ 7;
}

static void MailBody(struct page *p, char *bodytext) {
   char remote[128];
   strcpy(remote, LanguageC(LANG "INFO_REMOT_%s", p->info->name));

   cstr sent = CanonTime(ct_full, p->flags & _page_time);

   sprintf(bodytext, LanguageC(LANG "MAIL_TEMPLATE"), remote, sent);
   strcat(bodytext, "\n\n");
   strcat(bodytext, LanguageC(LANG "INFO_DESCR_%s", p->info->name));
}

static cstr GetShortName(struct page *p) {
   if(p->info->category == BIPC_MAIL) {
      return CanonTime(ct_short, p->flags & _page_time);
   } else {
      return LanguageC(LANG "INFO_SHORT_%s", p->info->name);
   }
}

static cstr GetFullName(struct page *p) {
   return LanguageC(LANG "INFO_TITLE_%s", p->info->name);
}

static cstr GetBody(struct page *p) {
   noinit
   static char bodytext[8192];
   switch(p->info->category) {
   case BIPC_EXTRA:
      EncryptedBody(p, bodytext);
      break;
   case BIPC_MAIL:
      MailBody(p, bodytext);
      break;
   default:
      LanguageCV(bodytext, LANG "INFO_DESCR_%s", p->info->name);
      break;
   }
   return bodytext;
}

static void SetCurPage(struct gui_state *g, struct player *p, struct page *page) {
   str body = l_strdup(GetBody(page));

   p->bip.curpage = page;

   G_TypeOn(g, &CBIState(g)->biptypeon, body);
   G_ScrollReset(g, &CBIState(g)->bipinfoscr);
}

static void DrawPage(struct gui_state *g, struct player *p, struct page *page) {
   str image  = LanguageNull(LANG "INFO_IMAGE_%s", page->info->name);
   i32 height = strtoi_str(Language(LANG "INFO_SSIZE_%s", page->info->name), nil, 10);

   struct gui_typ const *typeon = G_TypeOnUpdate(g, &CBIState(g)->biptypeon);

   i32 const gw = 190;
   i32 const gh = gui_p.scrdef.scrlh * 23;

   if(height) {
      G_ScrBeg(g, &CBIState(g)->bipinfoscr, 97 - gui_p.scrdef.scrlw,
               27, gw, gh, height * 8 + 20, 184);
   } else {
      G_Clip(g, g->ox+97, g->oy+27, gw, gh, 184);
      g->ox += 97;
      g->oy += 27;
   }

   if(image) PrintSpriteA(image, g->ox+186,2, g->oy+140,2, 0.4);

   PrintTextChS(GetFullName(page));
   PrintText(sf_lmidfont, CR_ORANGE, g->ox+1,1, g->oy+5,1);

   Str(st_nl_bar, s"\n|");

   #define DrawText(txt, cr, x, y) \
      PrintTextStr(txt); \
      if(typeon->pos == typeon->len) { \
         if(Ticker(true, false)) {ACS_PrintChar('\n'); ACS_PrintChar('|');} \
      } else { \
         ACS_PrintChar('|'); \
      } \
      PrintText(sf_smallfnt, cr, g->ox+x,1, g->oy+y,1)

   /* render an outline if the page has an image */
   if(image) {
      cstr txt = RemoveTextColors_str(typeon->txt, typeon->pos);
      str s = l_strdup(txt);

      DrawText(s, CR_BLACK, 2, 21); DrawText(s, CR_BLACK, 0, 21);
      DrawText(s, CR_BLACK, 2, 19); DrawText(s, CR_BLACK, 0, 19);

      DrawText(s, CR_BLACK, 1, 19);
      DrawText(s, CR_BLACK, 1, 21);

      DrawText(s, CR_BLACK, 2, 20);
      DrawText(s, CR_BLACK, 0, 20);
   }

   ACS_BeginPrint();
   for(i32 i = 0; i < typeon->pos; i++) {
      ACS_PrintChar(typeon->txt[i]);
   }
   DrawText(ACS_EndStrParam(), g->defcr, 1, 20);

   if(height) {
      G_ScrEnd(g, &CBIState(g)->bipinfoscr);
   } else {
      g->oy -= 27;
      g->ox -= 97;
      G_ClipRelease(g);
   }
}

static void MainUI(struct gui_state *g, struct player *p) {
   Str(sl_bip_categs, sLANG "BIP_CATEGS");

   i32 n = 0;

   PrintText_str(L(sl_bip_categs), sf_smallfnt, CR_PURPLE, g->ox+27,1, g->oy+57,1);

   p->bip.lastcategory = BIPC_MAIN;

   #define Categ(name) { \
      cstr s = LanguageC(LANG "BIP_HELP_%s", P_BIP_CategoryToName(name)); \
      PrintTextChS(s); \
      PrintTextA(sf_smallfnt, g->defcr, g->ox+92,1, g->oy+72+n,1, 0.7); \
      s = LanguageC(LANG "BIP_NAME_%s", P_BIP_CategoryToName(name)); \
      if(G_Button_HId(g, name, s, 32, 72 + n, Pre(btnbipmain))) { \
         p->bip.curcategory = name; \
         p->bip.curpage     = nil; \
      } \
      n += 10; \
   }
   Categ(BIPC_SEARCH);
   for_category() if(categ != BIPC_EXTRA) Categ(categ);
   #undef Categ
}

static void CategoryUI(struct gui_state *g, struct player *p) {
   u32 categ = p->bip.curcategory;
   u32 n = p->bip.categorymax[categ];

   if(categ == BIPC_EXTRA) goto draw;

   G_ScrBeg(g, &CBIState(g)->bipscr, 2, 37, gui_p.btnlist.w, 170, gui_p.btnlist.h * n);

   u32 i = 0;
   for_page() {
      if(page->info->category != categ || !get_bit(page->flags, _page_available) ||
         (categ == BIPC_MAIL && !get_bit(page->flags, _page_unlocked)))
         continue;

      i32 y = gui_p.btnlist.h * i++;

      if(G_ScrOcc(g, &CBIState(g)->bipscr, y, gui_p.btnlist.h))
         continue;

      bool lock = !get_bit(page->flags, _page_unlocked) || p->bip.curpage == page;

      char name[128] = "\Ci";
      strcpy(p->bip.curpage == page ? &name[2] : name, GetShortName(page));

      if(G_Button_HId(g, i, name, 0, y, lock, Pre(btnlist)))
         SetCurPage(g, p, page);
   }

   G_ScrEnd(g, &CBIState(g)->bipscr);

draw:
   if(p->bip.curpage) DrawPage(g, p, p->bip.curpage);
}

script static
i32 SearchPage(struct player *p, struct page *page, cstr query) {
   if(p->bip.resnum >= countof(p->bip.result)) {
      return 0;
   }

   if(!get_bit(page->flags, _page_unlocked) ||
      page->info->category == BIPC_EXTRA) {
      return 1;
   }

   if(strcasestr(GetShortName(page), query) ||
      strcasestr(GetFullName(page),  query) ||
      strcasestr(GetBody(page),      query)) {
      p->bip.result[p->bip.resnum++] = page;
   }

   return 2;
}

static void SearchUI(struct gui_state *g, struct player *p) {
   struct gui_txt *st = G_TxtBox(g, &CBIState(g)->bipsearch, 10, 52, p);

   p->bip.lastcategory = BIPC_MAIN;

   G_TxtBoxEvt(st) {
      /* That's a lot of numbers... */
      struct {
         u64 crc;
         u32 num;
      } const extranames[] = {
         {0x5F38B6C56F0A6D84L, BIP_EXTRA_Extra1},
         {0x90215131A36573D7L, BIP_EXTRA_Extra2},
         {0xC54EC0A7C6836A5BL, BIP_EXTRA_Extra3},
         {0xB315B81438717BA6L, BIP_EXTRA_Extra4},
         {0x9FD558A2C8C8D163L, BIP_EXTRA_Extra5},
      };

      u64 crc = crc64(txt_buf, txt_len);

      char query[128];
      fastmemmove(query, txt_buf, txt_len);
      query[txt_len] = '\0';

      p->bip.resnum = p->bip.rescur = 0;

      for(i32 i = 0; i < countof(extranames); i++) {
         if(crc == extranames[i].crc) {
            p->bip.result[p->bip.resnum++] = &p->bip.pages[extranames[i].num];
         }
      }

      for_page() {
         i32 res = SearchPage(p, page, query);
         /**/ if(res == 0) break;
         else if(res == 1) continue;
      }

      if(p->bip.resnum == 0) {
         ACS_LocalAmbientSound(ss_player_cbi_findfail, 127);
      }
   }

   if(p->bip.resnum) {
      for(i32 i = 0; i < p->bip.rescur; i++) {
         struct page *page = p->bip.result[i];
         cstr flname = GetFullName(page);

         if(G_Button_HId(g, i, flname, 57, 82 + (i * 10), Pre(btnbipmain))) {
            p->bip.lastcategory = p->bip.curcategory;
            p->bip.curcategory  = page->info->category;
            SetCurPage(g, p, page);
         }
      }

      if((ACS_Timer() % ACS_Random(14, 18)) == 0 &&
         p->bip.rescur != p->bip.resnum) {
         if(++p->bip.rescur == p->bip.resnum) {
            ACS_LocalAmbientSound(ss_player_cbi_finddone, 127);
         } else {
            ACS_LocalAmbientSound(ss_player_cbi_find, 127);
         }
      }
   } else {
      Str(sl_bip_no_results, sLANG "BIP_NO_RESULTS");
      PrintText_str(L(sl_bip_no_results), sf_smallfnt, CR_DARKGREY, g->ox+57,0, g->oy+82,0);
   }
}

/* Extern Functions -------------------------------------------------------- */

void P_CBI_TabBIP(struct gui_state *g, struct player *p) {
   i32 avail, max = 0;

   if(p->bip.curcategory == BIPC_MAIN) {
      MainUI(g, p);
      avail = p->bip.pageavail;
      max   = p->bip.pagemax;
   } else if(p->bip.curcategory == BIPC_SEARCH) {
      SearchUI(g, p);
   } else {
      CategoryUI(g, p);
      avail = p->bip.categoryavail[p->bip.curcategory];
      max   = p->bip.categorymax[p->bip.curcategory];
   }

   if(p->bip.curcategory != BIPC_MAIN) {
      if(G_Button(g, LC(LANG "BIP_BACK"), 7, 25, false, Pre(btnbipback)))
         p->bip.curcategory = p->bip.lastcategory;
   } else {
      Str(sl_bip_header, sLANG "BIP_HEADER");
      PrintSpriteA(sp_UI_bip, g->ox+7,1, g->oy+27,1, 0.1);
      PrintText_str(L(sl_bip_header), sf_lmidfont, g->defcr, g->ox+22,1, g->oy+27,1);
   }

   if(max) {
      PrintTextFmt(LC(LANG "BIP_AVAILABLE"), avail, max);
      PrintText(sf_smallfnt, g->defcr, g->ox+287,2, g->oy+17,1);
   }
}

/* EOF */
