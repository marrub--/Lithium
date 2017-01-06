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
   
   for(int i = BIP_CATEGORY_MIN; i < BIP_CATEGORY_MAX; i++)
      bip->infogr[i] = DList_Create();
   
   AddToBIP(bip, BIP_CATEGORY_WEAPONS, "Pistol",       Unlocks("Omakeda"));
   AddToBIP(bip, BIP_CATEGORY_WEAPONS, "Shotgun",      Unlocks("Omakeda"));
   AddToBIP(bip, BIP_CATEGORY_WEAPONS, "ShotgunUpgr",  Unlocks("Omakeda", "AOF", "DurlaPrime"));
   AddToBIP(bip, BIP_CATEGORY_WEAPONS, "Rifle",        Unlocks("AllPoint"));
   AddToBIP(bip, BIP_CATEGORY_WEAPONS, "RifleUpgr",    Unlocks("AllPoint"));
   AddToBIP(bip, BIP_CATEGORY_WEAPONS, "Launcher",     Unlocks("Sym43"));
   AddToBIP(bip, BIP_CATEGORY_WEAPONS, "LauncherUpgr", Unlocks("Sym43", "UnrealArms"));
   AddToBIP(bip, BIP_CATEGORY_WEAPONS, "Plasma",       Unlocks("AllPoint"));
   AddToBIP(bip, BIP_CATEGORY_WEAPONS, "PlasmaUpgr",   Unlocks("AllPoint"));
   AddToBIP(bip, BIP_CATEGORY_WEAPONS, "Cannon",       Unlocks("Cid"));
   AddToBIP(bip, BIP_CATEGORY_WEAPONS, "CannonUpgr",   Unlocks("Cid"));
   
   AddToBIP(bip, BIP_CATEGORY_UPGRADES, "JetBooster");
   AddToBIP(bip, BIP_CATEGORY_UPGRADES, "ReflexWetw");
   AddToBIP(bip, BIP_CATEGORY_UPGRADES, "CyberLegs", Unlocks("CulterDei"));
   AddToBIP(bip, BIP_CATEGORY_UPGRADES, "Yh0",       Unlocks("DurlaPrime"));
   
   AddToBIP(bip, BIP_CATEGORY_PLACES, "DurlaPrime", Unlocks("Earth"));
   AddToBIP(bip, BIP_CATEGORY_PLACES, "Earth");
   
   AddToBIP(bip, BIP_CATEGORY_CORPORATIONS, "AllPoint");
   AddToBIP(bip, BIP_CATEGORY_CORPORATIONS, "AOF");
   AddToBIP(bip, BIP_CATEGORY_CORPORATIONS, "Cid");
   AddToBIP(bip, BIP_CATEGORY_CORPORATIONS, "CulterDei");
   AddToBIP(bip, BIP_CATEGORY_CORPORATIONS, "Omakeda", Unlocks("Earth"));
   AddToBIP(bip, BIP_CATEGORY_CORPORATIONS, "Sym43");
   AddToBIP(bip, BIP_CATEGORY_CORPORATIONS, "UnrealArms");
   
   for(int i = BIP_CATEGORY_MIN; i < BIP_CATEGORY_MAX; i++)
      bip->pagemax += bip->categorymax[i] = DList_GetLength(bip->infogr[i]);
   
   Lith_UnlockBIPPage(bip, "Pistol");
}

bippage_t *Lith_FindBIPPage(bip_t *bip, __str name)
{
   for(int i = BIP_CATEGORY_MIN; i < BIP_CATEGORY_MAX; i++)
      for(slist_t *rover = bip->infogr[i]->head; rover; rover = rover->next)
         if(ACS_StrCmp(((bippage_t *)rover->data.vp)->name, name) == 0)
            return rover->data.vp;
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

// EOF

