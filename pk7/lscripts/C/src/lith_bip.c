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
               page->name     = name;
               page->category = categ;
               page->unlocked = false;
   if(unlocks) memmove(page->unlocks, unlocks, sizeof(*unlocks));
   DList_InsertBack(bip->infogr[categ], (listdata_t){.vp = page});
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
      bip->pagemax += bip->categorymax[i] = DList_GetLength(bip->infogr[i]);
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
   
   ForCategory()        bip->categoryavail[i] = bip->categorymax[i];
   ForCategoryAndPage() page->unlocked = true;
}

//
// Lith_DeallocateBIP
//
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

//
// Lith_PlayerLoseBIPPages
//
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

//
// CBI_Tab_BIP
//
int CBI_Tab_BIP(player_t *p, int hid, cbi_t *cbi, gui_state_t *gst)
{
   static gui_button_parm_t const btnp = {
      .f_gfx_def = "",
      .f_gfx_hot = "",
      .dim_x = 180,
      .dim_y = 9
   };
   
   static gui_button_parm_t const backbtnp = {
      .f_gfx_def = "",
      .f_gfx_hot = "",
      .dim_y = 9
   };
   
   bip_t *bip = &p->bip;
   int avail, max;
   
   if(bip->curcategory == BIPC_MAIN)
   {
      HudMessageF("CBIFONT", "\CTINFO");
      HudMessagePlain(hid--, 160.4, 70.1, TICSECOND);
      
#define LITH_X(n, id, name, capt) \
      if(GUI_Button(GUI_ID("b" #id), gst, &hid, 70, 80 + (n * 10), capt, false, &btnp)) \
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
      GUI_BEGIN_LIST(GUI_ID("bSCL"), gst, &hid, 20, 50, 130, &bip->scroll);
      
      dlist_t *l = bip->infogr[bip->curcategory];
      size_t n = DList_GetLength(l);
      size_t i = 0;
      for(slist_t *rover = l->head; rover; rover = rover->next, i++)
      {
         bippage_t *page = rover->data.vp;
         GUI_LIST_OFFSETS(i, n, 130, cbi->gst.scrlst[CBI_SCRLST_UPGRADES]);
         
         __str id   = StrParam("bN%.2i", i);
         __str name = StrParam("%S%S", bip->curpagenum == i ? "\Ci" : "", Language("LITH_TXT_INFO_SHORT_%S", page->name));
         
         if(GUI_Button(GUI_ID(id), gst, &hid, 0, addy, name, !page->unlocked || bip->curpagenum == i, &gui_listbtnparm))
         {
            bip->curpagenum = i;
            bip->curpage    = page;
         }
      }
      
      GUI_END_LIST(gst);
      
      if(bip->curpage)
      {
         ACS_SetHudClipRect(111, 40, 184, 140, 184);
         
         HudMessageF("CBIFONT", "\Cj%S", Language("LITH_TXT_INFO_TITLE_%S", bip->curpage->name));
         HudMessagePlain(hid--, 200.4, 45.1, TICSECOND);
         HudMessageF("CBIFONT", "%S", Language("LITH_TXT_INFO_DESCR_%S", bip->curpage->name));
         HudMessagePlain(hid--, 111.1, 60.1, TICSECOND);
         
         ACS_SetHudClipRect(0, 0, 0, 0);
      }
      
      if(GUI_Button(GUI_ID("bBBT"), gst, &hid, 20, 38, "<BACK", false, &backbtnp))
         bip->curcategory = BIPC_MAIN;
      
      avail = bip->categoryavail[bip->curcategory];
      max   = bip->categorymax[bip->curcategory];
   }
   
   DrawSpriteAlpha("lgfx/UI/bip.png", hid--, 20.1, 30.1, TICSECOND, 0.6);
   HudMessageF("CBIFONT", "BIOTIC INFORMATION PANEL ver2.5");
   HudMessagePlain(hid--, 35.1, 30.1, TICSECOND);
   
   HudMessageF("CBIFONT", "%i/%i AVAILABLE", avail, max);
   HudMessagePlain(hid--, 300.2, 30.1, TICSECOND);
   
   return hid;
}

// EOF

