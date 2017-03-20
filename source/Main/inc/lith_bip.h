#if defined(LITH_X)
  LITH_X(0, WEP, WEAPONS,      "Weapons")
  LITH_X(1, ENE, ENEMIES,      "Adversaries")
// Add when things like the Division Sigil are in
//LITH_X( , ITE, ITEMS,        "Items")
  LITH_X(2, UPG, UPGRADES,     "Upgrades")
  LITH_X(3, PLA, PLACES,       "Places")
  LITH_X(4, COR, CORPORATIONS, "Companies and Corporations")
  LITH_X(5, MAI, MAIL,         "Mail")
#undef LITH_X

#elif !defined(LITH_BIP_H)
#define LITH_BIP_H

#include "lith_list.h"

#define MAX_BIP_UNLOCKS 5


//----------------------------------------------------------------------------
// Extern Functions
//

[[__call("ScriptS")]] void Lith_PlayerInitBIP(struct player_s *p);
struct bippage_s *Lith_FindBIPPage(struct bip_s *bip, __str name);
struct bippage_s *Lith_UnlockBIPPage(struct bip_s *bip, __str name);
[[__call("ScriptS")]] void Lith_DeallocateBIP(struct bip_s *bip);
void Lith_PlayerLoseBIPPages(struct bip_s *bip);
void Lith_DeliverMail(struct bip_s *bip, __str title);


//----------------------------------------------------------------------------
// Types
//

enum
{
   BIPC_MIN,
#define LITH_X(n, id, name, capt) BIPC_##name = n,
#include "lith_bip.h"
   BIPC_MAX,
   BIPC_MAIN,
   BIPC_STATS
};

typedef __str bip_unlocks_t[MAX_BIP_UNLOCKS];

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
   property find        {call: Lith_FindBIPPage(this)}
   property unlock      {call: Lith_UnlockBIPPage(this)}
   property deallocate  {call: Lith_DeallocateBIP(this)}
   property losePages   {call: Lith_PlayerLoseBIPPages(this)}
   property deliverMail {call: Lith_DeliverMail(this)}
   
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
   int scroll;
   
   // Info
   list_t infogr[BIPC_MAX];
} bip_t;

#endif

