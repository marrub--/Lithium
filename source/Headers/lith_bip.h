// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#if defined(LITH_X)
LITH_X(WEAPONS,      "Weapons")
LITH_X(ENEMIES,      "Adversaries")
LITH_X(ITEMS,        "Items")
LITH_X(YOURSELF,     "Yourself")
LITH_X(UPGRADES,     "Upgrades")
LITH_X(PLACES,       "Locations")
LITH_X(CORPORATIONS, "Companies")
LITH_X(MAIL,         "Mail")
#undef LITH_X
#elif !defined(LITH_BIP_H)
#define LITH_BIP_H

#include "lith_list.h"

#define ForCategory() for(int categ = BIPC_NONE + 1; categ < BIPC_MAX; categ++)
#define ForPage() forlist(struct page *page, bip->infogr[categ])
#define ForCategoryAndPage() ForCategory() ForPage()

// Extern Functions ----------------------------------------------------------|

script void Lith_PlayerInitBIP(struct player *p);
struct page *Lith_FindBIPPage(struct bip *bip, char const *name);
optargs(1) struct page *Lith_UnlockBIPPage(struct bip *bip, char const *name, int pclass);
script void Lith_DeallocateBIP(struct bip *bip);
script optargs(1) void Lith_DeliverMail(struct player *p, __str title, int flags);

// Types ---------------------------------------------------------------------|

enum
{
   BIPC_NONE,
#define LITH_X(name, capt) BIPC_##name,
#include "lith_bip.h"
   BIPC_EXTRA,
   BIPC_MAX,
   BIPC_MAIN,
   BIPC_SEARCH
};

enum
{
   MAILF_NoPrint    = 1 << 0,
   MAILF_AllPlayers = 1 << 1,
};

typedef char bip_name_t[20];
typedef bip_name_t bip_unlocks_t[5];

struct page
{
   bip_name_t name;
   __str  image;
   __str  body;
   __str  title;
   int    height;
   int    category;
   bool   unlocked;
   list_t link;
   bip_unlocks_t unlocks;
};

struct page_info
{
   __str shname;
   __str flname;
   __str body;
};

struct bip
{
   __prop find       {call: Lith_FindBIPPage  (this)}
   __prop unlock     {call: Lith_UnlockBIPPage(this)}
   __prop deallocate {call: Lith_DeallocateBIP(this)}

   // Stats
   uint categoryavail[BIPC_MAX];
   uint categorymax[BIPC_MAX];

   uint pageavail;
   uint pagemax;

   uint mailreceived;
   uint mailtrulyreceived;

   // State
   bool init;

   struct page *curpage;
   struct page *result[8];

   uint curcategory;
   uint lastcategory;

   uint resnum;
   uint rescur;

   // Info
   list_t infogr[BIPC_MAX];
};

// Extern Functions ----------------------------------------------------------|

struct page_info Lith_GetPageInfo(struct page const *page);

#endif
