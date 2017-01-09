#include "lith_common.h"
#include "lith_player.h"
#include "lith_bip.h"
#include "lith_list.h"

#define Unlocks(...) &(bip_unlocks_t const){__VA_ARGS__}

[[__optional_args(1)]]
void AddToBIP(bip_t *bip, int categ, __str name, bip_unlocks_t const *unlocks)
{
   bippage_t  *page = calloc(1, sizeof(bippage_t));
               page->name     = name;
               page->category = categ;
               page->unlocked = false;
   if(unlocks) memmove(page->unlocks, unlocks, sizeof(*unlocks));
   DList_InsertBack(bip->infogr[categ], (listdata_t){.vp = page});
}

[[__call("ScriptS")]]
void Lith_PlayerInitBIP(player_t *p)
{
   bip_t *bip = &p->bip;
   
   for(int i = BIPC_MIN; i < BIPC_MAX; i++)
      bip->infogr[i] = DList_Create();
   
   AddToBIP(bip, BIPC_WEAPONS, "Pistol",       Unlocks("Omakeda"));
   AddToBIP(bip, BIPC_WEAPONS, "Shotgun",      Unlocks("Omakeda"));
   AddToBIP(bip, BIPC_WEAPONS, "ShotgunUpgr",  Unlocks("Shotgun", "AOF", "DurlaPrime"));
   AddToBIP(bip, BIPC_WEAPONS, "Rifle",        Unlocks("AllPoint"));
   AddToBIP(bip, BIPC_WEAPONS, "RifleUpgr",    Unlocks("Rifle"));
   AddToBIP(bip, BIPC_WEAPONS, "Launcher",     Unlocks("Sym43"));
   AddToBIP(bip, BIPC_WEAPONS, "LauncherUpgr", Unlocks("Launcher", "UnrealArms"));
   AddToBIP(bip, BIPC_WEAPONS, "Plasma",       Unlocks("AllPoint", "MDDO"));
   AddToBIP(bip, BIPC_WEAPONS, "PlasmaUpgr",   Unlocks("Plasma"));
   AddToBIP(bip, BIPC_WEAPONS, "Cannon",       Unlocks("Cid"));
   AddToBIP(bip, BIPC_WEAPONS, "CannonUpgr",   Unlocks("Cannon"));
   
   AddToBIP(bip, BIPC_UPGRADES, "JetBooster", Unlocks("OFMD"));
   AddToBIP(bip, BIPC_UPGRADES, "ReflexWetw", Unlocks("OFMD"));
   AddToBIP(bip, BIPC_UPGRADES, "CyberLegs",  Unlocks("CulterDei"));
   AddToBIP(bip, BIPC_UPGRADES, "Yh0",        Unlocks("DurlaPrime"));
   
   AddToBIP(bip, BIPC_PLACES, "AetosVi");
   AddToBIP(bip, BIPC_PLACES, "DurlaPrime", Unlocks("Earth", "AetosVi"));
   AddToBIP(bip, BIPC_PLACES, "Earth");
   
   AddToBIP(bip, BIPC_CORPORATIONS, "AllPoint");
   AddToBIP(bip, BIPC_CORPORATIONS, "AOF");
   AddToBIP(bip, BIPC_CORPORATIONS, "Cid");
   AddToBIP(bip, BIPC_CORPORATIONS, "CulterDei");
   AddToBIP(bip, BIPC_CORPORATIONS, "MDDO",    Unlocks("OFMD"));
   AddToBIP(bip, BIPC_CORPORATIONS, "OFMD");
   AddToBIP(bip, BIPC_CORPORATIONS, "Omakeda", Unlocks("Earth"));
   AddToBIP(bip, BIPC_CORPORATIONS, "Sym43",   Unlocks("AetosVi"));
   AddToBIP(bip, BIPC_CORPORATIONS, "UnrealArms");
   
   for(int i = BIPC_MIN; i < BIPC_MAX; i++)
      bip->pagemax += bip->categorymax[i] = DList_GetLength(bip->infogr[i]);
   
   Lith_UnlockBIPPage(bip, "Pistol");
}

bippage_t *Lith_FindBIPPage(bip_t *bip, __str name)
{
   for(int i = BIPC_MIN; i < BIPC_MAX; i++)
      for(slist_t *rover = bip->infogr[i]->head; rover; rover = rover->next)
   {
      bippage_t *page = rover->data.vp;
      if(ACS_StrCmp(page->name, name) == 0)
         return page;
   }
   
   return null;
}

bippage_t *Lith_UnlockBIPPage(bip_t *bip, __str name)
{
   bippage_t *page = Lith_FindBIPPage(bip, name);
   if(page && !page->unlocked)
   {
      bip->pageavail++;
      bip->categoryavail[page->category]++;
      page->unlocked = true;
      for(int i = 0; i < MAX_BIP_UNLOCKS && page->unlocks[i]; i++)
         Lith_UnlockBIPPage(bip, page->unlocks[i]);
   }
   return page;
}

void Lith_UnlockAllBIPPages(bip_t *bip)
{
   bip->pageavail = bip->pagemax;
   
   for(int i = BIPC_MIN; i < BIPC_MAX; i++)
      bip->categoryavail[i] = bip->categorymax[i];
   
   for(int i = BIPC_MIN; i < BIPC_MAX; i++)
      for(slist_t *rover = bip->infogr[i]->head; rover; rover = rover->next)
         ((bippage_t *)rover->data.vp)->unlocked = true;
}

[[__call("ScriptS")]]
void Lith_DeallocateBIP(bip_t *bip)
{
   for(int i = BIPC_MIN; i < BIPC_MAX; i++)
   {
      for(slist_t *rover = bip->infogr[i]->head; rover; rover = rover->next)
         free(rover->data.vp);
      
      DList_Free(bip->infogr[i]);
   }
}

// EOF

