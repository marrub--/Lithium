#include "lith_common.h"
#include "lith_player.h"
#include "lith_bip.h"
#include "lith_list.h"
#include "lith_world.h"

#define Unlocks(...) &(bip_unlocks_t const){__VA_ARGS__}

#define ForCategory() for(int categ = BIPC_MIN; categ < BIPC_MAX; categ++)
#define ForPage() Lith_ForList(bippage_t *page, bip->infogr[categ])
#define ForCategoryAndPage() ForCategory() ForPage()


//----------------------------------------------------------------------------
// Static Functions
//

//
// UnlockPage
//
static void UnlockPage(bip_t *bip, bippage_t *page)
{
   bip->pageavail++;
   bip->categoryavail[page->category]++;
   page->unlocked = true;
   
   for(int i = 0; i < MAX_BIP_UNLOCKS && page->unlocks[i]; i++)
      bip->unlock(page->unlocks[i]);
}

//
// AddToBIP
//
[[__optional_args(1)]]
static void AddToBIP(bip_t *bip, int categ, __str name, bip_unlocks_t const *unlocks)
{
   __str image = LanguageNull("LITH_TXT_INFO_IMAGE_%S", name);
   
   bippage_t  *page = calloc(1, sizeof(bippage_t));
               page->name     = name;
               page->category = categ;
               page->unlocked = false;
   if(image)   page->image    = image;
   if(unlocks) memmove(page->unlocks, unlocks, sizeof(*unlocks));
   page->link.construct(page);
   page->link.link(&bip->infogr[categ]);
   
   if(categ == BIPC_ENEMIES) UnlockPage(bip, page);
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
      bip->infogr[categ].free(free);
   
   // This could be done a lot better with an array or something, but fuck it.
   // edit: I now am glad that I didn't make this an array or something.
   
   AddToBIP(bip, BIPC_WEAPONS, "ChargeFist", Unlocks("KSKK"));
   
   switch(p->pclass)
   {
   case pclass_marine:
      AddToBIP(bip, BIPC_WEAPONS, "Pistol",          Unlocks("Omakeda"));
      AddToBIP(bip, BIPC_WEAPONS, "Shotgun",         Unlocks("Omakeda"));
      AddToBIP(bip, BIPC_WEAPONS, "ShotgunUpgr",     Unlocks("Shotgun", "AOF", "DurlaPrime"));
      AddToBIP(bip, BIPC_WEAPONS, "ShotgunUpg2",     Unlocks("Shotgun"));
      AddToBIP(bip, BIPC_WEAPONS, "SuperShotgun",    Unlocks("ChAri"));
      AddToBIP(bip, BIPC_WEAPONS, "CombatRifle",     Unlocks("AllPoint"));
      AddToBIP(bip, BIPC_WEAPONS, "RifleUpgr",       Unlocks("CombatRifle"));
      AddToBIP(bip, BIPC_WEAPONS, "RifleUpg2",       Unlocks("CombatRifle", "Semaphore"));
      AddToBIP(bip, BIPC_WEAPONS, "GrenadeLauncher", Unlocks("Sym43"));
      AddToBIP(bip, BIPC_WEAPONS, "LauncherUpgr",    Unlocks("GrenadeLauncher", "UnrealArms"));
      AddToBIP(bip, BIPC_WEAPONS, "LauncherUpg2",    Unlocks("GrenadeLauncher", "Sym43"));
      AddToBIP(bip, BIPC_WEAPONS, "PlasmaRifle",     Unlocks("AllPoint", "MDDO"));
      AddToBIP(bip, BIPC_WEAPONS, "PlasmaUpgr",      Unlocks("PlasmaRifle"));
      AddToBIP(bip, BIPC_WEAPONS, "PlasmaUpg2",      Unlocks("PlasmaRifle", "Semaphore"));
      AddToBIP(bip, BIPC_WEAPONS, "BFG9000",         Unlocks("Cid"));
      AddToBIP(bip, BIPC_WEAPONS, "CannonUpgr",      Unlocks("BFG9000", "SuperDimension"));
      AddToBIP(bip, BIPC_WEAPONS, "CannonUpg2",      Unlocks("BFG9000"));
      break;
   case pclass_cybermage:
      AddToBIP(bip, BIPC_WEAPONS, "Mateba",        Unlocks("AOF2"));
      AddToBIP(bip, BIPC_WEAPONS, "ShockRifle");
      AddToBIP(bip, BIPC_WEAPONS, "SPAS",          Unlocks("AOF2", "Newvec"));
      AddToBIP(bip, BIPC_WEAPONS, "SMG",           Unlocks("Omakeda", "Sym43"));
      AddToBIP(bip, BIPC_WEAPONS, "IonRifle",      Unlocks("KSKK"));
      AddToBIP(bip, BIPC_WEAPONS, "CPlasmaRifle",  Unlocks("AllPoint", "MDDO"));
      AddToBIP(bip, BIPC_WEAPONS, "StarDestroyer", Unlocks("Hell"));
      
      AddToBIP(bip, BIPC_WEAPONS, "Delear");
      AddToBIP(bip, BIPC_WEAPONS, "Hulgyon");
      AddToBIP(bip, BIPC_WEAPONS, "StarShot");
      break;
   }
   
   AddToBIP(bip, BIPC_WEAPONS, "Revolver",        Unlocks("Earth"));
   AddToBIP(bip, BIPC_WEAPONS, "LazShotgun",      Unlocks("Earth"));
   AddToBIP(bip, BIPC_WEAPONS, "SniperRifle",     Unlocks("Facer"));
   AddToBIP(bip, BIPC_WEAPONS, "MissileLauncher");
   AddToBIP(bip, BIPC_WEAPONS, "PlasmaDiffuser",  Unlocks("Sym43", "MDDO", "Semaphore"));
   
   AddToBIP(bip, BIPC_ENEMIES, "ZombieMan");
   AddToBIP(bip, BIPC_ENEMIES, "ShotgunGuy");
   AddToBIP(bip, BIPC_ENEMIES, "ChaingunGuy");
   
   AddToBIP(bip, BIPC_ENEMIES, "Imp");
   AddToBIP(bip, BIPC_ENEMIES, "Demon");
   AddToBIP(bip, BIPC_ENEMIES, "Spectre");
   
   AddToBIP(bip, BIPC_ENEMIES, "LostSoul");
   AddToBIP(bip, BIPC_ENEMIES, "Mancubus");
   AddToBIP(bip, BIPC_ENEMIES, "Arachnotron");
   AddToBIP(bip, BIPC_ENEMIES, "Cacodemon");
   
   AddToBIP(bip, BIPC_ENEMIES, "HellKnight");
   AddToBIP(bip, BIPC_ENEMIES, "BaronOfHell");
   AddToBIP(bip, BIPC_ENEMIES, "Revenant");
   
   AddToBIP(bip, BIPC_ENEMIES, "PainElemental");
   AddToBIP(bip, BIPC_ENEMIES, "Archvile");
   AddToBIP(bip, BIPC_ENEMIES, "SpiderMastermind");
   AddToBIP(bip, BIPC_ENEMIES, "Cyberdemon");
   
   AddToBIP(bip, BIPC_ENEMIES, "Phantom");
   AddToBIP(bip, BIPC_ENEMIES, "IconOfSin");
   
   switch(p->pclass)
   {
   case pclass_marine:
      AddToBIP(bip, BIPC_UPGRADES, "HeadsUpDisp", Unlocks("OFMD"));
      AddToBIP(bip, BIPC_UPGRADES, "JetBooster",  Unlocks("OFMD"));
      AddToBIP(bip, BIPC_UPGRADES, "ReflexWetw",  Unlocks("OFMD"));
      AddToBIP(bip, BIPC_UPGRADES, "CyberLegs",   Unlocks("OFMD"));
      AddToBIP(bip, BIPC_UPGRADES, "Yh0",         Unlocks("DurlaPrime"));
      AddToBIP(bip, BIPC_UPGRADES, "DefenseNuke", Unlocks("OFMD"));
      AddToBIP(bip, BIPC_UPGRADES, "Adrenaline",  Unlocks("KSKK"));
      break;
   case pclass_cybermage:
      AddToBIP(bip, BIPC_UPGRADES, "HeadsUpDispJem");
      AddToBIP(bip, BIPC_UPGRADES, "ReflexWetwJem");
      break;
   }
   
   AddToBIP(bip, BIPC_UPGRADES, "VitalScanner", Unlocks("KSKK"));
   AddToBIP(bip, BIPC_UPGRADES, "AutoReload",   Unlocks("KSKK"));
   
   AddToBIP(bip, BIPC_PLACES, "AetosVi");
   AddToBIP(bip, BIPC_PLACES, "ChAri");
   AddToBIP(bip, BIPC_PLACES, "DurlaPrime",     Unlocks("Earth", "AetosVi"));
   AddToBIP(bip, BIPC_PLACES, "Earth");
   AddToBIP(bip, BIPC_PLACES, "Hell");
   AddToBIP(bip, BIPC_PLACES, "Mars");
   AddToBIP(bip, BIPC_PLACES, "OmicronXevv");
   AddToBIP(bip, BIPC_PLACES, "SuperDimension", Unlocks("BFG9000", "SIGFPE"));
   
   AddToBIP(bip, BIPC_CORPORATIONS, "AllPoint");
   
   if(p->pclass == pclass_cybermage)
      AddToBIP(bip, BIPC_CORPORATIONS, "AOF2");
   else
      AddToBIP(bip, BIPC_CORPORATIONS, "AOF");
   
   AddToBIP(bip, BIPC_CORPORATIONS, "Cid",        Unlocks("SuperDimension", "Earth"));
   AddToBIP(bip, BIPC_CORPORATIONS, "Facer");
   AddToBIP(bip, BIPC_CORPORATIONS, "KSKK",       Unlocks("Earth"));
   AddToBIP(bip, BIPC_CORPORATIONS, "MDDO",       Unlocks("Mars", "OFMD"));
   AddToBIP(bip, BIPC_CORPORATIONS, "Newvec",     Unlocks("Earth"));
   AddToBIP(bip, BIPC_CORPORATIONS, "OFMD");
   AddToBIP(bip, BIPC_CORPORATIONS, "Omakeda",    Unlocks("Earth"));
   AddToBIP(bip, BIPC_CORPORATIONS, "Semaphore",  Unlocks("OmicronXevv"));
   AddToBIP(bip, BIPC_CORPORATIONS, "Sym43",      Unlocks("AetosVi"));
   AddToBIP(bip, BIPC_CORPORATIONS, "UnrealArms", Unlocks("AetosVi"));
   
   ForCategory()
      bip->pagemax += bip->categorymax[categ] = bip->infogr[categ].size;
   
   if(ACS_GetCVar("__lith_debug_on"))
   {
      bip->pageavail = bip->pagemax;
      
      ForCategory()        bip->categoryavail[categ] = bip->categorymax[categ];
      ForCategoryAndPage() page->unlocked = true;
   }
}

