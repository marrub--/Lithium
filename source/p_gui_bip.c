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

#include "common.h"
#include "p_player.h"
#include "p_bip.h"
#include "m_list.h"

/* Static Functions -------------------------------------------------------- */

static
void EncryptedBody(struct page *page, char *bodytext) {
   faststrcpy_str(bodytext,
                  ns(lang_fmt(LANG "INFO_DESCR_%s", page->name)));
   for(char *p = bodytext; *p; p++) {
      *p = !IsPrint(*p) ? *p : *p ^ 7;
   }
}

static
void MailBody(struct page *page, char *bodytext) {
   noinit static
   char remote[128];
   cstr sent;

   faststrcpy_str(remote, ns(lang_fmt(LANG "INFO_REMOT_%s", page->name)));
   sent = CanonTime(ct_full, page->time);

   mem_size_t end =
      sprintf(bodytext, tmpstr(lang(sl_mail_template)), remote, sent);
   bodytext[end++] = '\n';
   bodytext[end++] = '\n';
   faststrcpy_str(&bodytext[end],
                  ns(lang_fmt(LANG "INFO_DESCR_%s", page->name)));
}

static
cstr GetShortName(struct page *page) {
   if(page->category == _bipc_mail) {
      return CanonTime(ct_short, page->time);
   } else {
      return tmpstr(lang_fmt(LANG "INFO_SHORT_%s", page->name));
   }
}

static
cstr GetFullName(struct page *page) {
   return tmpstr(lang_fmt(LANG "INFO_TITLE_%s", page->name));
}

static
cstr GetBody(struct page *page) {
   noinit static
   char bodytext[8192];
   switch(page->category) {
   case _bipc_extra:
      EncryptedBody(page, bodytext);
      break;
   case _bipc_mail:
      MailBody(page, bodytext);
      break;
   default:
      faststrcpy_str(bodytext,
                     ns(lang_fmt(LANG "INFO_DESCR_%s", page->name)));
      break;
   }
   return bodytext;
}

static
void SetCurPage(struct gui_state *g, struct page *page) {
   i32 oldf = page->flags;
   dis_bit(page->flags, _page_new);
   if(page->flags != oldf) {
      P_Data_Save();
   }

   bip.curpage     = page;
   bip.pagebody    = GetBody(page);
   bip.pagebodypos = 0;
   bip.pagebodylen = faststrlen(bip.pagebody);

   G_ScrollReset(g, &CBIState(g)->bipinfoscr);
}

static
void DrawPage(struct gui_state *g, struct page *page) {
   gosub_enable();

   str image  = lang_fmt(LANG "INFO_IMAGE_%s", page->name);
   i32 height =
      faststrtoi32_str(ns(lang_fmt(LANG "INFO_SSIZE_%s", page->name)));

   i32 cr, x, y;

   bip.pagebodypos += 14;
   if(bip.pagebodypos > bip.pagebodylen) {
      bip.pagebodypos = bip.pagebodylen;
   }

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

   /* render an outline if the page has an image */
   if(image) {
      static
      i32 xs[8] = { 2,  0,  2,  0,  1,  1,  2,  0},
          ys[8] = {21, 21, 19, 19, 19, 21, 20, 20};

      cstr txt = RemoveTextColors(bip.pagebody, bip.pagebodypos);

      for(mem_size_t i = 0; i < 8; ++i) {
         gosub(drawText, PrintTextChS(txt),
               cr = CR_BLACK, x = xs[i], y = ys[i]);
      }
   }

   gosub(drawText, PrintTextChr(bip.pagebody, bip.pagebodypos),
         cr = g->defcr, x = 1, y = 20);

   if(height) {
      G_ScrEnd(g, &CBIState(g)->bipinfoscr);
   } else {
      g->oy -= 27;
      g->ox -= 97;
      G_ClipRelease(g);
   }
   return;

drawText:
   if(bip.pagebodypos == bip.pagebodylen) {
      if(Ticker(true, false)) {
         ACS_PrintChar('\n');
         ACS_PrintChar('|');
      }
   } else {
      ACS_PrintChar('|');
   }
   PrintText(sf_smallfnt, cr, g->ox+x,1, g->oy+y,1);
   gosub_ret();
}

static
void MainUI(struct gui_state *g) {
   gosub_enable();

   i32 n = 0;
   i32 cat;

   PrintText_str(ns(lang(sl_bip_categs)), sf_smallfnt, CR_PURPLE, g->ox+27,1, g->oy+57,1);

   bip.lastcategory = _bipc_main;

   gosub(doCateg, cat = _bipc_search);
   for_category() {
      if(categ != _bipc_extra) {
         gosub(doCateg, cat = categ);
      }
   }
   return;

doCateg:
   PrintTextA_str(ns(lang_fmt(LANG "BIP_HELP_%s", P_BIP_CategoryToName(cat))),
                  sf_smallfnt, g->defcr, g->ox+92,1, g->oy+72+n,1, 0.7);
   if(G_Button_HId(g, cat, tmpstr(ns(lang_fmt(LANG "BIP_NAME_%s",
                                              P_BIP_CategoryToName(cat)))),
                   32, 72 + n, Pre(btnbipmain)))
   {
      bip.curcategory = cat;
      bip.curpage     = nil;
   }
   n += 10;
   gosub_ret();
}

