// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
// vim: columns=120
#include "lith_upgrades_common.h"
#include "lith_world.h"

#define ForUpgrade(name) \
   for(int _i = 0; _i < p->upgrmax; _i++) \
      __with(upgrade_t *name = &p->upgrades[_i];)

#define CheckRequires(a1, a2) (upgr->info->requires & a1 && !(a2))
#define CheckRequires_AI      CheckRequires(UR_AI,  world.cbiupgr[cupg_armorinter])
#define CheckRequires_WMD     CheckRequires(UR_WMD, world.cbiupgr[cupg_weapninter])
#define CheckRequires_WRD     CheckRequires(UR_WRD, world.cbiupgr[cupg_weapninte2])
#define CheckRequires_RDI     CheckRequires(UR_RDI, world.cbiupgr[cupg_rdistinter])
#define CheckRequires_RA      CheckRequires(UR_RA,  p->getUpgr(UPGR_ReactArmor)->owned)

// Static Objects ------------------------------------------------------------|

#define Req(arg) .requires = arg
static upgradeinfo_t UpgrInfoBase[UPGR_BASE_MAX] = {
// {{"Name-------", "BIP---------", Cost---}, pcl_name, UC_Cat-, Pr, Score, [Group], [Requirements]},
   {{"HeadsUpDisp", "HeadsUpDisp",  0      }, pcl_marine,              UC_Body, 1,  -0.05,  UG_HUD},
   {{"HeadsUpDis2", "HeadsUpDisp",  0      }, pcl_cybermage,           UC_Body, 1,  -0.05,  UG_HUD},
   {{"JetBooster",  "JetBooster",   0      }, pcl_marine,              UC_Body, 0,  -0.05},
   {{"ReflexWetw",  "ReflexWetw",   0      }, pcl_outcasts | pcl_mods, UC_Body, 5,  -0.05},
   {{"Zoom",        null,           0      }, pcl_any,                 UC_Body, 0,  0.00},
   {{"VitalScan",   "VitalScanner", 0      }, pcl_any,                 UC_Body, 2,  0.00},
   {{"CyberLegs",   "CyberLegs",    1220000}, pcl_marine,              UC_Body, 4,  0.00},
   {{"ReactArmor",  "Yh0",          3500200}, pcl_marine,              UC_Body, 20, 0.00,   Req(UR_AI)},
   {{"ReactArmor2", "Yh0",          2990200}, pcl_marine,              UC_Body, 10, 0.00,   Req(UR_AI|UR_RA)},
   {{"DefenseNuke", "DefenseNuke",  580030 }, pcl_marine,              UC_Body, 0,  0.00,   Req(UR_AI)},
   {{"Adrenaline",  "Adrenaline",   1801000}, pcl_marine,              UC_Body, 10, 0.00},
   {{"Magic",       "Magic",        0      }, pcl_cybermage,           UC_Body, 0,  0.00},
   {{"SoulCleaver", "SoulCleaver",  1100000}, pcl_cybermage,           UC_Body, 0,  0.00},
   {{"StealthSys",  "StealthSys",   1800450}, pcl_cybermage,           UC_Body, 0,  0.00},

   {{"AutoReload",  "AutoReload",   950050 }, pcl_any,    UC_Weap, 2, 0.00},
   {{"AutoPistol",  null,           140940 }, pcl_marine, UC_Weap, 0, 0.00, UG_Pistol},
   {{"PlasPistol",  null,           340000 }, pcl_marine, UC_Weap, 0, 0.00, UG_Pistol,   Req(UR_WMD)},
   {{"GaussShotty", "ShotgunUpgr",  1079430}, pcl_marine, UC_Weap, 1, 0.00, UG_Shotgun,  Req(UR_WMD)},
   {{"PoisonShot",  "ShotgunUpg2",  1010420}, pcl_marine, UC_Weap, 0, 0.00, UG_Shotgun,  Req(UR_WMD)},
   {{"RifleModes",  "RifleUpgr",    340100 }, pcl_marine, UC_Weap, 0, 0.00,              Req(UR_WMD)},
   {{"LaserRCW",    "RifleUpg2",    1008080}, pcl_marine, UC_Weap, 1, 0.00,              Req(UR_WMD)},
   {{"ChargeRPG",   "LauncherUpgr", 1550000}, pcl_marine, UC_Weap, 0, 0.00, UG_Launcher, Req(UR_WMD|UR_WRD)},
   {{"HomingRPG",   "LauncherUpg2", 2505010}, pcl_marine, UC_Weap, 1, 0.00, UG_Launcher, Req(UR_WMD)},
   {{"PlasLaser",   "PlasmaUpgr",   2250000}, pcl_marine, UC_Weap, 0, 0.00, UG_Plasma,   Req(UR_WMD)},
   {{"PartBeam",    "PlasmaUpg2",   2500000}, pcl_marine, UC_Weap, 1, 0.00, UG_Plasma,   Req(UR_WMD|UR_WRD)},
   {{"PunctCannon", "CannonUpgr",   5100700}, pcl_marine, UC_Weap, 0, 0.00, UG_BFG,      Req(UR_WMD)},
   {{"OmegaRail",   "CannonUpg2",   5800100}, pcl_marine, UC_Weap, 5, 0.00, UG_BFG,      Req(UR_WMD|UR_WRD)},

   {{"Mateba_A",   "MatebaUpgr",   614100 }, pcl_cybermage, UC_Weap, 0, 0.00,  UG_Pistol},
   {{"ShockRif_A", "ShockRifUpgr", 1519590}, pcl_cybermage, UC_Weap, 0, 0.00,  UG_Shotgun},
   {{"ShockRif_B", "ShockRifUpg2", 1911590}, pcl_cybermage, UC_Weap, 0, 0.00,  UG_Shotgun},
   {{"SPAS_A",     "SPASUpgr",     2140400}, pcl_cybermage, UC_Weap, 0, 0.00,  UG_SSG},
   {{"SPAS_B",     "SPASUpg2",     1511100}, pcl_cybermage, UC_Weap, 0, 0.00,  UG_SSG},
   {{"SMG_A",      "SMGUpgr",      2055000}, pcl_cybermage, UC_Weap, 0, 0.00},
   {{"SMG_B",      "SMGUpg2",      2888000}, pcl_cybermage, UC_Weap, 0, 0.00},
   {{"SMG_C",      "SMGUpg3",      998900 }, pcl_cybermage, UC_Weap, 0, 0.00},
   {{"IonRifle_A", "IonRifleUpgr", 1977000}, pcl_cybermage, UC_Weap, 0, 0.00},
   {{"IonRifle_B", "IonRifleUpg2", 2855050}, pcl_cybermage, UC_Weap, 0, 0.00},
   {{"CPlasma_A",  "CPlasmaUpgr",  2230500}, pcl_cybermage, UC_Weap, 0, 0.00,  UG_Plasma},
   {{"ShipGun_A",  "ShipGunUpgr",  6988800}, pcl_cybermage, UC_Weap, 0, 0.00,  UG_BFG},
   {{"ShipGun_B",  "ShipGunUpg2",  7767700}, pcl_cybermage, UC_Weap, 0, 0.00,  UG_BFG},

   {{"TorgueMode", null, 8000000}, pcl_any, UC_Extr, 8, 0.00, Req(UR_RDI)},
   {{"7777777",    null, 8235430}, pcl_any, UC_Extr, 7, 0.10, Req(UR_RDI)},
   {{"lolsords",   null, 1000000}, pcl_any, UC_Extr, 0, 0.20, Req(UR_RDI)},
   {{"Goldeneye",  null, 70000  }, pcl_any, UC_Extr, 0, 0.07, Req(UR_RDI)},

   {{"Implying",   null, 0      }, pcl_any, UC_Down, 0, 0.20},
   {{"UNCEUNCE",   null, 0      }, pcl_any, UC_Down, 0, 0.30},
   {{"InstaDeath", null, 0      }, pcl_any, UC_Down, 0, 0.50},
};
#undef Req