//
// Lith_DeliverMail
//
void Lith_DeliverMail(player_t *p, __str title, int flags)
{
   int flag = strtoi_str(Language("LITH_TXT_MAIL_FLAG_%S", title), null, 0);
   flag |= flags;
   
   if(!(flags & MAILF_AllPlayers) && p->pclass == pclass_cybermage)
      title = StrParam("%SJem", title);
   
   bip_t *bip = &p->bip;
   
   bippage_t *page = calloc(1, sizeof(bippage_t));
   
   __str date = LanguageNull("LITH_TXT_MAIL_TIME_%S", title);
   __str size = LanguageNull("LITH_TXT_MAIL_SIZE_%S", title);
   __str send = LanguageNull("LITH_TXT_MAIL_SEND_%S", title);
   __str name = LanguageNull("LITH_TXT_MAIL_NAME_%S", title);
   __str body = Language    ("LITH_TXT_MAIL_BODY_%S", title);
   
   if(!send) send = "<internal>";
   
   page->name  = date ? date : world.canontimeshort;
   page->title = name ? name : "<title omitted>";
   page->body  = StrParam(Language("LITH_TXT_MAIL_TEMPLATE"), send, page->name, body);
   page->category = BIPC_MAIL;
   page->unlocked = true;
   
   if(size) page->height = strtoi_str(size, null, 0);
   
   page->link.construct(page);
   page->link.link(&bip->infogr[BIPC_MAIL]);
   
   bip->mailreceived++;
   
   if(flag & MAILF_PrintMessage)
   {
      p->log("> Mail received from <\Cj%S\C->.", send);
      
      if(ACS_Random(1, 10000) == 1)
      {
         bip->mailtrulyreceived++;
         ACS_LocalAmbientSound("player/YOUVEGOTMAIL", 127);
      }
   }
}

