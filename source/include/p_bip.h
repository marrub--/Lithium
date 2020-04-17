/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Info page data.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef p_bip_h
#define p_bip_h

#include "m_list.h"
#include "p_bipname.h"

#define for_category() \
   for(i32 categ = 0; categ < BIPC_MAX; categ++)

#define for_page() \
   for(i32 pagen = 0; pagen < BIP_MAX; pagen++) \
      __with(struct page *page = &p->bip.pages[pagen];)

/* Types ------------------------------------------------------------------- */

enum {
   _page_available = 30,
   _page_unlocked,

   _page_time = 0x3fffffff,
};

typedef char bip_name_t[20];

struct pageinfo {
   bip_name_t name;
   i32        category;
   i32        pclass;
   bool       aut;
   bip_name_t unlocks[5];
};

struct page {
   struct pageinfo const *info;

   u32 flags;
};

struct bip {
   /* Stats */
   u32 categoryavail[BIPC_MAX];
   u32 categorymax[BIPC_MAX];

   u32 pageavail;
   u32 pagemax;

   u32 mailreceived;
   u32 mailtrulyreceived;

   /* State */
   bool init;

   struct page *curpage;
   u32 curcategory;
   u32 lastcategory;

   struct page *result[8];
   u32 resnum;
   u32 rescur;

   struct page pages[BIP_MAX];
};

/* Extern Objects ---------------------------------------------------------- */

extern struct pageinfo bipinfo[BIP_MAX];

/* Extern Functions -------------------------------------------------------- */

script void P_BIP_PInit(struct player *p);
void P_BIP_Unlock(struct player *p, cstr name);
void P_BIP_PQuit(struct player *p);

cstr P_BIP_CategoryToName(u32 category);

#endif