static upgradeinfo_t UpgrInfoEx[UPGR_EXTRA_NUM];

static __str const UpgrCateg[UC_MAX] = {
   [UC_Body] = "\CnBody",
   [UC_Weap] = "\CaWeapon",
   [UC_Extr] = "\CfExtra",
   [UC_Down] = "\CtDowngrade"
};

static upgradeinfo_t *UpgrInfo;
static int UpgrMax = countof(UpgrInfoBase);

// Callbacks -----------------------------------------------------------------|

CallbackDefine(upgr_reinit_cb_t, UpgrReinit)

// Static Functions ----------------------------------------------------------|

//
// RenderProxy
//
[[__call("ScriptS")]]
static void RenderProxy(player_t *p, upgrade_t *upgr)
{
   ACS_SetHudSize(320, 240);
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
   upgradeinfo_t const *u1 = lhs, *u2 = rhs;
   int c1 = u1->category - u2->category;
   if(c1 != 0) return c1;
   else        return u1->key - u2->key;
}

//
// SetDataPtr
//
static void SetDataPtr(player_t *p, upgrade_t *upgr)
{
   upgr->dataptr = &p->upgrdata;
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_UpgradeRegister_
//
upgradeinfo_t *Lith_UpgradeRegister(upgradeinfo_t const *upgr)
{
   upgradeinfo_t *ui = &UpgrInfoEx[UpgrMax++ - UPGR_BASE_MAX];
   *ui = *upgr;
   return ui;
}

//
// Lith_GSReinit_Upgrade
//
void Lith_GSReinit_Upgrade(void)
{
   for(int i = 0; i < UpgrMax; i++)
   {
      upgradeinfo_t *ui = &UpgrInfo[i];

      // Set up static function pointers
      ui->Init = SetDataPtr; // this is set again by UpgrReinit

      ui->shopBuy    = Lith_UpgrShopBuy;
      ui->shopCanBuy = Lith_UpgrCanBuy;
      ui->shopGive   = Lith_UpgrGive;

      // Set up individual upgrades' function pointers
      switch(ui->key)
      {
      #define Ret(n) continue;
      #define Fn_F(n, cb) ui->cb = Upgr_##n##_##cb;
      #define Fn_S(n, cb) Fn_F(n, cb)
      #include "lith_upgradefuncs.h"
         continue;
      }

      CallbackEach(upgr_reinit_cb_t, UpgrReinit)
         if(cb(ui)) goto next;

   next:;
   }

   CallbackClear(upgr_reinit_cb_t, UpgrReinit);
}

//
// Lith_GSInit_Upgrade
//
void Lith_GSInit_Upgrade(void)
{
   if(world.grafZoneEntered)
      Lith_UpgradeRegister(&(upgradeinfo_t const){{"DarkCannon", null, 0x7FFFFFFF}, pcl_marine, UC_Extr, 0, 0.00, UG_BFG, .requires=UR_WMD|UR_WRD|UR_RDI, .key=UPGR_DarkCannon});

   for(int i = 0; i < countof(UpgrInfoBase); i++)
      UpgrInfoBase[i].key  = i;

   UpgrInfo = calloc(UpgrMax, sizeof(upgradeinfo_t));
   memmove(UpgrInfo, UpgrInfoBase, sizeof(UpgrInfoBase));

   for(int i = 0; i < countof(UpgrInfoEx); i++)
      if(UpgrInfoEx[i].name != null)
         UpgrInfo[UPGR_BASE_MAX + i] = UpgrInfoEx[i];

   qsort(UpgrInfo, UpgrMax, sizeof(upgradeinfo_t), CompUpgrInfo);

   for(int i = 0; i < UpgrMax; i++)
      UpgrInfo[i].id = i;

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
[[__call("ScriptS")]]
void Lith_PlayerInitUpgrades(player_t *p)
{
   #define CheckPClass() (UpgrInfo[i].pclass & p->pclass)
   for(int i = 0; i < UpgrMax; i++)
      if(CheckPClass())
         p->upgrmax++;

   upgrademap_t_ctor(&p->upgrademap, p->upgrmax, 1);

   for(int i = 0; i < countof(p->upgrades); i++)
      p->upgrades[i] = (upgrade_t){};

   for(int i = 0, j = 0; i < UpgrMax; i++)
      if(CheckPClass())
   {
      upgrade_t *upgr = &p->upgrades[j];

      UpgrInfo[i].Init(p, upgr);
      upgr->info = &UpgrInfo[i];

      p->upgrademap.insert(upgr);

      if(upgr->info->cost == 0 || world.dbgUpgr)
         Lith_UpgrBuy(p, upgr, true, true);

      j++;
   }

   p->upgrinit = true;
   #undef CheckPClass
}

//
// Lith_PlayerDeallocUpgrades
//
void Lith_PlayerDeallocUpgrades(player_t *p)
{
   upgrademap_t_dtor(&p->upgrademap);
   p->upgrmax = 0;

   for(int i = 0; i < countof(p->upgrades); i++)
      p->upgrades[i] = (upgrade_t){};

   p->upgrinit = false;
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
bool Lith_UpgrCanActivate(player_t *p, upgrade_t *upgr)
{
   if(!upgr->active)
   {
      if((p->pclass == pcl_marine &&
         CheckRequires_AI  ||
         CheckRequires_WMD ||
         CheckRequires_WRD ||
         CheckRequires_RDI ||
         CheckRequires_RA) ||
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

// GUI -----------------------------------------------------------------------|

//
// GUIUpgradesList
//
static void GUIUpgradesList(gui_state_t *g, player_t *p)
{
   if(Lith_GUI_Button(g, .x = 88, 27, .preset = &guipre.btnprev))
      if(g->st[st_upgrfilter].i-- <= 0)
         g->st[st_upgrfilter].i = UC_MAX;

   if(Lith_GUI_Button(g, .x = 88 + guipre.btnprev.w, 27, .preset = &guipre.btnnext))
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

      HudMessageF("CBIFONT", "Filter: %S", UpgrCateg[filter]);
   }
   else
      HudMessageF("CBIFONT", "Filter: \CjAll");

   HudMessagePlain(g->hid--, 15.1, 28.1, TICSECOND);

   Lith_GUI_ScrollBegin(g, st_upgrscr, 15, 38, guipre.btnlist.w, 184, guipre.btnlist.h * numbtns);

   int curcategory = UC_MAX;
   int y = 0;

   for(int i = 0; i < p->upgrmax; i++, y += guipre.btnlist.h)
   {
      bool changed = false;

      upgrade_t *upgr = &p->upgrades[i];

      if(upgr->info->category != curcategory)
      {
         curcategory = upgr->info->category;
         changed = true;

         if(filter == -1)
            y += guipre.btnlist.h;
      }

      if(filter != -1 && curcategory != filter)
      {
         y -= guipre.btnlist.h;
         continue;
      }

      if(Lith_GUI_ScrollOcclude(g, st_upgrscr, y, guipre.btnlist.h))
         continue;

      if(changed && filter == -1)
      {
         HudMessageF("CBIFONT", "%S", UpgrCateg[curcategory]);
         HudMessagePlain(g->hid--, g->ox + 4.1, g->oy + (y - guipre.btnlist.h) + 1.1, TICSECOND);
      }

      __str name = Language("LITH_TXT_UPGRADE_TITLE_%S", upgr->info->name);
      __str color;

      gui_button_preset_t const *preset;

      bool canbuy = p->canBuy(&upgr->info->shopdef, upgr);

           if(!upgr->owned && !canbuy) color = "u";
      else switch(upgr->info->key)
               { case UPGR_TorgueMode: color = "g"; break;
                 case UPGR_DarkCannon: color = "m"; break;
                 default:              color = null;}

           if(upgr->active) preset = &guipre.btnlistactivated;
      else if(upgr->owned)  preset = &guipre.btnlistactive;
      else                  preset = &guipre.btnlistsel;

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
   if(upgr->info->perf && p->pclass != pcl_cybermage)
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

   if(upgr->info->cost) cost = StrParam("%S%S", Lith_ScoreSep(p->getCost(&upgr->info->shopdef)), mark);
   else                 cost = "Free";

   HudMessageF("CBIFONT", "%S", cost);
   HudMessagePlain(g->hid--, 111.1, 30.1, TICSECOND);

   // Category
   HudMessageF("CBIFONT", "%S", UpgrCateg[upgr->info->category]);
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
   if(Lith_GUI_Button(g, "Buy", 111, 205, !p->canBuy(&upgr->info->shopdef, upgr)))
      Lith_UpgrBuy(p, upgr, false);

   if(Lith_GUI_Button(g, upgr->active ? "Deactivate" : "Activate", 111 + guipre.btndef.w + 2, 205, !upgr->canUse(p)))
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
