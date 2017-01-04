#if defined(LITH_X)

LITH_X(0, WEP, WEAPONS,      "Weapons")
LITH_X(1, ENE, ENEMIES,      "Enemies")
LITH_X(2, ITE, ITEMS,        "Items")
LITH_X(3, UPG, UPGRADES,     "Upgrades")
LITH_X(4, ABI, ABILITIES,    "Abilities")
LITH_X(5, YOU, YOURSELF,     "Yourself")
LITH_X(6, PLA, PLACES,       "Places")
LITH_X(7, COR, CORPORATIONS, "Companies and Corporations")

#undef LITH_X

#elif !defined(LITH_BIP_H)
#define LITH_BIP_H


// ---------------------------------------------------------------------------
// BIP - Biotic Information Panel
//

#define MAX_BIP_UNLOCKS 5

enum
{
   BIP_CATEGORY_MIN,
#define LITH_X(n, id, name, capt) BIP_CATEGORY_##name = n,
#include "lith_bip.h"
   BIP_CATEGORY_MAX,
   BIP_CATEGORY_MAIN
};

typedef __str bip_unlocks_t[MAX_BIP_UNLOCKS];

typedef struct bippage_s
{
   __str name;
   int   category;
   bool  unlocked;
   bip_unlocks_t unlocks;
} bippage_t;

typedef struct bip_s
{
   int categoryavail[BIP_CATEGORY_MAX];
   int categorymax[BIP_CATEGORY_MAX];
   int pageavail;
   int pagemax;
   bippage_t *curpage;
   int curpagenum;
   int curcategory;
   int scroll;
   struct dlist_s *infogr[BIP_CATEGORY_MAX];
} bip_t;

[[__call("ScriptS")]]
void Lith_PlayerInitBIP(struct player_s *p);

bippage_t *Lith_FindBIPPage(bip_t *bip, __str name);
bippage_t *Lith_UnlockBIPPage(bip_t *bip, __str name);

#endif

