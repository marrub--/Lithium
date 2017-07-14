#if defined(LITH_X)
  LITH_X(WEAPONS,      "Weapons")
  LITH_X(ENEMIES,      "Adversaries")
// TODO: Add when things like the Division Sigil are in
//LITH_X(ITEMS,        "Items")
  LITH_X(UPGRADES,     "Upgrades")
  LITH_X(PLACES,       "Places")
  LITH_X(CORPORATIONS, "Companies and Corporations")
  LITH_X(MAIL,         "Mail")
#undef LITH_X

#elif !defined(LITH_BIP_H)
#define LITH_BIP_H

#include "lith_list.h"


//----------------------------------------------------------------------------
// Extern Functions
//

[[__call("ScriptS")]] void Lith_PlayerInitBIP(struct player *p);
struct bippage_s *Lith_FindBIPPage(struct bip_s *bip, __str name);
struct bippage_s *Lith_UnlockBIPPage(struct bip_s *bip, __str name);
[[__call("ScriptS")]] void Lith_DeallocateBIP(struct bip_s *bip);
void Lith_PlayerLoseBIPPages(struct bip_s *bip);
[[__optional_args(1)]] void Lith_DeliverMail(struct player *p, __str title, int flags);


//----------------------------------------------------------------------------
// Types
//

enum
{
#define LITH_X(name, capt) BIPC_##name,
#include "lith_bip.h"
   BIPC_EXTRA,
   BIPC_MAX,
   BIPC_MAIN,
   BIPC_STATS,
   BIPC_SEARCH
};

enum
{
   MAILF_PrintMessage = 1 << 0,
   MAILF_AllPlayers   = 1 << 1,
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

typedef struct bip_s
{
   property find       {call: Lith_FindBIPPage(this)}
   property unlock     {call: Lith_UnlockBIPPage(this)}
   property deallocate {call: Lith_DeallocateBIP(this)}
   property losePages  {call: Lith_PlayerLoseBIPPages(this)}
   
   // Stats
   int categoryavail[BIPC_MAX];
   int categorymax[BIPC_MAX];
   int pageavail;
   int pagemax;
   int mailreceived, mailtrulyreceived;
   
   // State
   bippage_t *curpage;
   int curpagenum;
   int curcategory;
   int lastcategory;
   int scroll;
   bippage_t *result[8];
   int resnum;
   int rescur;
   
   // Info
   list_t infogr[BIPC_MAX];
} bip_t;

#endif

