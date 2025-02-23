// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Info tab GUI.                                                            │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "p_bip.h"
#include "m_list.h"

static
void EncryptedBody(struct page *page, char *bodytext) {
   faststrcpy_str(bodytext, ns(P_BIP_GetDescr(page)));
   for(char *p = bodytext; *p; p++) {
      *p = !IsPrint(*p) ? *p : *p ^ 7;
   }
}

static
void MailBody(struct page *page, char *bodytext) {
   noinit static char remote[128];
   cstr sent;

   faststrcpy_str(remote, ns(lang(fast_strdup2(LANG "INFO_REMOT_", page->name))));
   sent = CanonTime(ct_full, page->time);

   mem_size_t end = sprintf(bodytext, tmpstr(sl_mail_template), remote, sent);
   bodytext[end++] = '\n';
   bodytext[end++] = '\n';
   faststrcpy_str(&bodytext[end], ns(P_BIP_GetDescr(page)));
}

static
cstr GetShortName(struct page *page) {
   if(page->category == _bipc_mail) {
      return CanonTime(ct_short, page->time);
   } else {
      str infonam = lang(fast_strdup2(LANG "INFO_SHORT_", page->name));
      if(!infonam) {
         return page->name;
      } else {
         return tmpstr(infonam);
      }
   }
}

static
cstr GetFullName(struct page *page) {
   return tmpstr(lang(fast_strdup2(LANG "INFO_TITLE_", page->name)));
}

