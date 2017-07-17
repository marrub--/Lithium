#include "lith_upgrades_common.h"
#include "lith_world.h"

#include <Lth.h>

#define ForUpgrade(name) \
   for(int _i = 0; _i < p->upgrmax; _i++) \
      __with(upgrade_t *name = &p->upgrades[_i];)

#define CheckRequires(a1, a2) (upgr->info->requires & a1 && !(a2))
#define CheckRequires_AI      CheckRequires(UR_AI,  world.cbiupgr[cupg_armorinter])
#define CheckRequires_WMD     CheckRequires(UR_WMD, world.cbiupgr[cupg_weapninter])
#define CheckRequires_WRD     CheckRequires(UR_WRD, world.cbiupgr[cupg_weapninte2])
#define CheckRequires_RDI     CheckRequires(UR_RDI, world.cbiupgr[cupg_rdistinter])
#define CheckRequires_RA      CheckRequires(UR_RA,  p->getUpgr(UPGR_ReactArmor)->owned)


//----------------------------------------------------------------------------
// Types
//

enum
{
   UG_None,
   UG_Pistol,
   UG_Shotgun,
   UG_Rifle,
   UG_Launcher,
   UG_Plasma,
   UG_BFG,
   UG_HUD,
   UG_BASE_MAX
};

enum
{
   UR_AI  = 1 << 0,
   UR_WMD = 1 << 1,
   UR_WRD = 1 << 2,
   UR_RDI = 1 << 3,
   UR_RA  = 1 << 4,
};


//----------------------------------------------------------------------------
// Static Objects
//

