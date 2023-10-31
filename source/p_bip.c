// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Info page handling and loading.                                          │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "p_bip.h"
#include "m_list.h"
#include "w_world.h"
#include "m_file.h"
#include "m_tokbuf.h"
#include "m_trie.h"

noinit struct bip bip;
noinit char bipstring[0x7FFF];
mem_size_t  bipstrptr;
noinit struct page bippages[512];
mem_size_t         bippagenum;

script static void MailNotifyFinish(cstr name) {
   char remote[128];
   faststrcpy_str(remote, ns(lang(fast_strdup2(LANG "INFO_REMOT_", name))));
   P_LogB(1, tmpstr(sl_log_mailrecv), remote);
   if(ACS_Random(1, 10000) == 1) {
      bip.mailtrulyreceived++;
      AmbientSound(ss_player_YOUVEGOTMAIL, 1.0);
   } else {
      AmbientSound(ss_player_cbi_mail, 1.0);
   }
}

alloc_aut(0) script static void MailNotify(cstr name) {
   pl.setActivator();
   bip.mailreceived++;
   #ifndef NDEBUG
   if(dbgflags(dbgf_bip)) {
      return;
   }
   #endif
   ACS_Delay(20);
   MailNotifyFinish(name);
}

script cstr BipStr(cstr in) {
   cstr       out = &bipstring[bipstrptr];
   mem_size_t len = faststrlen(in) + 1;
   fastmemcpy(out, in, len);
   bipstrptr += len;
   return out;
}

script static void BipInfo_Page(struct tokbuf *tb, struct err *res, struct page const *template) {
   noinit static char k[64], v[64];
   struct page *page = &bippages[bippagenum++];
   fastmemcpy(page, template, sizeof *page);
   while(tb_kv(tb, res, k, v)) {
      unwrap(res);
      switch(BipInfo_Page_Name(k)) {
      case _bipinfo_page_cl: {
         i32 pcl = P_ClassToInt(v);
         if(pcl != -1) {
            page->pclass = pcl;
         } else {
            tb_err(tb, res, "invalid pclass %s", nil, _f, v);
            unwrap_retn();
         }
         break;
      }
      case _bipinfo_page_tag:
         page->name = BipStr(v);
         break;
      case _bipinfo_page_unl: {
         i32 i = 0;
         for(char *next = nil, *word = faststrtok(v, &next, ' '); word;
             word = faststrtok(nil, &next, ' ')) {
            page->unlocks[i++] = BipStr(word);
         }
         break;
      }
      default:
         tb_err(tb, res, "invalid key %s; expected "
                "cl, "
                "tag, "
                "or unl",
                nil, _f, k);
         unwrap_retn();
      }
   }
   unwrap(res);
   Dbg_Log(log_gsinfo, _l("added bip page '"), _p(page->name), _c('\''));
   return;
}

