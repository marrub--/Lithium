/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
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
#include "m_trie.h"

noinit
struct bip bip;

noinit
char       bipstring[0x7FFF];
mem_size_t bipstrptr;

noinit
struct page bippages[512];
mem_size_t  bippagenum;

dynam_aut script static
void MailNotify(cstr name) {
   pl.setActivator();

   bip.mailreceived++;

   #ifndef NDEBUG
   if(dbgflags(dbgf_bip)) return;
   #endif

   ACS_Delay(20);

   char remote[128];
   faststrcpy_str(remote, ns(lang_fmt(LANG "INFO_REMOT_%s", name)));

   pl.logB(1, tmpstr(lang(sl_log_mailrecv)), remote);

   if(ACS_Random(1, 10000) == 1) {
      bip.mailtrulyreceived++;
      AmbientSound(ss_player_YOUVEGOTMAIL, 1.0);
   } else {
      AmbientSound(ss_player_cbi_mail, 1.0);
   }
}

script
cstr BipStr(cstr in) {
   cstr       out = &bipstring[bipstrptr];
   mem_size_t len = faststrlen(in) + 1;
   fastmemcpy(out, in, len);
   bipstrptr += len;
   return out;
}

script static
void BipInfo_Page(struct tokbuf *tb, struct tbuf_err *res, struct page const *template) {
   noinit static
   char k[64], v[64];

   cstr st;

   struct page *page = &bippages[bippagenum++];
   fastmemcpy(page, template, sizeof *page);

   while(tb->kv(res, k, v)) {
      unwrap(res);
      switch(BipInfo_Page_Name(k)) {
      case _bipinfo_page_cl: {
         i32 pcl = P_ClassToInt(v);
         if(pcl != -1) {
            page->pclass = pcl;
         } else {
            tb->err(res, "%s BipInfo_Page: invalid pclass %s",
                    TokPrint(tb->reget()), v);
            unwrap_cb();
         }
         break;
      }
      case _bipinfo_page_tag:
         page->name = BipStr(v);
         break;
      case _bipinfo_page_unl: {
         i32 i = 0;
         for(char *next = nil,
                  *word = faststrtok(v, &next, ' ');
             word;
             word = faststrtok(nil, &next, ' ')
         ) {
            page->unlocks[i++] = BipStr(word);
         }
         break;
      }
      default:
         tb->err(res, "%s BipInfo_Page: invalid key %s; expected "
                 "cl, "
                 "tag, "
                 "or unl",
                 TokPrint(tb->reget()), k);
         unwrap_cb();
      }
   }
   unwrap(res);
   return;
}

static
struct page BipInfo_Template(struct tokbuf *tb, struct tbuf_err *res) {
   struct token *tok = tb->reget();
   struct page template = {};

   for(char *next = nil,
            *word = faststrtok(tok->textV, &next, ' ');
       word;
       word = faststrtok(nil, &next, ' ')
   ) {
      switch(BipInfo_Template_Name(word)) {
      #define bip_category_x(name) \
      case _bipinfo_template_##name: \
         template.category = _bipc_##name; \
         break;
      #include "p_bip.h"
      case _bipinfo_template_auto:
         set_bit(template.flags, _page_auto);
         break;
      default:
         /* TODO: make a function for generating proper lists */
         tb->err(res, "%s BipInfo_Template: invalid word %s; expected "
                 #define bip_category_x(name) #name ", "
                 #include "p_bip.h"
                 "auto, "
                 "or `\"'",
                 TokPrint(tok), word);
         unwrap_cb();
      }
   }
   return template;
}

script static
void BipInfo_Pages(struct tokbuf *tb, struct tbuf_err *res) {
   struct page const template = BipInfo_Template(tb, res);
   unwrap(res);

   tb->expdr(res, tok_braceo);
   unwrap(res);

   while(!tb->drop(tok_bracec)) {
      BipInfo_Page(tb, res, &template);
      unwrap(res);
   }
}

