#include "lith_common.h"
#include "lith_player.h"
#include "lith_bip.h"
#include "lith_list.h"

#define Unlocks(...) &(bip_unlocks_t const){__VA_ARGS__}

#define ForCategory() \
   for(int categ = BIPC_MIN; categ < BIPC_MAX; categ++)

#define ForPage() \
   for(list_t *rover = bip->infogr[categ].next; rover != &bip->infogr[categ]; rover = rover->next) \
      __with(bippage_t *page = rover->object;)

#define ForCategoryAndPage() \
   ForCategory() \
      ForPage()


//----------------------------------------------------------------------------
// Static Functions
//

//
// AddToBIP
//
[[__optional_args(1)]]
static void AddToBIP(bip_t *bip, int categ, __str name, bip_unlocks_t const *unlocks)
{
   bippage_t  *page = calloc(1, sizeof(bippage_t));
   Lith_LinkDefault(&page->link, page);
               page->name     = name;
               page->category = categ;
               page->unlocked = false;
   if(unlocks) memmove(page->unlocks, unlocks, sizeof(*unlocks));
   Lith_ListLink(&bip->infogr[categ], &page->link);
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_PlayerInitBIP
//
[[__call("ScriptS")]]
void Lith_PlayerInitBIP(player_t *p)
{
   bip_t *bip = &p->bip;
   
   ForCategory()
      Lith_LinkDefault(&bip->infogr[categ]);
   
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
   AddToBIP(bip, BIPC_WEAPONS, "CannonUpg2",   Unlocks("Cannon"));
   
   AddToBIP(bip, BIPC_UPGRADES, "JetBooster",  Unlocks("OFMD"));
   AddToBIP(bip, BIPC_UPGRADES, "ReflexWetw",  Unlocks("OFMD"));
   AddToBIP(bip, BIPC_UPGRADES, "CyberLegs",   Unlocks("OFMD"));
   AddToBIP(bip, BIPC_UPGRADES, "Yh0",         Unlocks("DurlaPrime"));
   AddToBIP(bip, BIPC_UPGRADES, "DefenseNuke");
   AddToBIP(bip, BIPC_UPGRADES, "Adrenaline");
   
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
      bip->pagemax += bip->categorymax[categ] = Lith_ListSize(&bip->infogr[categ]);
}

//
// Lith_FindBIPPage
//
bippage_t *Lith_FindBIPPage(bip_t *bip, __str name)
{
   ForCategoryAndPage()
      if(ACS_StrCmp(page->name, name) == 0)
         return page;
   
   return null;
}

//
// Lith_UnlockBIPPage
//
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

//
// Lith_UnlockAllBIPPages
//
void Lith_UnlockAllBIPPages(bip_t *bip)
{
   bip->pageavail = bip->pagemax;
   
   ForCategory()        bip->categoryavail[categ] = bip->categorymax[categ];
   ForCategoryAndPage() page->unlocked = true;
}

//
// Lith_DeallocateBIP
//
[[__call("ScriptS")]]
void Lith_DeallocateBIP(bip_t *bip)
{
   ForCategory()
      Lith_ListFree(&bip->infogr[categ], free);
}

//
// Lith_PlayerLoseBIPPages
//
void Lith_PlayerLoseBIPPages(bip_t *bip)
{
   ForCategory()
   {
      ForPage()
         page->unlocked = false;
      bip->categoryavail[categ] = 0;
   }
   
   bip->pageavail = 0;
}

//
// Lith_CBITab_BIP
//
void Lith_CBITab_BIP(gui_state_t *g, player_t *p)
{
   bip_t *bip = &p->bip;
   int avail, max;
   
   if(bip->curcategory == BIPC_MAIN)
   {
      HudMessageF("CBIFONT", "\CTINFO");
      HudMessagePlain(g->hid--, 160.4, 70.1, TICSECOND);
      
#define LITH_X(n, id, name, capt) \
      if(Lith_GUI_Button_Id(g, n, capt, 70, 80 + (n * 10), .preset = &btnbipmain)) \
      { \
         bip->curcategory = BIPC_##name; \
         bip->curpagenum  = -1; \
         bip->curpage     = null; \
      }
#include "lith_bip.h"
      
      avail = bip->pageavail;
      max   = bip->pagemax;
   }
   else
   {
      list_t *list = &bip->infogr[bip->curcategory];
      size_t n = Lith_ListSize(list);
      size_t i = 0;
      
      Lith_GUI_ScrollBegin(g, st_bipscr, 15, 50, btnlist.w, 130, btnlist.h * n);
      
      for(list_t *rover = list->next; rover != list; rover = rover->next, i++)
      {
         int y = btnlist.h * i;
         
         if(Lith_GUI_ScrollOcclude(g, st_bipscr, y, btnlist.h))
            continue;
         
         bippage_t *page = rover->object;
         
         __str name =
            StrParam("%S%S", bip->curpagenum == i ? "\Ci" : "", Language("LITH_TXT_INFO_SHORT_%S", page->name));
         
         if(Lith_GUI_Button_Id(g, i, name, 0, y, !page->unlocked || bip->curpagenum == i, .preset = &btnlist))
         {
            bip->curpagenum = i;
            bip->curpage    = page;
         }
      }
      
      Lith_GUI_ScrollEnd(g, st_bipscr);
      
      if(Lith_GUI_Button(g, "<BACK", 20, 38, false, &btnbipback))
         bip->curcategory = BIPC_MAIN;
      
      if(bip->curpage)
      {
         ACS_SetHudClipRect(111, 40, 184, 140, 184);
         
         HudMessageF("CBIFONT", "\Cj%S", Language("LITH_TXT_INFO_TITLE_%S", bip->curpage->name));
         HudMessagePlain(g->hid--, 200.4, 45.1, TICSECOND);
         HudMessageF("CBIFONT", "%S", Language("LITH_TXT_INFO_DESCR_%S", bip->curpage->name));
         HudMessagePlain(g->hid--, 111.1, 60.1, TICSECOND);
         
         ACS_SetHudClipRect(0, 0, 0, 0);
      }
      
      avail = bip->categoryavail[bip->curcategory];
      max   = bip->categorymax[bip->curcategory];
   }
   
   DrawSpriteAlpha("lgfx/UI/bip.png", g->hid--, 20.1, 30.1, TICSECOND, 0.6);
   HudMessageF("CBIFONT", "BIOTIC INFORMATION PANEL ver2.5");
   HudMessagePlain(g->hid--, 35.1, 30.1, TICSECOND);
   
   HudMessageF("CBIFONT", "%i/%i AVAILABLE", avail, max);
   HudMessagePlain(g->hid--, 300.2, 30.1, TICSECOND);
}

// EOF

