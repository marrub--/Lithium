// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
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

// Extern Functions ----------------------------------------------------------|

script void Lith_PlayerInitBIP(struct player *p);
struct bippage_s *Lith_FindBIPPage(struct bip *bip, __str name);
optargs(1) struct bippage_s *Lith_UnlockBIPPage(struct bip *bip, __str name, int pclass);
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

typedef __str bip_unlocks_t[5];

typedef struct bippage_s
{
   __str  name;
   __str  image;
   __str  body;
   __str  title;
   int    height;
   int    category;
   bool   unlocked;
   list_t link;
   bip_unlocks_t unlocks;
} bippage_t;

struct bip
{
   // Attributes -------------------------------------------------------------|

   __prop find       {call: Lith_FindBIPPage  (this)}
   __prop unlock     {call: Lith_UnlockBIPPage(this)}
   __prop deallocate {call: Lith_DeallocateBIP(this)}

   // Members ----------------------------------------------------------------|

   // Stats
   uint categoryavail[BIPC_MAX];
   uint categorymax[BIPC_MAX];

   // State
   bippage_t *curpage;
   bippage_t *result[8];

   // Info
   list_t infogr[BIPC_MAX];

   // Bitfields --------------------------------------------------------------|

   // Stats
   uint pageavail ;//: 10;
   uint pagemax   ;//: 10;
   uint mailreceived      ;//: 6;
   uint mailtrulyreceived ;//: 6;

   // State
   bool init ;//: 1;
   uint curcategory  ;//: 4;
   uint lastcategory ;//: 4;
   uint resnum ;//: 3;
   uint rescur ;//: 3;
};

#endif
