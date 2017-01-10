#include "lith_common.h"
#include "lith_player.h"
#include "lith_bip.h"
#include "lith_list.h"

#define Unlocks(...) &(bip_unlocks_t const){__VA_ARGS__}

#define ForCategory() \
   for(int i = BIPC_MIN; i < BIPC_MAX; i++)

#define ForPage() \
   for(slist_t *rover = bip->infogr[i]->head; rover; rover = rover->next) \
      __with(bippage_t *page = rover->data.vp;)

#define ForCategoryAndPage() \
   ForCategory() \
      ForPage()

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
   
   ForCategory()
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
   AddToBIP(bip, BIPC_WEAPONS, "CannonUpgr",   Unlocks("Cannon", "SuperDimension"));
   
   AddToBIP(bip, BIPC_UPGRADES, "JetBooster", Unlocks("OFMD"));
   AddToBIP(bip, BIPC_UPGRADES, "ReflexWetw", Unlocks("OFMD"));
   AddToBIP(bip, BIPC_UPGRADES, "CyberLegs",  Unlocks("OFMD"));
   AddToBIP(bip, BIPC_UPGRADES, "Yh0",        Unlocks("DurlaPrime"));
   
   AddToBIP(bip, BIPC_PLACES, "AetosVi");
   AddToBIP(bip, BIPC_PLACES, "DurlaPrime",     Unlocks("Earth", "AetosVi"));
   AddToBIP(bip, BIPC_PLACES, "Earth");
   AddToBIP(bip, BIPC_PLACES, "SuperDimension", Unlocks("Cannon"));
   
   AddToBIP(bip, BIPC_CORPORATIONS, "AllPoint");
   AddToBIP(bip, BIPC_CORPORATIONS, "AOF");
   AddToBIP(bip, BIPC_CORPORATIONS, "Cid",        Unlocks("SuperDimension", "Earth"));
   AddToBIP(bip, BIPC_CORPORATIONS, "MDDO",       Unlocks("OFMD"));
   AddToBIP(bip, BIPC_CORPORATIONS, "OFMD");
   AddToBIP(bip, BIPC_CORPORATIONS, "Omakeda",    Unlocks("Earth"));
   AddToBIP(bip, BIPC_CORPORATIONS, "Sym43",      Unlocks("AetosVi"));
   AddToBIP(bip, BIPC_CORPORATIONS, "UnrealArms", Unlocks("AetosVi"));
   
   ForCategory()
      bip->pagemax += bip->categorymax[i] = DList_GetLength(bip->infogr[i]);
}

bippage_t *Lith_FindBIPPage(bip_t *bip, __str name)
{
   ForCategoryAndPage()
      if(ACS_StrCmp(page->name, name) == 0)
         return page;
   
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
   
   ForCategory()        bip->categoryavail[i] = bip->categorymax[i];
   ForCategoryAndPage() page->unlocked = true;
}

[[__call("ScriptS")]]
void Lith_DeallocateBIP(bip_t *bip)
{
   ForCategory()
   {
      ForPage()
         free(rover->data.vp);
      DList_Free(bip->infogr[i]);
   }
}

void Lith_PlayerLoseBIPPages(bip_t *bip)
{
   ForCategory()
   {
      ForPage()
         page->unlocked = false;
      bip->categoryavail[i] = 0;
   }
   
   bip->pageavail = 0;
}

// EOF