#define Req(arg) .requires = arg
static upgradeinfo_t staticupgradeinfo[UPGR_BASE_MAX] = {
// {{"Name-------", "BIP---------", Cost----}, pclass_name,      UC_Cat-, Pr, Score, [Group], [Requirements]},
   {{"HeadsUpDisp", "HeadsUpDisp",  0       }, pclass_marine,    UC_Body,  1, -0.05, UG_HUD},
   {{"HeadsUpDis2", "HeadsUpDisp",  0       }, pclass_cybermage, UC_Body,  1, -0.05, UG_HUD},
   {{"JetBooster",  "JetBooster",   0       }, pclass_marine,    UC_Body,  0, -0.05},
   {{"ReflexWetw",  "ReflexWetw",   0       }, pclass_any,       UC_Body,  5, -0.05},
   {{"Zoom",        null,           0       }, pclass_any,       UC_Body,  0,  0.00},
   {{"VitalScan",   "VitalScanner", 0       }, pclass_any,       UC_Body,  2,  0.00},
   {{"Magic",       "Magic",        0       }, pclass_cybermage, UC_Body,  0,  0.00},
   {{"CyberLegs",   "CyberLegs",    1220000 }, pclass_marine,    UC_Body,  4,  0.00},
   {{"ReactArmor",  "Yh0",          3500200 }, pclass_marine,    UC_Body, 20,  0.00, Req(UR_AI)},
   {{"ReactArmor2", "Yh0",          2990200 }, pclass_marine,    UC_Body, 10,  0.00, Req(UR_AI|UR_RA)},
   {{"DefenseNuke", "DefenseNuke",  580030  }, pclass_marine,    UC_Body,  0,  0.00, Req(UR_AI)},
   {{"Adrenaline",  "Adrenaline",   1801000 }, pclass_marine,    UC_Body, 10,  0.00},
   {{"SoulCleaver", "SoulCleaver",  1100000 }, pclass_cybermage, UC_Body, 10,  0.00},

   {{"AutoReload",  "AutoReload",   950050  }, pclass_any,       UC_Weap,  2,  0.00},
   {{"AutoPistol",  null,           140940  }, pclass_marine,    UC_Weap,  0,  0.00, UG_Pistol},
   {{"PlasPistol",  null,           340000  }, pclass_marine,    UC_Weap,  0,  0.00, UG_Pistol,   Req(UR_WMD)},
   {{"GaussShotty", "ShotgunUpgr",  1079430 }, pclass_marine,    UC_Weap,  1,  0.00, UG_Shotgun,  Req(UR_WMD)},
   {{"PoisonShot",  "ShotgunUpg2",  1010420 }, pclass_marine,    UC_Weap,  0,  0.00, UG_Shotgun,  Req(UR_WMD)},
   {{"RifleModes",  "RifleUpgr",    340100  }, pclass_marine,    UC_Weap,  0,  0.00,              Req(UR_WMD)},
   {{"LaserRCW",    "RifleUpg2",    1008080 }, pclass_marine,    UC_Weap,  1,  0.00,              Req(UR_WMD)},
   {{"ChargeRPG",   "LauncherUpgr", 1550000 }, pclass_marine,    UC_Weap,  0,  0.00, UG_Launcher, Req(UR_WMD|UR_WRD)},
   {{"HomingRPG",   "LauncherUpg2", 2505010 }, pclass_marine,    UC_Weap,  1,  0.00, UG_Launcher, Req(UR_WMD)},
   {{"PlasLaser",   "PlasmaUpgr",   2250000 }, pclass_marine,    UC_Weap,  0,  0.00, UG_Plasma,   Req(UR_WMD)},
   {{"PartBeam",    "PlasmaUpg2",   2500000 }, pclass_marine,    UC_Weap,  1,  0.00, UG_Plasma,   Req(UR_WMD|UR_WRD)},
   {{"PunctCannon", "CannonUpgr",   5100700 }, pclass_marine,    UC_Weap,  0,  0.00, UG_BFG,      Req(UR_WMD)},
   {{"OmegaRail",   "CannonUpg2",   5800100 }, pclass_marine,    UC_Weap,  5,  0.00, UG_BFG,      Req(UR_WMD|UR_WRD)},
   {{"Mateba_A",    "MatebaUpgr",   614100  }, pclass_cybermage, UC_Weap,  0,  0.00, UG_Pistol},
   {{"SPAS_A",      "SPASUpgr",     0       }, pclass_cybermage, UC_Weap,  0,  0.00, UG_Shotgun},
   {{"SPAS_B",      "SPASUpg2",     0       }, pclass_cybermage, UC_Weap,  0,  0.00, UG_Shotgun},
   {{"SMG_A",       "SMGUpgr",      2055000 }, pclass_cybermage, UC_Weap,  0,  0.00},
   {{"SMG_B",       "SMGUpg2",      2888000 }, pclass_cybermage, UC_Weap,  0,  0.00},
   {{"SMG_C",       "SMGUpg3",      998900  }, pclass_cybermage, UC_Weap,  0,  0.00},
   {{"IonRifle_A",  "IonRifleUpgr", 0       }, pclass_cybermage, UC_Weap,  0,  0.00, UG_Launcher},
   {{"IonRifle_B",  "IonRifleUpg2", 0       }, pclass_cybermage, UC_Weap,  0,  0.00, UG_Launcher},
   {{"CPlasma_A",   "CPlasmaUpgr",  0       }, pclass_cybermage, UC_Weap,  0,  0.00, UG_Plasma},
   {{"ShipGun_A",   "ShipGunUpgr",  6688800 }, pclass_cybermage, UC_Weap,  0,  0.00, UG_BFG},
   {{"ShipGun_B",   "ShipGunUpg2",  0       }, pclass_cybermage, UC_Weap,  0,  0.00, UG_BFG},

   {{"TorgueMode",  null,           8000000 }, pclass_any,       UC_Extr,  8,  0.00, Req(UR_RDI)},
   {{"7777777",     null,           8235430 }, pclass_any,       UC_Extr,  7,  0.10, Req(UR_RDI)},
   {{"lolsords",    null,           1000000 }, pclass_any,       UC_Extr,  0,  0.20, Req(UR_RDI)},
   {{"Goldeneye",   null,           70000   }, pclass_any,       UC_Extr,  0,  0.07, Req(UR_RDI)},

   {{"Implying",    null,           0       }, pclass_any,       UC_Down,  0,  0.20},
   {{"UNCEUNCE",    null,           0       }, pclass_any,       UC_Down,  0,  0.30},
   {{"InstaDeath",  null,           0       }, pclass_any,       UC_Down,  0,  0.50},
};
#undef Req

