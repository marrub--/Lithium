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

#if defined(bip_category_x)
bip_category_x(enemies)
bip_category_x(items)
bip_category_x(yourself)
bip_category_x(upgrades)
bip_category_x(places)
bip_category_x(corporations)
bip_category_x(weapons)
bip_category_x(extra)
bip_category_x(mail)
#undef bip_category_x
#elif !defined(p_bip_h)
#define p_bip_h

#include "m_math.h"
#include "m_list.h"

#define for_category() \
   for(i32 categ = 0; categ < _bipc_max; categ++)

#define for_page() \
   for(i32 pagen = 0; pagen < bippagenum; pagen++) \
      __with(struct page *page = &bippages[pagen];)

/* Types ------------------------------------------------------------------- */

enum {
   #define bip_category_x(c) _bipc_##c,
   #include "p_bip.h"
   _bipc_max,
   _bipc_main = _bipc_max,
   _bipc_search,
   _bipc_last_normal = _bipc_weapons,
};

enum {
   _page_available,
   _page_unlocked,
   _page_auto,
   _page_new,
};

struct page {
   cstr name;
   cstr unlocks[5];
   i32  category;
   i32  pclass;

   i32  flags;
   i32  time;
};

struct bip {
   /* Stats */
   i32 categoryavail[_bipc_max];
   i32 categorymax[_bipc_max];

   i32 pageavail;
   i32 pagemax;

   i32 mailreceived;
   i32 mailtrulyreceived;

   /* State */
   bool init;

   struct page *curpage;
   cstr         pagebody;
   i32          pagebodypos;
   mem_size_t   pagebodylen;
   i32          curcategory;
   i32          lastcategory;

   struct page *result[8];
   u32 resnum;
   u32 rescur;
};

/* Extern Objects ---------------------------------------------------------- */

extern struct bip  bip;
extern struct page bippages[];
extern mem_size_t  bippagenum;

/* Extern Functions -------------------------------------------------------- */

script void P_BIP_PInit(void);
stkcall void P_BIP_PQuit(void);

script void P_BIP_Unlock(struct page *page, bool from_load);
stkcall struct page *P_BIP_NameToPage(cstr name);
stkcall cstr P_BIP_CategoryToName(i32 category);

#endif
