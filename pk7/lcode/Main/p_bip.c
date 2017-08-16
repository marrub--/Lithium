#include "lith_common.h"
#include "lith_player.h"
#include "lith_bip.h"
#include "lith_list.h"
#include "lith_world.h"

#include <ctype.h>

#define ForCategory() for(int categ = 0; categ < BIPC_MAX; categ++)
#define ForPage() Lith_ForList(bippage_t *page, bip->infogr[categ])
#define ForCategoryAndPage() ForCategory() ForPage()


//----------------------------------------------------------------------------
// Types
//

struct page_info
{
   __str shname;
   __str flname;
   __str body;
};


//----------------------------------------------------------------------------
// Static Functions
//

//
// DecryptBody
//
static __str DecryptBody(char __str_ars const *str)
{
   ACS_BeginPrint();
   for(; *str; str++)
      ACS_PrintChar(!isprint(*str) ? *str : *str ^ 7);
   return ACS_EndStrParam();
}

//
// GetPageInfo
//
static struct page_info GetPageInfo(bippage_t const *page)
{
   struct page_info pinf;

   pinf.shname = page->category == BIPC_MAIL
      ? page->name
      : Language("LITH_TXT_INFO_SHORT_%S", page->name);

   pinf.body = page->body
      ? page->body
      : Language("LITH_TXT_INFO_DESCR_%S", page->name);

   pinf.flname = page->title
      ? page->title
      : Language("LITH_TXT_INFO_TITLE_%S", page->name);

   if(page->category == BIPC_EXTRA)
      pinf.body = DecryptBody(pinf.body);

   return pinf;
}

//
// SetCurPage
//
void SetCurPage(gui_state_t *g, bip_t *bip, bippage_t *page, __str body)
{
   bip->curpage = page;

   Lith_GUI_TypeOn(g, st_biptypeon, body);
   Lith_GUI_ScrollReset(g, st_bipinfoscr);
}

//
// UnlockPage
//
static void UnlockPage(bip_t *bip, bippage_t *page)
{
   bip->pageavail++;
   bip->categoryavail[page->category]++;
   page->unlocked = true;

   for(int i = 0; i < countof(page->unlocks) && page->unlocks[i]; i++)
      bip->unlock(page->unlocks[i]);
}