static upgradeinfo_t extraupgradeinfo[UPGR_EXTRA_NUM];

static __str const upgrcateg[UC_MAX] = {
   [UC_Body] = "\CnBody",
   [UC_Weap] = "\CaWeapon",
   [UC_Extr] = "\CfExtra",
   [UC_Down] = "\CtDowngrade"
};

//static int numextraupgradecallbacks;
//static upgrade_cb_register_t extraupgradecallbacks;


//----------------------------------------------------------------------------
// Extern Objects
//

upgradeinfo_t *upgradeinfo;
int UPGR_MAX = countof(staticupgradeinfo);


//----------------------------------------------------------------------------
// Static Functions
//

//
// RenderProxy
//
[[__call("ScriptS")]]
static void RenderProxy(player_t *p, upgrade_t *upgr)
{
   ACS_SetHudSize(320, 200);
   upgr->info->Render(p, upgr);
}

//
// Lith_UpgrCanBuy
//
static bool Lith_UpgrCanBuy(player_t *p, shopdef_t const *, void *upgr)
{
   return !((upgrade_t *)upgr)->owned;
}

//
// Lith_UpgrShopBuy
//
static void Lith_UpgrShopBuy(player_t *p, shopdef_t const *, void *upgr)
{
   ((upgrade_t *)upgr)->setOwned(p);
}

//
// Lith_UpgrGive
//
static bool Lith_UpgrGive(player_t *, shopdef_t const *, void *upgr_, int tid)
{
   upgrade_t const *upgr = upgr_;
   
   ACS_SetUserVariable(tid, "user_upgradeid", upgr->info->key);
   
   switch(upgr->info->category)
   {
   case UC_Body: ACS_SetUserVariable(tid, "user_upgradebody", true); break;
   case UC_Weap: ACS_SetUserVariable(tid, "user_upgradeweap", true); break;
   default:      ACS_SetUserVariable(tid, "user_upgradeextr", true); break;
   }
   
   return true;
}

//
// CompUpgrInfo
//
static int CompUpgrInfo(void const *lhs, void const *rhs)
{
   upgradeinfo_t const *u1 = lhs;
   upgradeinfo_t const *u2 = rhs;
   int c1 = u1->category - u2->category;
   if(c1 != 0) return c1;
   else        return u1->key - u2->key;
}

