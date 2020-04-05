/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
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

static cstr EncryptedBody(struct page *p) {
   char *s = LanguageC(LANG "INFO_DESCR_%s", p->info->name);
   for(; *s; s++) *s = !IsPrint(*s) ? *s : *s ^ 7;
   return s;
}

static cstr MailBody(struct page *p) {
   noinit static char body[8192];

   char remote[64];
   strcpy(remote, LanguageC(LANG "INFO_REMOT_%s", p->info->name));

   cstr sent = CanonTime(ct_full, p->flags & _page_time);

   sprintf(body, LanguageC(LANG "MAIL_TEMPLATE"), remote, sent);
   strcat(body, "\n\n");
   strcat(body, LanguageC(LANG "INFO_DESCR_%s", p->info->name));

   return body;
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
   switch(p->info->category) {
      case BIPC_EXTRA: return EncryptedBody(p);
      case BIPC_MAIL:  return      MailBody(p);
   }
   return LanguageC(LANG "INFO_DESCR_%s", p->info->name);
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

   i32 oy = 0;

   if(height) {
      G_ScrollBegin(g, &CBIState(g)->bipinfoscr, 100, 40, 200, 180,
                    height * 8 + 20, 184);
      oy = g->oy - 40;
   } else {
      SetClipW(110, 40, 201, 180, 185);
   }

   if(image) PrintSpriteA(image, 296,2, 180,2, 0.4);

   PrintTextChS(GetFullName(page));
   PrintText(s_lmidfont, CR_ORANGE, 111,1, 45 + oy,1);

   Str(nl_bar, s"\n|");

   #define DrawText(txt, cr, x, y) \
      PrintTextStr(txt); \
      if(typeon->pos == typeon->len) { \
         if(Ticker(true, false)) {ACS_PrintChar('\n'); ACS_PrintChar('|');} \
      } else { \
         ACS_PrintChar('|'); \
      } \
      PrintText(s_smallfnt, cr, x,1, y+oy,1)

   /* render an outline if the page has an image */
   if(image) {
      cstr txt = RemoveTextColors_str(typeon->txt, typeon->pos);
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

   if(height) G_ScrollEnd(g, &CBIState(g)->bipinfoscr);
   else             ClearClip();
}

static void MainUI(struct gui_state *g, struct player *p) {
   i32 n = 0;

   PrintText_str(st_info_categories, s_smallfnt, CR_PURPLE, 40,1, 70,1);

   p->bip.lastcategory = BIPC_MAIN;

   #define Categ(name) { \
      cstr s = LanguageC(LANG "BIP_HELP_%s", P_BIP_CategoryToName(name)); \
      PrintTextChS(s); \
      PrintTextA(s_smallfnt, CR_WHITE, 105,1, 85+n,1, 0.7); \
      s = LanguageC(LANG "BIP_NAME_%s", P_BIP_CategoryToName(name)); \
      if(G_Button_Id(g, name, s, 45, 85 + n, Pre(btnbipmain))) { \
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
   u32 n = p->bip.categoryavail[categ];

   if(categ == BIPC_EXTRA) goto draw;

   G_ScrollBegin(g, &CBIState(g)->bipscr, 15, 50, gui_p.btnlist.w, 170, gui_p.btnlist.h * n);

   u32 i = 0;
   for_page() {
      if(page->info->category != categ || !(page->flags & _page_available) ||
         (categ == BIPC_MAIL && !(page->flags & _page_unlocked)))
         continue;

      i32 y = gui_p.btnlist.h * i;

      if(G_ScrollOcclude(g, &CBIState(g)->bipscr, y, gui_p.btnlist.h))
         continue;

      bool lock = !(page->flags & _page_unlocked) || p->bip.curpage == page;

      char name[128] = "\Ci";
      strcpy(p->bip.curpage == page ? &name[2] : name, GetShortName(page));

      if(G_Button_Id(g, i, name, 0, y, lock, Pre(btnlist)))
         SetCurPage(g, p, page);

      i++;
   }

   G_ScrollEnd(g, &CBIState(g)->bipscr);

draw:
   if(p->bip.curpage) DrawPage(g, p, p->bip.curpage);
}

static void SearchUI(struct gui_state *g, struct player *p) {
   struct gui_txt *st = G_TextBox(g, &CBIState(g)->bipsearch, 23, 65, p);

   p->bip.lastcategory = BIPC_MAIN;

   G_TextBox_OnTextEntered(st) {
      /* That's a lot of numbers... */
      struct extraname {
         u64 crc;
         u32 num;
      };

      struct extraname const extranames[] = {
         {0x5F38B6C56F0A6D84L, BIP_EXTRA_Extra1},
         {0x90215131A36573D7L, BIP_EXTRA_Extra2},
         {0xC54EC0A7C6836A5BL, BIP_EXTRA_Extra3},
         {0xB315B81438717BA6L, BIP_EXTRA_Extra4},
         {0x9FD558A2C8C8D163L, BIP_EXTRA_Extra5},
      };

      u64 crc = crc64(txt_buf, txt_len);

      char query[128];
      memmove(query, txt_buf, txt_len);
      query[txt_len] = '\0';

      p->bip.resnum = p->bip.rescur = 0;

      for(i32 i = 0; i < countof(extranames); i++) {
         if(crc == extranames[i].crc) {
            p->bip.result[p->bip.resnum++] = &p->bip.pages[extranames[i].num];
         }
      }

      for_page() {
         if(p->bip.resnum >= countof(p->bip.result)) {
            break;
         }

         if(!(page->flags & _page_unlocked) ||
            page->info->category == BIPC_EXTRA) {
            continue;
         }

         if(strcasestr(GetShortName(page), query) ||
            strcasestr(GetFullName(page),  query) ||
            strcasestr(GetBody(page),      query)) {
            p->bip.result[p->bip.resnum++] = page;
         }
      }

      if(p->bip.resnum == 0) {
         ACS_LocalAmbientSound(ss_player_cbi_findfail, 127);
      }
   }

   if(p->bip.resnum) {
      for(i32 i = 0; i < p->bip.rescur; i++) {
         struct page *page = p->bip.result[i];
         cstr flname = GetFullName(page);

         if(G_Button_Id(g, i, flname, 70, 95 + (i * 10), Pre(btnbipmain))) {
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
      PrintText_str(st_no_results, s_smallfnt, CR_DARKGREY, 70,0, 95,0);
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
      /* TODO */
      if(G_Button(g, "<BACK", 20, 38, false, Pre(btnbipback)))
         p->bip.curcategory = p->bip.lastcategory;
   } else {
      PrintSpriteA(sp_UI_bip, 20,1, 40,1, 0.1);
      PrintText_str(st_bip_header, s_lmidfont, CR_WHITE, 35,1, 40,1);
   }

   if(max) {
      /* TODO */
      PrintTextFmt("%i/%i AVAILABLE", avail, max);
      PrintText(s_smallfnt, CR_WHITE, 300,2, 30,1);
   }
}

/* EOF */