script static
void BipInfo_Compile(struct tokbuf *tb, struct tbuf_err *res) {
   for(;;) {
      struct token *tok = tb->expc(res, tb->get(), tok_string, tok_eof, 0);
      unwrap(res);

      if(tok->type == tok_eof) {
         return;
      } else {
         BipInfo_Pages(tb, res);
         unwrap(res);
      }
   }
}

script static
void P_BIP_InitInfo(void) {
   Dbg_Log(log_dev, "P_BIP_InitInfo");

   FILE *fp;
   i32 prev = 0;

   bippagenum = 0;

   while((fp = W_OpenIter(sp_LITHINFO, 't', &prev))) {
      struct tokbuf tb;
      TBufCtor(&tb, fp, "LITHINFO");

      struct tbuf_err res = {};
      BipInfo_Compile(&tb, &res);
      unwrap_print(&res);

      TBufDtor(&tb);
      fclose(fp);
   }
}

script
void P_BIP_PInit(void) {
   if(!bippagenum) P_BIP_InitInfo();

   #ifndef NDEBUG
   if(dbglevel_any()) pl.logH(1, "There are %u info pages!", bippagenum);
   #endif

   bip.pagemax = 0;

   for_category() {
      bip.categorymax[categ] = 0;
   }

   for_page() {
      bool avail = page->pclass & pl.pclass;

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

   bip.init = true;
}

script
void P_BIP_Unlock(struct page *page, bool from_load) {
   if(!page) {
      Dbg_Log(log_bip, "ERROR page was null");
      return;
   }

   if(!get_bit(page->flags, _page_available)) {
      Dbg_Log(log_bip, "ERROR page '%s' not available", page->name);
      return;
   }

   if(!get_bit(page->flags, _page_unlocked)) {
      Dbg_Log(log_bip, "unlocking page '%s'", page->name);

      bip.pageavail++;
      bip.categoryavail[page->category]++;

      if(page->category == _bipc_mail && !get_bit(page->flags, _page_auto)) {
         MailNotify(page->name);
      }

      page->time = ticks;
      set_bit(page->flags, _page_unlocked);

      if(!from_load && page->category <= _bipc_last_normal) {
         set_bit(page->flags, _page_new);
         if(!get_bit(page->flags, _page_auto) && ticks > 3) {
            P_Data_Save();
         }
      }

      for(i32 i = 0; i < countof(page->unlocks) && page->unlocks[i]; i++) {
         P_BIP_Unlock(P_BIP_NameToPage(page->unlocks[i]), from_load);
      }
   } else {
      Dbg_Log(log_bip, "already unlocked page '%s'", page->name);
   }
}

alloc_aut(0) stkcall
void P_BIP_PQuit(void) {
   bip.init = false;
}

alloc_aut(0) stkcall
cstr P_BIP_CategoryToName(i32 category) {
   switch(category) {
      #define bip_category_x(c) case _bipc_##c: return #c;
      bip_category_x(search);
      #include "p_bip.h"
   }
   return nil;
}

script static
void BipNameErr(cstr name, cstr discrim) {
   Dbg_Log(log_bip, "ERROR couldn't find page %s or %s");
}

alloc_aut(0) stkcall
struct page *P_BIP_NameToPage(cstr name) {
   noinit static
   char discrim[32];
   faststrcpy2(discrim, name, pl.discrim);
   for_page() {
      if(faststrchk(page->name, discrim) ||
         faststrchk(page->name, name)) {
         return page;
      }
   }
   BipNameErr(name, discrim);
   return nil;
}

script_str ext("ACS") addr(OBJ "BIPUnlock")
void Sc_UnlockPage(void) {
   if(!P_None()) {
      noinit static
      char tag[32];
      faststrcpy_str(tag, ServCallS(sm_GetBipName));
      P_BIP_Unlock(P_BIP_NameToPage(tag), false);
   }
}

/* EOF */
