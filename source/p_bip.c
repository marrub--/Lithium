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

/* Static Functions -------------------------------------------------------- */

script static void MailNotify(struct player *p, cstr name) {
   p->setActivator();

   p->bip.mailreceived++;

   if(get_bit(dbgflag, dbgf_bip)) return;

   ACS_Delay(20);

   char remote[64];
   strcpy(remote, LanguageC(LANG "INFO_REMOT_%s", name));

   p->logB(1, LC(LANG "LOG_MailRecv"), remote);

   if(ACS_Random(1, 10000) == 1) {
      p->bip.mailtrulyreceived++;
      ACS_LocalAmbientSound(ss_player_YOUVEGOTMAIL, 127);
   } else {
      ACS_LocalAmbientSound(ss_player_cbi_mail, 127);
   }
}

script static void UnlockPage(struct player *p, struct page *page) {
   if(!get_bit(page->flags, _page_available)) {
      Dbg_Log(log_bip, "page '%s' not available", page->info->name);
      return;
   }

   if(!get_bit(page->flags, _page_unlocked)) {
      Dbg_Log(log_bip, "unlocking page '%s'", page->info->name);

      if(page->info->category == BIPC_MAIL && !page->info->aut) {
         MailNotify(p, page->info->name);
      }

      page->flags |= ticks;
      set_bit(page->flags, _page_unlocked);

      p->bip.pageavail++;
      p->bip.categoryavail[page->info->category]++;

      for(i32 i = 0;
          i < countof(page->info->unlocks) && page->info->unlocks[i][0];
          i++) {
         P_BIP_Unlock(p, page->info->unlocks[i]);
      }
   } else {
      Dbg_Log(log_bip, "already unlocked page '%s'", page->info->name);
   }
}

static u32 NameToNum(cstr discrim, cstr name) {
   for(u32 i = 0; i < BIP_MAX; i++) {
      bip_name_t tag;
      lstrcpy2(tag, name, discrim);
      if(faststrcmp(bipinfo[i].name, tag)  == 0 ||
         faststrcmp(bipinfo[i].name, name) == 0) {
         return i;
      }
   }

   return BIP_MAX;
}

/* Extern Functions -------------------------------------------------------- */

script void P_BIP_PInit(struct player *p) {
   if(dbglevel) p->logH(1, "There are %u info pages!", BIP_MAX);

   p->bip.pagemax = 0;

   for_category() {
      p->bip.categorymax[categ] = 0;
   }

   for_page() {
      page->info  = &bipinfo[pagen];
      page->flags = 0;

      bool avail = page->info->pclass & p->pclass;

      if(avail) {
         set_bit(page->flags, _page_available);

         p->bip.pagemax++;
         p->bip.categorymax[page->info->category]++;

         if(get_bit(dbgflag, dbgf_bip) || page->info->aut) {
            UnlockPage(p, page);
         }
      }
   }

   p->bip.init = true;
}

void P_BIP_Unlock(struct player *p, cstr name) {
   u32 num = NameToNum(p->discrim, name);

   if(num == BIP_MAX) {
      Dbg_Log(log_bip, "no page '%s' found", name);
      return;
   }

   UnlockPage(p, &p->bip.pages[num]);
}

void P_BIP_PQuit(struct player *p) {
   p->bip.init = false;
}

cstr P_BIP_CategoryToName(u32 category) {
   switch(category) {
      #define Categ(name) case BIPC_##name: return #name;
      Categ(SEARCH);
      #include "p_bipname.h"
   }
   return nil;
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "BIPUnlock")
void Sc_UnlockPage(void) {
   with_player(LocalPlayer) {
      bip_name_t tag; lstrcpy_str(tag, ServCallS(sm_GetBipName));
      P_BIP_Unlock(p, tag);
   }
}

/* EOF */
