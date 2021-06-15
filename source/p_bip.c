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

dynam_aut script static
void MailNotify(cstr name) {
   pl.setActivator();

   pl.bip.mailreceived++;

   #ifndef NDEBUG
   if(get_bit(dbgflags, dbgf_bip)) return;
   #endif

   ACS_Delay(20);

   char remote[128];
   faststrcpy(remote, LanguageC(LANG "INFO_REMOT_%s", name));

   pl.logB(1, LC(LANG "LOG_MailRecv"), remote);

   if(ACS_Random(1, 10000) == 1) {
      pl.bip.mailtrulyreceived++;
      ACS_LocalAmbientSound(ss_player_YOUVEGOTMAIL, 127);
   } else {
      ACS_LocalAmbientSound(ss_player_cbi_mail, 127);
   }
}

script static void UnlockPage(struct page *page) {
   if(!get_bit(page->flags, _page_available)) {
      Dbg_Log(log_bip, "ERROR page '%s' not available", page->info->name);
      return;
   }

   if(!get_bit(page->flags, _page_unlocked)) {
      Dbg_Log(log_bip, "unlocking page '%s'", page->info->name);

      if(page->info->category == BIPC_MAIL && !page->info->aut) {
         MailNotify(page->info->name);
      }

      page->flags |= ticks;
      set_bit(page->flags, _page_unlocked);

      pl.bip.pageavail++;
      pl.bip.categoryavail[page->info->category]++;

      for(i32 i = 0;
          i < countof(page->info->unlocks) && page->info->unlocks[i][0];
          i++) {
         P_BIP_Unlock(page->info->unlocks[i]);
      }
   } else {
      Dbg_Log(log_bip, "already unlocked page '%s'", page->info->name);
   }
}

static i32 NameToNum(cstr discrim, cstr name) {
   for(i32 i = 0; i < BIP_MAX; i++) {
      bip_name_t tag;
      faststrcpy2(tag, name, discrim);
      if(faststrcmp(bipinfo[i].name, tag)  == 0 ||
         faststrcmp(bipinfo[i].name, name) == 0) {
         return i;
      }
   }

   return BIP_MAX;
}

/* Extern Functions -------------------------------------------------------- */

script void P_BIP_PInit(void) {
   #ifndef NDEBUG
   if(dbglevel) pl.logH(1, "There are %u info pages!", BIP_MAX);
   #endif

   pl.bip.pagemax = 0;

   for_category() {
      pl.bip.categorymax[categ] = 0;
   }

   for_page() {
      page->info  = &bipinfo[pagen];
      page->flags = 0;
   }

   for_page() {
      bool avail = page->info->pclass & pl.pclass;

      if(avail) {
         set_bit(page->flags, _page_available);

         pl.bip.pagemax++;
         pl.bip.categorymax[page->info->category]++;
      }
   }

   for_page() {
      if(get_bit(page->flags, _page_available) &&
         (
          #ifndef NDEBUG
          get_bit(dbgflags, dbgf_bip)
          #else
          false
          #endif
          || page->info->aut)) {
         UnlockPage(page);
      }
   }

   pl.bip.init = true;
}

void P_BIP_Unlock(cstr name) {
   i32 num = NameToNum(pl.discrim, name);

   if(num == BIP_MAX) {
      Dbg_Log(log_bip, "ERROR no page '%s' found", name);
      return;
   }

   UnlockPage(&pl.bip.pages[num]);
}

void P_BIP_PQuit(void) {
   pl.bip.init = false;
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
   if(!P_None()) {
      bip_name_t tag; faststrcpy_str(tag, ServCallS(sm_GetBipName));
      P_BIP_Unlock(tag);
   }
}

/* EOF */