//
// Lith_FindBIPPage
//
bippage_t *Lith_FindBIPPage(bip_t *bip, __str name)
{
   if(!name)
      return null;
   
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
   bippage_t *page = bip->find(name);
   
   if(page && !page->unlocked)
      UnlockPage(bip, page);
   
   return page;
}

//
// Lith_DeallocateBIP
//
[[__call("ScriptS")]]
void Lith_DeallocateBIP(bip_t *bip)
{
   ForCategory()
      bip->infogr[categ].free(free);
}

//
// Lith_PlayerLoseBIPPages
//
void Lith_PlayerLoseBIPPages(bip_t *bip)
{
   ForCategory()
   {
      if(categ == BIPC_MAIL) continue;
      
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
      if(Lith_GUI_Button(g, "Statistics", 70, 80 + (BIPC_MAX * 10), .preset = &btnbipmain))
         bip->curcategory = BIPC_STATS;
      
      avail = bip->pageavail;
      max   = bip->pagemax;
   }
   else if(bip->curcategory == BIPC_STATS)
   {
      int n = 0;
      
      #define Stat(name, f, x) \
         HudMessageF("CBIFONT", name); HudMessagePlain(g->hid--, 23.1,  0.1 + 70 + (8 * n), TICSECOND); \
         HudMessageF("CBIFONT", f, x); HudMessagePlain(g->hid--, 300.2, 0.1 + 70 + (8 * n), TICSECOND); \
         n++
      
      HudMessageF("SMALLFNT", "\Cj%S", p->name);
      HudMessagePlain(g->hid--, 20.1, 60.1, TICSECOND);
      Stat("Weapons Found",       "%i",   p->weaponsheld);
      Stat("Health Used",         "%li",  p->healthused);
      Stat("Health Sum",          "%li",  p->healthsum);
      Stat("Score Used",          "%lli", p->scoreused);
      Stat("Score Sum",           "%lli", p->scoresum);
      Stat("Armor Used",          "%li",  p->armorused);
      Stat("Armor Sum",           "%li",  p->armorsum);
      Stat("Secrets Found",       "%i",   world.secretsfound);
      Stat("Units Travelled",     "%i",   p->unitstravelled);
      Stat("Upgrades Owned",      "%i",   p->upgradesowned);
      Stat("Items Bought",        "%i",   p->itemsbought);
      Stat("Mail Received",       "%i",   bip->mailreceived);
      Stat("Seconds Played",      "%li",  p->ticks / 35l);
      Stat("Spurious Explosions", "%i",   p->spuriousexplosions);
      Stat("Brouzouf Gained",     "%i",   p->brouzouf);
      Stat("Mail Truly Received", "%i",   bip->mailtrulyreceived);
      Stat("Score Multiplier",    "%i%%", ceilk(p->scoremul * 100.0));
   // Stat("Rituals Performed",   "%i",   0);
      
      #undef Stat
   }
   else
   {
      list_t *list = &bip->infogr[bip->curcategory];
      size_t n = list->size;
      size_t i = 0;
      
      Lith_GUI_ScrollBegin(g, st_bipscr, 15, 50, btnlist.w, 170, btnlist.h * n);
      
      Lith_ForListIter(bippage_t *page, *list, i++)
      {
         int y = btnlist.h * i;
         
         if(Lith_GUI_ScrollOcclude(g, st_bipscr, y, btnlist.h))
            continue;
         
         __str name;
         
         if(page->category == BIPC_MAIL) name = page->name;
         else                            name = Language("LITH_TXT_INFO_SHORT_%S", page->name);
         
         name = StrParam("%S%S", bip->curpagenum == i ? "\Ci" : "", name);
         
         if(Lith_GUI_Button_Id(g, i, name, 0, y, !page->unlocked || bip->curpagenum == i, .preset = &btnlist))
         {
            bip->curpagenum = i;
            bip->curpage    = page;
            
            __str body;
            
            if(page->body) body = page->body;
            else           body = Language("LITH_TXT_INFO_DESCR_%S", page->name);
            
            Lith_GUI_TypeOn(g, st_biptypeon, body);
            
            Lith_GUI_ScrollReset(g, st_bipinfoscr);
         }
      }
      
      Lith_GUI_ScrollEnd(g, st_bipscr);
      
      if(bip->curpage)
      {
         bippage_t *page = bip->curpage;
         
         gui_typeon_state_t const *typeon = Lith_GUI_TypeOnUpdate(g, st_biptypeon);
         
         int oy = 0;
         
         if(page->height)
         {
            Lith_GUI_ScrollBegin(g, st_bipinfoscr, 100, 40, 184, 180, page->height);
            oy = g->oy - 40;
         }
         else
            ACS_SetHudClipRect(111, 40, 184, 180, 184);
         
         if(page->image)
            DrawSpriteAlpha(page->image, g->hid--, 296.2, 180.2, TICSECOND, 0.4);
         
         DrawSpriteAlpha("lgfx/UI/Background.png", g->hid--, 0.1, 0.1, TICSECOND, 0.5);
         
         __str title;
         
         if(page->title) title = page->title;
         else            title = Language("LITH_TXT_INFO_TITLE_%S", page->name);
         
         HudMessageF("CBIFONT", "\Ci%S", title);
         HudMessagePlain(g->hid--, 200.4, 45.1 + oy, TICSECOND);
         
         #define DrawText(txt, pos, ...) \
            HudMessageF("CBIFONT", "%.*S", pos, txt), \
            HudMessageParams(0, g->hid--, __VA_ARGS__ + oy, TICSECOND)
         
         // render an outline if the page has an image
         if(page->image)
         {
            __str s = Lith_RemoveTextColors(typeon->txt, typeon->pos);
            int len = ACS_StrLen(s);
            
            DrawText(s, len, CR_BLACK, 112.1, 61.1); DrawText(s, len, CR_BLACK, 110.1, 61.1);
            DrawText(s, len, CR_BLACK, 112.1, 59.1); DrawText(s, len, CR_BLACK, 110.1, 59.1);
            
            DrawText(s, len, CR_BLACK, 111.1, 59.1);
            DrawText(s, len, CR_BLACK, 111.1, 61.1);
            
            DrawText(s, len, CR_BLACK, 112.1, 60.1);
            DrawText(s, len, CR_BLACK, 110.1, 60.1);
         }
         
         DrawText(typeon->txt, typeon->pos, CR_WHITE, 111.1, 60.1);
         
         if(page->height) Lith_GUI_ScrollEnd(g, st_bipinfoscr);
         else             ACS_SetHudClipRect(0, 0, 0, 0);
      }
      
      avail = bip->categoryavail[bip->curcategory];
      max   = bip->categorymax[bip->curcategory];
   }
   
   if(bip->curcategory != BIPC_MAIN)
      if(Lith_GUI_Button(g, "<BACK", 20, 38, false, .preset = &btnbipback))
         bip->curcategory = BIPC_MAIN;
   
   DrawSpriteAlpha("lgfx/UI/bip.png", g->hid--, 20.1, 30.1, TICSECOND, 0.6);
   HudMessageF("CBIFONT", "BIOTIC INFORMATION PANEL ver2.5");
   HudMessagePlain(g->hid--, 35.1, 30.1, TICSECOND);
   
   if(max)
   {
      HudMessageF("CBIFONT", "%i/%i AVAILABLE", avail, max);
      HudMessagePlain(g->hid--, 300.2, 30.1, TICSECOND);
   }
}

// EOF