//
// AddToBIP_
//
[[__optional_args(1)]]
static void AddToBIP_(bip_t *bip, int categ, __str name, bip_unlocks_t const *unlocks)
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

   if(categ == BIPC_ENEMIES || categ == BIPC_EXTRA) UnlockPage(bip, page);
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

   #define AddToBIP(...) AddToBIP_(bip, categ,  __VA_ARGS__)
   #define Category(c) categ = c
   #define Unlocks(...) &(bip_unlocks_t const){__VA_ARGS__}

   int categ;

   Category(BIPC_WEAPONS); // ------------------------------------------------|
   AddToBIP("ChargeFist", Unlocks("KSKK"));

   switch(p->pclass)
   {
   case pclass_marine:
      AddToBIP("Pistol",          Unlocks("Omakeda"));
      AddToBIP("Shotgun",         Unlocks("Omakeda"));
      AddToBIP("ShotgunUpgr",     Unlocks("Shotgun", "AOF", "DurlaPrime"));
      AddToBIP("ShotgunUpg2",     Unlocks("Shotgun"));
      AddToBIP("SuperShotgun",    Unlocks("ChAri"));
      AddToBIP("CombatRifle",     Unlocks("AllPoint"));
      AddToBIP("RifleUpgr",       Unlocks("CombatRifle"));
      AddToBIP("RifleUpg2",       Unlocks("CombatRifle", "Semaphore"));
      AddToBIP("GrenadeLauncher", Unlocks("Sym43"));
      AddToBIP("LauncherUpgr",    Unlocks("GrenadeLauncher", "UnrealArms"));
      AddToBIP("LauncherUpg2",    Unlocks("GrenadeLauncher", "Sym43"));
      AddToBIP("PlasmaRifle",     Unlocks("AllPoint", "MDDO"));
      AddToBIP("PlasmaUpgr",      Unlocks("PlasmaRifle"));
      AddToBIP("PlasmaUpg2",      Unlocks("PlasmaRifle", "Semaphore"));
      AddToBIP("BFG9000",         Unlocks("Cid"));
      AddToBIP("CannonUpgr",      Unlocks("BFG9000", "SuperDimension"));
      AddToBIP("CannonUpg2",      Unlocks("BFG9000"));
      break;
   case pclass_cybermage:
      AddToBIP("Mateba",        Unlocks("AOF2"));
      AddToBIP("MatebaUpgr",    Unlocks("AOF2", "Algidistari"));
      AddToBIP("ShockRifle");
      AddToBIP("SPAS",          Unlocks("AOF2", "Newvec"));
      AddToBIP("SPASUpgr");
      AddToBIP("SPASUpg2");
      AddToBIP("SMG",           Unlocks("Omakeda", "Sym43"));
      AddToBIP("SMGUpgr");
      AddToBIP("SMGUpg2");
      AddToBIP("SMGUpg3");
      AddToBIP("IonRifle",      Unlocks("KSKK"));
      AddToBIP("IonRifleUpgr");
      AddToBIP("IonRifleUpg2");
      AddToBIP("CPlasmaRifle",  Unlocks("AllPoint", "MDDO"));
      AddToBIP("CPlasmaUpgr");
      AddToBIP("StarDestroyer", Unlocks("Hell"));
      AddToBIP("ShipGunUpgr");
      AddToBIP("ShipGunUpg2");

      AddToBIP("Blade");
      AddToBIP("Delear");
      AddToBIP("Fire");
      AddToBIP("Rend");
      AddToBIP("Hulgyon");
      AddToBIP("StarShot");
      AddToBIP("Cercle");
      break;
   }

   AddToBIP("Revolver",        Unlocks("Earth"));
   AddToBIP("LazShotgun",      Unlocks("Earth"));
   AddToBIP("SniperRifle",     Unlocks("Facer"));
   AddToBIP("MissileLauncher");
   AddToBIP("PlasmaDiffuser",  Unlocks("Sym43", "MDDO", "Semaphore"));

   Category(BIPC_ENEMIES); // ------------------------------------------------|
   AddToBIP("ZombieMan");
   AddToBIP("ShotgunGuy");
   AddToBIP("ChaingunGuy");

   AddToBIP("Imp");
   AddToBIP("Demon");
   AddToBIP("Spectre");

   AddToBIP("LostSoul");
   AddToBIP("Mancubus");
   AddToBIP("Arachnotron");
   AddToBIP("Cacodemon");

   AddToBIP("HellKnight");
   AddToBIP("BaronOfHell");
   AddToBIP("Revenant");

   AddToBIP("PainElemental");
   AddToBIP("Archvile");
   AddToBIP("SpiderMastermind");
   AddToBIP("Cyberdemon");

   AddToBIP("Phantom");
   AddToBIP("IconOfSin");

   Category(BIPC_UPGRADES); // -----------------------------------------------|
   switch(p->pclass)
   {
   case pclass_marine:
      AddToBIP("HeadsUpDisp", Unlocks("OFMD"));
      AddToBIP("JetBooster",  Unlocks("OFMD"));
      AddToBIP("ReflexWetw",  Unlocks("OFMD"));
      AddToBIP("CyberLegs",   Unlocks("OFMD"));
      AddToBIP("Yh0",         Unlocks("DurlaPrime"));
      AddToBIP("DefenseNuke", Unlocks("OFMD"));
      AddToBIP("Adrenaline",  Unlocks("KSKK"));
      break;
   case pclass_cybermage:
      AddToBIP("HeadsUpDispJem");
      AddToBIP("ReflexWetwJem");
      AddToBIP("Magic");
      AddToBIP("SoulCleaver");
      break;
   }

   AddToBIP("VitalScanner", Unlocks("KSKK"));
   AddToBIP("AutoReload",   Unlocks("KSKK"));

   Category(BIPC_PLACES); // -------------------------------------------------|
   AddToBIP("AetosVi");
   AddToBIP("Algidistari");
   AddToBIP("ChAri");
   AddToBIP("DurlaPrime",     Unlocks("Earth", "AetosVi"));
   AddToBIP("Earth");
   AddToBIP("Hell");
   AddToBIP("Mars");
   AddToBIP("OmicronXevv");
   AddToBIP("SuperDimension", Unlocks("BFG9000", "SIGFPE"));

   Category(BIPC_CORPORATIONS); // -------------------------------------------|
   AddToBIP("AllPoint");

   if(p->pclass == pclass_cybermage) AddToBIP("AOF2");
   else                              AddToBIP("AOF");

   AddToBIP("Cid",        Unlocks("SuperDimension", "Earth"));
   AddToBIP("Facer");
   AddToBIP("KSKK",       Unlocks("Earth"));
   AddToBIP("MDDO",       Unlocks("Mars", "OFMD"));
   AddToBIP("Newvec",     Unlocks("Earth"));
   AddToBIP("OFMD");
   AddToBIP("Omakeda",    Unlocks("Earth"));
   AddToBIP("Semaphore",  Unlocks("OmicronXevv"));
   AddToBIP("Sym43",      Unlocks("AetosVi"));
   AddToBIP("UnrealArms", Unlocks("AetosVi"));

   Category(BIPC_EXTRA); // --------------------------------------------------|
   AddToBIP("Extra1");
   AddToBIP("Extra2");
   AddToBIP("Extra3");
   AddToBIP("Extra4");
   AddToBIP("Extra5");
   #undef AddToBIP
   #undef Category
   #undef Unlocks

   ForCategory()
      bip->pagemax += bip->categorymax[categ] = bip->infogr[categ].size;

   if(world.dbgBIP)
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
   int flag = flags | strtoi_str(Language("LITH_TXT_MAIL_FLAG_%S", title), null, 0);

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
      if(page->name == name)
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
      if(categ == BIPC_MAIL || categ == BIPC_EXTRA) continue;

      ForPage()
         page->unlocked = false;
      bip->categoryavail[categ] = 0;
   }

   bip->pageavail = 0;
}