static
cstr GetBody(struct page *page) {
   noinit static char bodytext[8192];
   switch(page->category) {
   case _bipc_extra:
      EncryptedBody(page, bodytext);
      break;
   case _bipc_mail:
      MailBody(page, bodytext);
      break;
   default:
      faststrcpy_str(bodytext, ns(P_BIP_GetDescr(page)));
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

   G_ScrollReset(g, &pl.cbi.st.bipinfoscr);
}

static
void DrawPage(struct gui_state *g, struct page *page) {
   str image  = lang(fast_strdup2(LANG "INFO_IMAGE_", page->name));
   i32 height =
      faststrtoi32_str(ns(lang(fast_strdup2(LANG "INFO_SSIZE_", page->name))));

   bip.pagebodypos += 14;
   if(bip.pagebodypos > bip.pagebodylen) {
      bip.pagebodypos = bip.pagebodylen;
   }

   i32 const gw = 190;
   i32 const gh = gui_p.scrdef.scrlh * 23;

   if(height) {
      G_ScrBeg(g, &pl.cbi.st.bipinfoscr, 97 - gui_p.scrdef.scrlw,
               27, gw, gh, height * 8 + 20, 184);
   } else {
      G_Clip(g, 97, 27, gw, gh, 184);
      g->ox += 97;
      g->oy += 27;
   }

   if(image) PrintSprite(image, g->ox+186,2, g->oy+140,2, _u_alpha, 0.4);

   BeginPrintStr(GetFullName(page));
   PrintText(sf_lmidfont, CR_ORANGE, g->ox+1,1, g->oy+5,1);

   BeginPrintStrN(bip.pagebody, bip.pagebodypos);
   if(bip.pagebodypos == bip.pagebodylen) {
      if(ACS_Timer() % 35 < 17) {
         _l("\n\Cj|");
      }
   } else {
      _l("\Cj|");
   }
   PrintText(sf_smallfnt, g->defcr, g->ox+1,1, g->oy+20,1, image ? _u_outline : 0);

   if(height) {
      G_ScrEnd(g, &pl.cbi.st.bipinfoscr);
   } else {
      g->oy -= 27;
      g->ox -= 97;
      G_ClipRelease(g);
   }
}

static
void MainUI(struct gui_state *g) {
   gosub_enable();

   i32 n = 0;
   i32 cat;

   PrintText_str(sl_bip_categs, sf_smallfnt, CR_PURPLE, g->ox+27,1, g->oy+57,1);

   bip.lastcategory = _bipc_main;

   gosub(doCateg, cat = _bipc_search);
   for_category() {
      if(categ != _bipc_extra) {
         gosub(doCateg, cat = categ);
      }
   }
   return;

doCateg:
   PrintText_str(ns(lang(fast_strdup2(LANG "BIP_HELP_", P_BIP_CategoryToName(cat)))), sf_smallfnt, g->defcr, g->ox+92,1, g->oy+72+n,1, _u_alpha, 0.7);
   if(G_Button_HId(g, cat, tmpstr(ns(lang(fast_strdup2(LANG "BIP_NAME_", P_BIP_CategoryToName(cat))))), 32, 72 + n, Pre(btnbipmain))) {
      bip.curcategory = cat;
      bip.curpage     = nil;
   }
   n += 10;
   gosub_ret();
}

static
void CategoryUI(struct gui_state *g) {
   i32 categ = bip.curcategory;
   i32 n = bip.categorymax[categ];

   if(categ == _bipc_extra) goto draw;

   G_ScrBeg(g, &pl.cbi.st.bipscr, 2, 37, gui_p.btnlist.w, 170, gui_p.btnlist.h * n);

   i32 i = 0;
   for_page() {
      if(page->category != categ || !get_bit(page->flags, _page_available) ||
         (categ == _bipc_mail && !get_bit(page->flags, _page_unlocked)))
      {
         continue;
      }

      i32 y = gui_p.btnlist.h * i++;

      if(G_ScrOcc(g, &pl.cbi.st.bipscr, y, gui_p.btnlist.h)) {
         continue;
      }

      bool thispage = bip.curpage == page;
      struct gui_pre_btn const *pre;
      /**/ if(thispage) pre = &gui_p.btnlistsel;
      else              pre = &gui_p.btnlist;
      cstr name = GetShortName(page);
      if(G_Button_HId(g, i, name, 0, y, !get_bit(page->flags, _page_unlocked) || thispage, .color = name == page->name ? CR_BLACK : -1, .preset = pre)) {
         SetCurPage(g, page);
      }

      if(get_bit(page->flags, _page_new)) {
         PrintSprite(sp_UI_New, g->ox+gui_p.btnlist.w,2, g->oy+y+gui_p.btnlist.h,2, _u_add|_u_alpha, 0.4);
      }
   }

   G_ScrEnd(g, &pl.cbi.st.bipscr);

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
   struct gui_txt *st = &pl.cbi.st.bipsearch;

   bip.lastcategory = _bipc_main;

   if(G_TxtBox(g, &pl.cbi.st.bipsearch, 10, 52)) {
      static struct {u64 crc; cstr which;} const extranames[] = {
         /* That's a lot of numbers... */
         {0x5F38B6C56F0A6D84L, "Extra1"},
         {0x90215131A36573D7L, "Extra2"},
         {0xC54EC0A7C6836A5BL, "Extra3"},
         {0xB315B81438717BA6L, "Extra4"},
         {0x9FD558A2C8C8D163L, "Extra5"},
         {0xD84CE9973077FD8DL, "Extra6"},
         {0x53FDEB6F120C71EBL, "Extra7"},
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
         AmbientSound(ss_player_cbi_findfail, 1.0);
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

      if((ACS_Timer() % ACS_Random(14, 18)) == 0 && bip.rescur != bip.resnum) {
         AmbientSound(++bip.rescur == bip.resnum ?
                      ss_player_cbi_finddone :
                      ss_player_cbi_find,
                      1.0);
      }
   } else {
      PrintText_str(sl_bip_no_results, sf_smallfnt, CR_DARKGREY, g->ox+57,0, g->oy+82,0);
   }
}

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
      if(G_Button(g, tmpstr(sl_bip_back), 7, 25, false,
                  Pre(btnbipback)))
      {
         bip.curcategory = bip.lastcategory;
      }
   } else {
      PrintSprite(sp_UI_bip, g->ox+7,1, g->oy+27,1, _u_alpha, 0.1);
      PrintText_str(sl_bip_header, sf_lmidfont, g->defcr,
                    g->ox+22,1, g->oy+27,1);
   }

   if(max) {
      ACS_BeginPrint();
      _p(avail);
      _l(" / ");
      _p(max);
      _c(' ');
      _p(sl_bip_available);
      PrintText(sf_smallfnt, g->defcr, g->ox+287,2, g->oy+17,1);
   }
}

/* EOF */