static
void CategoryUI(struct gui_state *g) {
   u32 categ = bip.curcategory;
   u32 n = bip.categorymax[categ];

   if(categ == _bipc_extra) goto draw;

   G_ScrBeg(g, &CBIState(g)->bipscr, 2, 37, gui_p.btnlist.w, 170, gui_p.btnlist.h * n);

   u32 i = 0;
   for_page() {
      if(page->category != categ || !get_bit(page->flags, _page_available) ||
         (categ == _bipc_mail && !get_bit(page->flags, _page_unlocked)))
      {
         continue;
      }

      i32 y = gui_p.btnlist.h * i++;

      if(G_ScrOcc(g, &CBIState(g)->bipscr, y, gui_p.btnlist.h)) {
         continue;
      }

      bool lock = !get_bit(page->flags, _page_unlocked) || bip.curpage == page;

      char name[128] = "\Ci";
      faststrcpy(bip.curpage == page ? &name[2] : name, GetShortName(page));

      if(G_Button_HId(g, i, name, 0, y, lock, Pre(btnlist))) {
         SetCurPage(g, page);
      }

      if(get_bit(page->flags, _page_new)) {
         PrintSpriteAP(sp_UI_New, g->ox+gui_p.btnlist.w,2, g->oy+y+gui_p.btnlist.h,2, 0.4);
      }
   }

   G_ScrEnd(g, &CBIState(g)->bipscr);

draw:
   if(bip.curpage) DrawPage(g, bip.curpage);
}

script static
i32 SearchPage(struct page *page, cstr query) {
   if(bip.resnum >= countof(bip.result)) {
      return 0;
   }

   if(!get_bit(page->flags, _page_unlocked) ||
      page->category == _bipc_extra) {
      return 1;
   }

   if(faststrcasestr(GetShortName(page), query) ||
      faststrcasestr(GetFullName(page),  query) ||
      faststrcasestr(GetBody(page),      query)) {
      bip.result[bip.resnum++] = page;
   }

   return 2;
}

static
void SearchUI(struct gui_state *g) {
   struct gui_txt *st = &CBIState(g)->bipsearch;

   bip.lastcategory = _bipc_main;

   if(G_TxtBox(g, &CBIState(g)->bipsearch, 10, 52)) {
      struct extraname {
         u64  crc;
         cstr which;
      };

      static
      struct extraname const extranames[] = {
         /* That's a lot of numbers... */
         {0x5F38B6C56F0A6D84L, "Extra1"},
         {0x90215131A36573D7L, "Extra2"},
         {0xC54EC0A7C6836A5BL, "Extra3"},
         {0xB315B81438717BA6L, "Extra4"},
         {0x9FD558A2C8C8D163L, "Extra5"},
      };

      noinit static
      char query[128];
      fastmemcpy(query, Cps_Expand(st->txtbuf, 0, st->tbptr), st->tbptr);
      query[st->tbptr] = '\0';

      bip.resnum = bip.rescur = 0;

      u64 crc = crc64(query, st->tbptr);
      for(i32 i = 0; i < countof(extranames); i++) {
         if(crc == extranames[i].crc) {
            bip.result[bip.resnum++] = P_BIP_NameToPage(extranames[i].which);
         }
      }

      for_page() {
         i32 res = SearchPage(page, query);
         /**/ if(res == 0) break;
         else if(res == 1) continue;
      }

      if(bip.resnum == 0) {
         ACS_LocalAmbientSound(ss_player_cbi_findfail, 127);
      }
   }

   if(bip.resnum) {
      for(i32 i = 0; i < bip.rescur; i++) {
         struct page *page = bip.result[i];
         cstr flname = GetFullName(page);

         if(G_Button_HId(g, i, flname, 57, 82 + (i * 10), Pre(btnbipmain))) {
            bip.lastcategory = bip.curcategory;
            bip.curcategory  = page->category;
            SetCurPage(g, page);
         }
      }

      if((ACS_Timer() % ACS_Random(14, 18)) == 0 &&
         bip.rescur != bip.resnum) {
         if(++bip.rescur == bip.resnum) {
            ACS_LocalAmbientSound(ss_player_cbi_finddone, 127);
         } else {
            ACS_LocalAmbientSound(ss_player_cbi_find, 127);
         }
      }
   } else {
      PrintText_str(ns(lang(sl_bip_no_results)), sf_smallfnt, CR_DARKGREY, g->ox+57,0, g->oy+82,0);
   }
}

/* Extern Functions -------------------------------------------------------- */

void P_CBI_TabBIP(struct gui_state *g) {
   i32 avail, max = 0;

   if(bip.curcategory == _bipc_main) {
      MainUI(g);
      avail = bip.pageavail;
      max   = bip.pagemax;
   } else if(bip.curcategory == _bipc_search) {
      SearchUI(g);
   } else {
      CategoryUI(g);
      avail = bip.categoryavail[bip.curcategory];
      max   = bip.categorymax[bip.curcategory];
   }

   if(bip.curcategory != _bipc_main) {
      if(G_Button(g, tmpstr(lang(sl_bip_back)), 7, 25, false,
                  Pre(btnbipback)))
      {
         bip.curcategory = bip.lastcategory;
      }
   } else {
      PrintSpriteA(sp_UI_bip, g->ox+7,1, g->oy+27,1, 0.1);
      PrintText_str(ns(lang(sl_bip_header)), sf_lmidfont, g->defcr,
                    g->ox+22,1, g->oy+27,1);
   }

   if(max) {
      PrintTextFmt(tmpstr(lang(sl_bip_available)), avail, max);
      PrintText(sf_smallfnt, g->defcr, g->ox+287,2, g->oy+17,1);
   }
}

/* EOF */