//
// CheckMatch
//
[[__call("ScriptS")]]
static bool CheckMatch(struct page_info *pinf, __str query)
{
   return strcasestr_str(pinf->shname, query) ||
          strcasestr_str(pinf->flname, query) ||
          strcasestr_str(pinf->body,   query);
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
      int n = 0;

      HudMessageF("CBIFONT", "\CTINFO");
      HudMessagePlain(g->hid--, 160.4, 70.1, TICSECOND);

      bip->lastcategory = BIPC_MAIN;

      if(world.grafZoneEntered)
      {
         if(Lith_GUI_Button(g, "Search", 70, 80 + n, .preset = &btnbipmain))
            bip->curcategory = BIPC_SEARCH;
         n += 10;
      }
#define LITH_X(name, capt) \
      if(Lith_GUI_Button_Id(g, BIPC_##name, capt, 70, 80 + n, .preset = &btnbipmain)) \
      { \
         bip->curcategory = BIPC_##name; \
         bip->curpage     = null; \
      } \
      n += 10;
#include "lith_bip.h"
      if(Lith_GUI_Button(g, "Statistics", 70, 80 + n, .preset = &btnbipmain))
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
//    Stat("Rituals Performed",   "%i",   0);

      #undef Stat
   }
   else if(bip->curcategory == BIPC_SEARCH)
   {
      gui_txtbox_state_t *st = Lith_GUI_TextBox(g, st_bipsearch, 23, 65, p->num, p->txtbuf);
      p->clearTextBuf();

      bip->lastcategory = BIPC_MAIN;

      ifauto(char *, c, strchr(st->txtbuf, '\n'))
      {
         // That's a lot of numbers...
         crc64_t const extranames[] = {
            0x5F38B6C56F0A6D84L,
            0x90215131A36573D7L,
            0xC54EC0A7C6836A5BL,
            0xB315B81438717BA6L,
            0x9FD558A2C8C8D163L,
         };

         int size = c - st->txtbuf;
         __str query = StrParam("%.*s", size, st->txtbuf);
         crc64_t crc = Lith_CRC64(st->txtbuf, c - st->txtbuf);

         bip->resnum = bip->rescur = st->tbptr = 0;

         for(int i = 0; i < countof(extranames); i++)
         {
            if(crc == extranames[i])
            {
               list_t *link = bip->infogr[BIPC_EXTRA].next;
               for(int j = 0; j < i; j++)
                  link = link->next;
               bip->result[bip->resnum++] = link->object;
            }
         }

         ForCategoryAndPage()
         {
            if(bip->resnum >= countof(bip->result))
               break;

            if(!page->unlocked || page->category == BIPC_EXTRA)
               continue;

            struct page_info pinf = GetPageInfo(page);

            if(CheckMatch(&pinf, query))
               bip->result[bip->resnum++] = page;
         }

         if(bip->resnum == 0)
            ACS_LocalAmbientSound("player/cbi/findfail", 127);
      }

      if(bip->resnum)
      {
         for(int i = 0; i < bip->rescur; i++)
         {
            bippage_t *page = bip->result[i];
            struct page_info pinf = GetPageInfo(page);
            if(Lith_GUI_Button_Id(g, i, pinf.flname, 70, 80 + (i * 10), .preset = &btnbipmain))
            {
               bip->lastcategory = bip->curcategory;
               bip->curcategory = page->category;
               SetCurPage(g, bip, page, pinf.body);
            }
         }

         if((ACS_Timer() % ACS_Random(10, 20)) == 0 &&
            bip->rescur != bip->resnum)
         {
            if(++bip->rescur == bip->resnum)
               ACS_LocalAmbientSound("player/cbi/finddone", 127);
            else
               ACS_LocalAmbientSound("player/cbi/find", 127);
         }
      }
      else
      {
         HudMessageF("CBIFONT", "\CmNo results");
         HudMessagePlain(g->hid--, 70, 80, TICSECOND);
      }
   }
   else
   {
      list_t *list = &bip->infogr[bip->curcategory];
      size_t n = list->size;
      size_t i = 0;

      Lith_GUI_ScrollBegin(g, st_bipscr, 15, 50, btnlist.w, 170, btnlist.h * n);

      if(bip->curcategory != BIPC_EXTRA)
         Lith_ForListIter(bippage_t *page, *list, i++)
      {
         int y = btnlist.h * i;

         if(Lith_GUI_ScrollOcclude(g, st_bipscr, y, btnlist.h))
            continue;

         struct page_info pinf = GetPageInfo(page);
         __str name = StrParam("%S%S", bip->curpage == page ? "\Ci" : "", pinf.shname);

         if(Lith_GUI_Button_Id(g, i, name, 0, y, !page->unlocked || bip->curpage == page, .preset = &btnlist))
            SetCurPage(g, bip, page, pinf.body);
      }

      Lith_GUI_ScrollEnd(g, st_bipscr);

      if(bip->curpage)
      {
         bippage_t *page = bip->curpage;
         struct page_info pinf = GetPageInfo(page);

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

         HudMessageF("CBIFONT", "\Ci%S", pinf.flname);
         HudMessagePlain(g->hid--, 200.4, 45.1 + oy, TICSECOND);

         #define DrawText(txt, pos, ...) \
            HudMessageF("CBIFONT", "%.*S%S", pos, txt, pos == typeon->len ? Ticker("\n|", "") : "|"), \
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
         bip->curcategory = bip->lastcategory;

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