//
// RegisterBasicUpgrade
//
static void RegisterBasicUpgrade(upgradeinfo_t *upgr)
{
   if(upgr)
      extraupgradeinfo[UPGR_MAX++ - UPGR_BASE_MAX] = *upgr;
   else
      Log("%s: Failed to register upgrade!", __func__);
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_LoadUpgrInfoBalance
//
// Load extra balance info from a file.
//
void Lith_LoadUpgrInfoBalance(upgradeinfo_t *uinfo, int max, char const *fname)
{
   Lth_ResourceMap *rsrc = Lth_ManifestLoad_extern(fname);
   
   if(!rsrc)
      return;
   
   for(int i = 0; i < max; i++)
   {
      __str name = uinfo[i].name;
      
      int   const *cost     = Lth_HashMapFind(&rsrc->map, StrParam("%S.Cost",     name));
      fixed const *scoreadd = Lth_HashMapFind(&rsrc->map, StrParam("%S.ScoreAdd", name));
      if(cost)     uinfo[i].cost     = *cost;
      if(scoreadd) uinfo[i].scoreadd = *scoreadd;
   }
   
   Lth_ResourceMapDestroy(rsrc);
}

//
// Lith_GSReinit_Upgrade
//
void Lith_GSReinit_Upgrade(void)
{
   // Set up function pointers for upgrade info.
   for(int i = 0; i < UPGR_MAX; i++)
   {
      switch(upgradeinfo[i].key)
      {
      #define Case(n) continue; case UPGR_##n:
      #define A(n) upgradeinfo[i].Activate   = Upgr_##n##_Activate;
      #define D(n) upgradeinfo[i].Deactivate = Upgr_##n##_Deactivate;
      #define U(n) upgradeinfo[i].Update     = Upgr_##n##_Update;
      #define E(n) upgradeinfo[i].Enter      = Upgr_##n##_Enter;
      #define R(n) upgradeinfo[i].Render     = Upgr_##n##_Render;
      #include "lith_upgradefuncs.h"
         continue;
      }
      
      // TODO
      //for(int i = 0; i < numextraupgradecallbacks; i++)
         //extraupgradecallbacks[i](&upgradeinfo[i]);
   }
   
   // Load shop function pointers and IDs.
   for(int i = 0; i < UPGR_MAX; i++)
   {
      upgradeinfo[i].shopBuy    = Lith_UpgrShopBuy;
      upgradeinfo[i].shopCanBuy = Lith_UpgrCanBuy;
      upgradeinfo[i].shopGive   = Lith_UpgrGive;
   }
}

//
// Lith_GSInit_Upgrade
//
void Lith_GSInit_Upgrade(void)
{
   if(world.grafZoneEntered)
      RegisterBasicUpgrade(&(upgradeinfo_t)
         {{"DarkCannon", null, 0x7FFFFFFF}, pclass_any, UC_Extr, 0, 0.00, UG_BFG, .requires=UR_WMD|UR_WRD|UR_RDI, .key=UPGR_DarkCannon});
   
   for(int i = 0; i < countof(staticupgradeinfo); i++)
      staticupgradeinfo[i].key = i;
   
   Lith_LoadUpgrInfoBalance(staticupgradeinfo, countof(staticupgradeinfo), c"Upgrades.lthm");
   
   upgradeinfo = calloc(UPGR_MAX, sizeof(upgradeinfo_t));
   memmove(upgradeinfo, staticupgradeinfo, sizeof(staticupgradeinfo));
   
   for(int i = 0; i < countof(extraupgradeinfo); i++)
      if(extraupgradeinfo[i].name != null)
         upgradeinfo[UPGR_BASE_MAX + i] = extraupgradeinfo[i];
   
   qsort(upgradeinfo, UPGR_MAX, sizeof(upgradeinfo_t), CompUpgrInfo);
   
   for(int i = 0; i < UPGR_MAX; i++)
      upgradeinfo[i].id = i;
   
   Lith_GSReinit_Upgrade();
}

//
// Lith_UpgrSetOwned
//
void Lith_UpgrSetOwned(player_t *p, upgrade_t *upgr)
{
   if(upgr->owned) return;
   
   upgr->owned = true;
   p->upgradesowned++;
   
   if(upgr->info->category == UC_Body && upgr->info->cost == 0)
      upgr->toggle(p);
}

//
// Lith_PlayerInitUpgrades
//
void Lith_PlayerInitUpgrades(player_t *p)
{
   #define CheckPClass() (upgradeinfo[i].pclass == pclass_any || upgradeinfo[i].pclass == p->pclass)
   
   for(int i = 0; i < UPGR_MAX; i++)
      if(CheckPClass())
         p->upgrmax++;
   
   p->upgrademap.alloc(p->upgrmax);
   
   memset(p->upgrades, 0, sizeof(p->upgrades));
   
   for(int i = 0, j = 0; i < UPGR_MAX; i++)
   {
      if(CheckPClass())
      {
         upgrade_t *upgr = &p->upgrades[j];
         
         upgr->dataptr = &p->upgrdata;
         upgr->info = &upgradeinfo[i];
         
         p->upgrademap.elem.data[j].keyhash = upgr->info->key;
         p->upgrademap.elem.data[j].value   = upgr;
         
         if(upgr->info->cost == 0 || world.dbgUpgr)
            upgr->setOwned(p);
         
         j++;
      }
   }
   
   p->upgrademap.build();
   
   #undef CheckPClass
}

//
// Lith_PlayerDeinitUpgrades
//
void Lith_PlayerDeinitUpgrades(player_t *p)
{
   ForUpgrade(upgr)
      if(upgr->active)
         upgr->wasactive = true,  upgr->toggle(p);
}

//
// Lith_PlayerReinitUpgrades
//
void Lith_PlayerReinitUpgrades(player_t *p)
{
   ForUpgrade(upgr)
      if(upgr->wasactive)
         upgr->wasactive = false, upgr->toggle(p);
}

//
// Lith_PlayerLoseUpgrades
//
void Lith_PlayerLoseUpgrades(player_t *p)
{
   ForUpgrade(upgr)
      if(upgr->info->cost != 0 && upgr->owned)
   {
      upgr->owned = false;
      p->upgradesowned--;
   }
}

//
// Lith_PlayerUpdateUpgrades
//
void Lith_PlayerUpdateUpgrades(player_t *p)
{
   if(Lith_ScriptCall("Lith_PauseManager", "GetPaused"))
      return;

   ForUpgrade(upgr)
      if(upgr->active && upgr->info->Update)
         upgr->info->Update(p, upgr);
}

//
// Lith_PlayerRenderUpgrades
//
void Lith_PlayerRenderUpgrades(player_t *p)
{
   ForUpgrade(upgr)
      if(upgr->active && upgr->info->Render)
         RenderProxy(p, upgr);
}

//
// Lith_PlayerEnterUpgrades
//
void Lith_PlayerEnterUpgrades(player_t *p)
{
   ForUpgrade(upgr)
      if(upgr->active && upgr->info->Enter)
         upgr->info->Enter(p, upgr);
}

//
// Lith_UpgrCanActivate
//
bool Lith_UpgrCanActivate(player_t *p, struct upgrade_s *upgr)
{
   if(!upgr->active)
   {
      if(CheckRequires_AI  ||
         CheckRequires_WMD ||
         CheckRequires_WRD ||
         CheckRequires_RDI ||
         CheckRequires_RA  ||
         
         p->cbi.pruse + upgr->info->perf > world.cbiperf)
      {
         return false;
      }
   }
   
   return upgr->owned;
}

//
// Lith_UpgrToggle
//
bool Lith_UpgrToggle(player_t *p, upgrade_t *upgr)
{
   if(!upgr->canUse(p)) return false;
   
   upgr->active = !upgr->active;
   
   if(upgr->active) p->cbi.pruse += upgr->info->perf;
   else             p->cbi.pruse -= upgr->info->perf;
   
   if(upgr->active && upgr->info->group)
      ForUpgrade(other)
         if(other != upgr && other->active && other->info->group == upgr->info->group)
            other->toggle(p);
   
   if(upgr->active)
   {
      if(upgr->info->Activate)
         upgr->info->Activate(p, upgr);

      p->scoremul += upgr->info->scoreadd;
   }
   else
   {
      if(upgr->info->Deactivate)
         upgr->info->Deactivate(p, upgr);

      p->scoremul -= upgr->info->scoreadd;
   }
   
   return true;
}


//----------------------------------------------------------------------------
// GUI
//

//
// GUIUpgradesList
//
static void GUIUpgradesList(gui_state_t *g, player_t *p)
{
   if(Lith_GUI_Button(g, .x = 88, 27, .preset = &btnprev))
      if(g->st[st_upgrfilter].i-- <= 0)
         g->st[st_upgrfilter].i = UC_MAX;
   
   if(Lith_GUI_Button(g, .x = 88 + btnprev.w, 27, .preset = &btnnext))
      if(g->st[st_upgrfilter].i++ >= UC_MAX)
         g->st[st_upgrfilter].i = 0;
   
   int numbtns = p->upgrmax + UC_MAX;
   int filter  = g->st[st_upgrfilter].i - 1;
   
   if(filter != -1)
   {
      numbtns = 0;
      for(int i = 0; i < p->upgrmax; i++)
         if(p->upgrades[i].info->category == filter)
            numbtns++;
      
      HudMessageF("CBIFONT", "Filter: %S", upgrcateg[filter]);
   }
   else
      HudMessageF("CBIFONT", "Filter: \CjAll");
   
   HudMessagePlain(g->hid--, 15.1, 28.1, TICSECOND);
   
   Lith_GUI_ScrollBegin(g, st_upgrscr, 15, 38, btnlist.w, 184, btnlist.h * numbtns);
   
   int curcategory = UC_MAX;
   int y = 0;
   
   for(int i = 0; i < p->upgrmax; i++, y += btnlist.h)
   {
      bool changed = false;
      
      upgrade_t *upgr = &p->upgrades[i];
      
      if(upgr->info->category != curcategory)
      {
         curcategory = upgr->info->category;
         changed = true;
         
         if(filter == -1)
            y += btnlist.h;
      }
      
      if(filter != -1 && curcategory != filter)
      {
         y -= btnlist.h;
         continue;
      }
      
      if(Lith_GUI_ScrollOcclude(g, st_upgrscr, y, btnlist.h))
         continue;
      
      if(changed && filter == -1)
      {
         HudMessageF("CBIFONT", "%S", upgrcateg[curcategory]);
         HudMessagePlain(g->hid--, g->ox + 4.1, g->oy + (y - btnlist.h) + 1.1, TICSECOND);
      }
      
      __str name = Language("LITH_TXT_UPGRADE_TITLE_%S", upgr->info->name);
      __str color;
      
      gui_button_preset_t const *preset;
      
      bool canbuy = Lith_ShopCanBuy(p, &upgr->info->shopdef, upgr);
      
           if(!upgr->owned && !canbuy) color = "u";
      else switch(upgr->info->key)
               { case UPGR_TorgueMode: color = "g"; break;
                 case UPGR_DarkCannon: color = "m"; break;
                 default:              color = null;}
      
           if(upgr->active) preset = &btnlistactivated;
      else if(upgr->owned)  preset = &btnlistactive;
      else                  preset = &btnlistsel;
      
      if(Lith_GUI_Button_Id(g, i, name, 0, y, i == g->st[st_upgrsel].i, .color = color, .preset = preset))
         g->st[st_upgrsel].i = i;
   }
   
   Lith_GUI_ScrollEnd(g, st_upgrscr);
}

//
// GUIUpgradeRequirements
//
static void GUIUpgradeRequirements(gui_state_t *g, player_t *p, upgrade_t *upgr)
{
   int y = 0;
   
   #define Req(name) \
   { \
      HudMessageF("CBIFONT", "\CgRequires " name "."); \
      HudMessagePlain(g->hid--, 111.1, 200 + y + 0.2, TICSECOND); \
      y -= 10; \
   }
   
   if(CheckRequires_AI)  Req("Armor Interface")
   if(CheckRequires_WMD) Req("Weapon Modification Device")
   if(CheckRequires_WRD) Req("Weapon Refactoring Device")
   if(CheckRequires_RDI) Req("Reality Distortion Interface")
   if(CheckRequires_RA)  Req("Reactive Armor")
   
   #undef Req
   
   // Performance rating
   if(upgr->info->perf)
   {
      char cr = upgr->info->perf + p->cbi.pruse > world.cbiperf ? 'a' : 'j';
      
      if(upgr->active)
         HudMessageF("CBIFONT", "Disabling saves \Cn%i\CbPr\C-.", upgr->info->perf);
      else
         HudMessageF("CBIFONT", "Activating requires \C%c%i\CbPr\C-.", cr, upgr->info->perf);
      
      HudMessagePlain(g->hid--, 111.1, 200 + y + 0.2, TICSECOND);
      y -= 10;
   }
   
   // Score multiplier
   if(upgr->info->scoreadd != 0)
   {
      char  cr;
      __str op;
      bool  chk;
      
      if(upgr->active) {chk = upgr->info->scoreadd > 0; op = "Disabling";}
      else             {chk = upgr->info->scoreadd < 0; op = "Enabling" ;}
      
      int perc = abs(ceilk(100.0 * upgr->info->scoreadd));
      if(chk) {cr = 'a'; perc = 100 - perc;}
      else    {cr = 'n'; perc = 100 + perc;}
      
      HudMessageF("CBIFONT", "%S will multiply score by \C%c%i\C-%%", op, cr, perc);
      HudMessagePlain(g->hid--, 111.1, 200 + y + 0.2, TICSECOND);
      y -= 10;
   }
}

//
// GUIUpgradeDescription
//
static void GUIUpgradeDescription(gui_state_t *g, player_t *p, upgrade_t *upgr)
{
   ACS_SetHudClipRect(111, 30, 190, 170, 184);
   
   // Cost
   __str mark;
   __str cost;
   
   switch(upgr->info->key)
   {
   case UPGR_lolsords:   mark = "\Cjfolds"; break;
   case UPGR_TorgueMode: mark = "\Cd$";     break;
   default:              mark = "\Cnscr";   break;
   }
   
   if(upgr->info->cost) cost = StrParam("%S%S", Lith_ScoreSep(Lith_ShopGetCost(p, &upgr->info->shopdef)), mark);
   else                 cost = "Free";
   
   HudMessageF("CBIFONT", "%S", cost);
   HudMessagePlain(g->hid--, 111.1, 30.1, TICSECOND);
   
   // Category
   HudMessageF("CBIFONT", "%S", upgrcateg[upgr->info->category]);
   HudMessagePlain(g->hid--, 111.1, 40.1, TICSECOND);
   
   // Effect
   ifauto(__str, effect, LanguageNull("LITH_TXT_UPGRADE_EFFEC_%S", upgr->info->name))
      if(upgr->info->key == UPGR_UNCEUNCE) HudMessageRainbowsF("CBIFONT", "Effect: %S", effect);
      else                                 HudMessageF        ("CBIFONT", "Effect: %S", effect);
   HudMessageParams(HUDMSG_PLAIN, g->hid--, CR_WHITE, 111.1, 50.1, TICSECOND);
   
   ACS_SetHudClipRect(0, 0, 0, 0);
}

//
// GUIUpgradeButtons
//
static void GUIUpgradeButtons(gui_state_t *g, player_t *p, upgrade_t *upgr)
{
   if(Lith_GUI_Button(g, "Buy", 111, 205, !Lith_ShopCanBuy(p, &upgr->info->shopdef, upgr)))
      Lith_UpgrBuy(p, upgr, false);
   
   if(Lith_GUI_Button(g, upgr->active ? "Deactivate" : "Activate", 111 + btndefault.w + 2, 205, !upgr->canUse(p)))
      upgr->toggle(p);
}

//
// Lith_CBITab_Upgrades
//
void Lith_CBITab_Upgrades(gui_state_t *g, player_t *p)
{
   //DrawSpriteAlpha("lgfx/UI/ItemBG.png", g->hid--, 113.1, 95.1, TICSECOND, 0.5);
   
   GUIUpgradesList(g, p);
   
   upgrade_t *upgr = &p->upgrades[g->st[st_upgrsel].i];
   
   GUIUpgradeDescription (g, p, upgr);
   GUIUpgradeButtons     (g, p, upgr);
   GUIUpgradeRequirements(g, p, upgr);
}

// EOF