static struct page BipInfo_Template(struct tokbuf *tb, struct err *res) {
   struct token *tok = tb_reget(tb);
   struct page template = {};
   i32 tflags = tb_rflag(tb, res, tok->textV, BipInfo_Template_Name);
   unwrap(res);
   #define bip_category_x(name) \
   if(get_bit(tflags, _bipinfo_template_##name)) { \
      template.category = _bipc_##name; \
   }
   #include "p_bip.h"
   if(get_bit(tflags, _bipinfo_template_auto)) {
      set_bit(template.flags, _page_auto);
   }
   return template;
}

script static void BipInfo_Pages(struct tokbuf *tb, struct err *res) {
   struct page const template = BipInfo_Template(tb, res);
   unwrap(res);
   tb_expdr(tb, res, tok_braceo);
   unwrap(res);
   while(!tb_drop(tb, tok_bracec)) {
      BipInfo_Page(tb, res, &template);
      unwrap(res);
   }
}

script static void BipInfo_Compile(struct tokbuf *tb, struct err *res) {
   for(;;) {
      struct token *tok = tb_expc(tb, res, tb_get(tb), tok_string, tok_eof, 0);
      unwrap(res);
      if(tok->type == tok_eof) {
         return;
      } else {
         BipInfo_Pages(tb, res);
         unwrap(res);
      }
   }
}

script static void P_BIP_InitInfo(void) {
   Dbg_Log(log_dev, _l(_f));
   bippagenum = 0;
   i32 prev = 0;
   for(FILE *fp; (fp = W_OpenIter(sp_LITHINFO, 't', &prev));) {
      struct tokbuf tb;
      tb_ctor(&tb, fp, "LITHINFO");
      struct err res = {};
      BipInfo_Compile(&tb, &res);
      unwrap_print(&res);
      tb_dtor(&tb);
      fclose(fp);
   }
}

script void P_BIP_PInit(void) {
   if(!bippagenum) P_BIP_InitInfo();
   #ifndef NDEBUG
   if(dbglevel_any()) {
      P_LogH(1, "There are %i info pages!", bippagenum);
   }
   #endif
   bip.pagemax = 0;
   for_category() {
      bip.categorymax[categ] = 0;
   }
   for_page() {
      bool avail = get_bit(page->pclass, pl.pclass);
      if(avail) {
         set_bit(page->flags, _page_available);
         bip.pagemax++;
         bip.categorymax[page->category]++;
      }
   }
   for_page() {
      if(get_bit(page->flags, _page_available) && (
         #ifndef NDEBUG
         dbgflags(dbgf_bip) ||
         #endif
         get_bit(page->flags, _page_auto)))
      {
         P_BIP_Unlock(page, false);
      }
   }
}

script void P_BIP_Unlock(struct page *page, bool from_load) {
   if(!page) {
      PrintErr(_l("page was null"));
      return;
   }
   bool was_unlocked = get_bit(page->flags, _page_unlocked);
   set_bit(page->flags, _page_unlocked);
   /* don't do the rest of the unlock process if this page isn't available
    * if this wasn't from a load, it's probably in error that it isn't
    */
   if(!get_bit(page->flags, _page_available)) {
      if(!from_load) {
         Dbg_Log(log_dev, _l("page '"), _p(page->name), _l("' not available"));
      }
      return;
   }
   if(!was_unlocked) {
      Dbg_Log(log_bip, _l("unlocking page '"), _p(page->name), _c('\''));
      bip.pageavail++;
      bip.categoryavail[page->category]++;
      if(page->category == _bipc_mail && !get_bit(page->flags, _page_auto)) {
         MailNotify(page->name);
      }
      page->time = ACS_Timer();
      if(!from_load && page->category <= _bipc_last_normal) {
         set_bit(page->flags, _page_new);
         if(!get_bit(page->flags, _page_auto) && ACS_Timer() > 3) {
            P_Data_Save();
         }
      }
      for(i32 i = 0; i < countof(page->unlocks) && page->unlocks[i]; i++) {
         if(page->unlocks[i]) {
            P_BIP_Unlock(P_BIP_NameToPage(page->unlocks[i]), from_load);
         }
      }
   }
}

stkoff cstr P_BIP_CategoryToName(i32 category) {
   switch(category) {
      #define bip_category_x(c) case _bipc_##c: return #c;
      bip_category_x(search);
      #include "p_bip.h"
   }
   return nil;
}

stkoff struct page *P_BIP_NameToPage(cstr name) {
   noinit static
   char discrim[32];
   faststrcpy2(discrim, name, pl.discrim);
   for_page() {
      if(faststrchk(page->name, discrim) || faststrchk(page->name, name)) {
         return page;
      }
   }
   PrintErr(_l("couldn't find page "), _p(name), _l(" or "),
            _p((cstr)discrim));
   return nil;
}

script_str ext("ACS") addr(OBJ "BIPUnlock") void Z_UnlockPage(void) {
   noinit static char tag[32];
   faststrcpy_str(tag, EDataS(_edt_bipname));
   P_BIP_Unlock(P_BIP_NameToPage(tag), false);
}

/* EOF */
