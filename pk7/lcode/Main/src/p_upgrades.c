#include "lith_upgrades_common.h"

#include "Lth_manifest.h"

#define ForUpgrade(name) \
   for(int _i = 0; _i < UPGR_MAX; _i++) \
      __with(upgrade_t *name = &p->upgrades[_i];)


//----------------------------------------------------------------------------
// Static Objects
//

static upgradeinfo_t const upgradeinfo[UPGR_MAX] = {
// {{"Name-------", "BIP---------", Cost----}, UC_Cat-, Score, [Group]},
   {{"HeadsUpDisp", "HeadsUpDisp",  0       }, UC_Body, -0.05},
   {{"JetBooster",  "JetBooster",   0       }, UC_Body, -0.05},
   {{"ReflexWetw",  "ReflexWetw",   0       }, UC_Body, -0.05},
   {{"Zoom",        null,           0       }, UC_Body,  0.00},
   {{"CyberLegs",   "CyberLegs",    1220000 }, UC_Body,  0.00},
   {{"ReactArmor",  "Yh0",          3500200 }, UC_Body,  0.00},
   {{"ReactArmor2", "Yh0",          2990200 }, UC_Body,  0.00},
   {{"DefenseNuke", "DefenseNuke",  580030  }, UC_Body,  0.00},
   {{"Adrenaline",  "Adrenaline",   1801000 }, UC_Body,  0.00},
   {{"VitalScan",   "VitalScanner", 601700  }, UC_Body,  0.00},
   
   {{"AutoReload",  "AutoReload",   950050  }, UC_Weap,  0.00},
   {{"AutoPistol",  null,           140940  }, UC_Weap,  0.00, ug_pistol},
   {{"PlasPistol",  null,           340000  }, UC_Weap,  0.00, ug_pistol},
   {{"GaussShotty", "ShotgunUpgr",  1079430 }, UC_Weap,  0.00, ug_shotgun},
   {{"PoisonShot",  "ShotgunUpg2",  1010420 }, UC_Weap,  0.00, ug_shotgun},
   {{"RifleModes",  "RifleUpgr",    340100  }, UC_Weap,  0.00},
   {{"LaserRCW",    "RifleUpg2",    1008080 }, UC_Weap,  0.00},
   {{"ChargeRPG",   "LauncherUpgr", 1550000 }, UC_Weap,  0.00, ug_launcher},
   {{"HomingRPG",   "LauncherUpg2", 2505010 }, UC_Weap,  0.00, ug_launcher},
   {{"PlasLaser",   "PlasmaUpgr",   2250000 }, UC_Weap,  0.00, ug_plasma},
   {{"PartBeam",    "PlasmaUpg2",   2500000 }, UC_Weap,  0.00, ug_plasma},
   {{"PunctCannon", "CannonUpgr",   5100700 }, UC_Weap,  0.00, ug_bfg},
   {{"OmegaRail",   "CannonUpg2",   5800100 }, UC_Weap,  0.00, ug_bfg},
   
   {{"TorgueMode",  null,           80000000}, UC_Extr,  0.00},
   {{"7777777",     null,           82354300}, UC_Extr,  0.10},
   {{"lolsords",    null,           1000000 }, UC_Extr,  0.20},
   
   {{"Implying",    null,           0       }, UC_Down,  0.20},
   {{"UNCEUNCE",    null,           0       }, UC_Down,  0.30},
   {{"InstaDeath",  null,           0       }, UC_Down,  0.50},
};


//----------------------------------------------------------------------------
// Extern Objects
//

__str Lith_AutoGroupNames[NUMAUTOGROUPS] = {"\Ca#1", "\Cd#2", "\Cn#3", "\Ck#4"};


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
static bool Lith_UpgrCanBuy(player_t *p, shopdef_t const *, void *upgr_)
{
   upgrade_t *upgr = upgr_;
   return !upgr->owned;
}

//
// Lith_UpgrShopBuy
//
static void Lith_UpgrShopBuy(player_t *p, shopdef_t const *, void *upgr)
{
   Lith_UpgrSetOwned(p, upgr);
}

//
// Lith_UpgrGive
//
static void Lith_UpgrGive(int tid, shopdef_t const *, void *upgr_)
{
   upgrade_t const *upgr = upgr_;
   
   ACS_SetUserVariable(tid, "lith_upgradeid", upgr->info->id);
   
   switch(upgr->info->category)
   {
   case UC_Body: ACS_SetUserVariable(tid, "lith_upgradebody", true); break;
   case UC_Weap: ACS_SetUserVariable(tid, "lith_upgradeweap", true); break;
   default:      ACS_SetUserVariable(tid, "lith_upgradeextr", true); break;
   }
}

//
// LoadBalance
//
static void LoadBalance(upgradeinfo_t *upgradeinfo_mut)
{
   Lth_ResourceMap *rsrc = Lth_ManifestLoad_extern(c"Upgrades.lthm");
   
   if(!rsrc)
      return;
   
   for(int i = 0; i < UPGR_MAX; i++)
   {
      __str name = upgradeinfo[i].name;
      
      int   const *cost     = Lth_HashMapFind(&rsrc->map, StrParam("%S.Cost",     name));
      fixed const *scoreadd = Lth_HashMapFind(&rsrc->map, StrParam("%S.ScoreAdd", name));
      if(cost)     upgradeinfo_mut[i].cost     = *cost;
      if(scoreadd) upgradeinfo_mut[i].scoreadd = *scoreadd;
   }
   
   Lth_ResourceMapDestroy(rsrc);
}


//----------------------------------------------------------------------------
// External Functions
//

//
// Lith_UpgrSetOwned
//
void Lith_UpgrSetOwned(player_t *p, upgrade_t *upgr)
{
   if(upgr->owned) return;
   
   upgr->owned = true;
   p->upgradesowned++;
   
   if(upgr->info->category == UC_Body && upgr->info->cost == 0)
      Lith_UpgrToggle(p, upgr);
}

//
// Lith_GSInit_Upgrade
//
void Lith_GSInit_Upgrade(bool first)
{
   upgradeinfo_t *upgradeinfo_mut = (upgradeinfo_t *)upgradeinfo;
   
   #define A(n) upgradeinfo_mut[UPGR_##n].Activate   = Upgr_##n##_Activate;
   #define D(n) upgradeinfo_mut[UPGR_##n].Deactivate = Upgr_##n##_Deactivate;
   #define U(n) upgradeinfo_mut[UPGR_##n].Update     = Upgr_##n##_Update;
   #define E(n) upgradeinfo_mut[UPGR_##n].Enter      = Upgr_##n##_Enter;
   #define R(n) upgradeinfo_mut[UPGR_##n].Render     = Upgr_##n##_Render;
   #include "lith_upgradefuncs.h"
   
   for(int i = 0; i < UPGR_MAX; i++)
   {
      upgradeinfo_t *info = &upgradeinfo_mut[i];
      info->shopBuy    = Lith_UpgrShopBuy;
      info->shopCanBuy = Lith_UpgrCanBuy;
      info->shopGive   = Lith_UpgrGive;
      info->id = i;
   }
   
   if(first)
      LoadBalance(upgradeinfo_mut);
}

//
// Lith_PlayerInitUpgrades
//
void Lith_PlayerInitUpgrades(player_t *p)
{
   for(int i = 0; i < UPGR_MAX; i++)
   {
      upgrade_t *upgr = &p->upgrades[i];
      memset(upgr, 0, sizeof(upgr));
      
      upgr->info = &upgradeinfo[i];
      
      if(upgr->info->cost == 0)
         Lith_UpgrSetOwned(p, upgr);
      
      if(ACS_GetCVar("__lith_debug_on") && !ACS_GetCVar("__lith_debug_noupgrades"))
      {
         if(upgr->info->cost != 0)
            Lith_UpgrSetOwned(p, upgr);
         
         if(ACS_StrCmp(ACS_GetCVarString("__lith_debug_upgrade"), upgr->info->name) == 0)
            Lith_UpgrToggle(p, upgr);
      }
   }
}

//
// Lith_PlayerDeinitUpgrades
//
void Lith_PlayerDeinitUpgrades(player_t *p)
{
   ForUpgrade(upgr)
      if(upgr->active)
         upgr->wasactive = true,  Lith_UpgrToggle(p, upgr);
}

//
// Lith_PlayerReinitUpgrades
//
void Lith_PlayerReinitUpgrades(player_t *p)
{
   ForUpgrade(upgr)
      if(upgr->wasactive)
         upgr->wasactive = false, Lith_UpgrToggle(p, upgr);
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
   extern void Lith_CheckAutoBuy(player_t *p);
   
   Lith_CheckAutoBuy(p);
   
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
// Lith_UpgrToggle
//
bool Lith_UpgrToggle(player_t *p, upgrade_t *upgr)
{
   if(!upgr->owned) return false;
   
   upgr->active = !upgr->active;
   
   if(upgr->active && upgr->info->group)
      ForUpgrade(other)
         if(other != upgr && other->active && other->info->group == upgr->info->group)
            Lith_UpgrToggle(p, other);
   
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

//
// Lith_CBITab_Upgrades
//
void Lith_CBITab_Upgrades(gui_state_t *g, player_t *p)
{
   static __str const upgrcateg[UC_MAX] = {
      [UC_Body] = "\CnBody",
      [UC_Weap] = "\CaWeapon",
      [UC_Extr] = "\CfExtra",
      [UC_Down] = "\CtDowngrade"
   };
   
   Lith_GUI_ScrollBegin(g, st_upgrscr, 15, 30, btnlist.w, 192, btnlist.h * UPGR_MAX);
   
   for(int i = 0; i < UPGR_MAX; i++)
   {
      int y = btnlist.h * i;
      
      if(Lith_GUI_ScrollOcclude(g, st_upgrscr, y, btnlist.h))
         continue;
      
      upgrade_t *upgr = &p->upgrades[i];
      
      __str name = Language("LITH_TXT_UPGRADE_TITLE_%S", upgr->info->name);
      __str color;
      
      gui_button_preset_t const *preset;
      
      bool canbuy = Lith_ShopCanBuy(p, &upgr->info->shopdef, upgr);
      
           if(!upgr->owned && !canbuy) color = "u";
      else if(i == UPGR_TorgueMode)    color = "g";
      else                             color = null;
      
           if(upgr->active) preset = &btnlistactivated;
      else if(upgr->owned)  preset = &btnlistactive;
      else                  preset = &btnlistsel;
      
      if(Lith_GUI_Button_Id(g, i, name, 0, y, i == g->st[st_upgrsel].i, .color = color, .preset = preset))
         g->st[st_upgrsel].i = i;
      
      HudMessageF("CBIFONT", "%.3S", upgrcateg[upgr->info->category]);
      HudMessageAdd(g->hid--, g->ox + 2.1, g->oy + y + 1.1, TICSECOND, 0.57);
      
      for(int i = 0; i < NUMAUTOGROUPS; i++)
         if(upgr->autogroups[i])
            DrawSpritePlain(StrParam("lgfx/UI/Group%i.png", i + 1), g->hid--,
               g->ox + btnlist.w + 0.2, g->oy + y + 1.1, TICSECOND);
   }
   
   Lith_GUI_ScrollEnd(g, st_upgrscr);
   
   int sel = g->st[st_upgrsel].i;
   upgrade_t *upgr = &p->upgrades[sel];
   
   if(g->st[st_upgrselold].i != g->st[st_upgrsel].i)
   {
      Lith_GUI_TypeOn(g, st_upgrtypeon, Language("LITH_TXT_UPGRADE_DESCR_%S", upgr->info->name));
      g->st[st_upgrselold].i = g->st[st_upgrsel].i;
   }
   
   __str mark;
   switch(sel)
   {
   case UPGR_lolsords:   mark = "\Cjfolds"; break;
   case UPGR_TorgueMode: mark = "\Cd$";     break;
   default:              mark = "\Cnscr";   break;
   }
   
   __str cost = "---";
   
   if(upgr->info->cost)
      cost = StrParam("%S%S", Lith_ScoreSep(Lith_ShopGetCost(p, &upgr->info->shopdef)), mark);
   
   ACS_SetHudClipRect(111, 30, 190, 170, 184);
   
   HudMessageF("CBIFONT", "%LS: %S", "LITH_COST", cost);
   HudMessagePlain(g->hid--, 111.1, 30.1, TICSECOND);
   
   HudMessageF("CBIFONT", "%LS: %S", "LITH_CATEGORY", upgrcateg[upgr->info->category]);
   HudMessagePlain(g->hid--, 111.1, 40.1, TICSECOND);
   
   if(upgr->info->scoreadd != 0.0)
   {
      __str color = upgr->info->scoreadd < 0 ? "\Ca" : "\Cn";
      
      HudMessageF("CBIFONT", "%LS: %S%i\Cl%%", "LITH_SCOREMULT", color, ceilk(100.0 * (upgr->info->scoreadd + 1.0)));
      HudMessagePlain(g->hid--, 111.1, 50.1, TICSECOND);
      
      double after;
      if(upgr->active) after = p->scoremul - upgr->info->scoreadd;
      else             after = p->scoremul + upgr->info->scoreadd;
      
      color = after - p->scoremul < 0 ? "\Ca" : "\Cn";
      HudMessageF("CBIFONT", "%LS: \Cj%i\C-%% -> %S%i\C-%%", "LITH_TOTALMULT", ceilk(100.0 * p->scoremul), color, ceilk(100.0 * after));
      HudMessagePlain(g->hid--, 111.1, 60.1, TICSECOND);
   }
   
   HudMessageF("CBIFONT", "Effect: %S", Language("LITH_TXT_UPGRADE_EFFEC_%S", upgr->info->name));
   HudMessageParams(HUDMSG_PLAIN, g->hid--, CR_WHITE, 111.1, 70.1, TICSECOND);
   
   HudMessageF("CBIFONT", "----------------------------------------------");
   HudMessageParams(HUDMSG_PLAIN, g->hid--, CR_GREEN, 111.1, 100.1, TICSECOND);
   
   gui_typeon_state_t const *typeon = Lith_GUI_TypeOnUpdate(g, st_upgrtypeon);
   
   if(sel != UPGR_UNCEUNCE)
      HudMessageF        ("CBIFONT", "%.*S", typeon->pos, typeon->txt);
   else
      HudMessageRainbowsF("CBIFONT", "%.*S", typeon->pos, typeon->txt);
   
   HudMessagePlain(g->hid--, 111.1, 110.1, TICSECOND);
   
   ACS_SetHudClipRect(0, 0, 0, 0);
   
   if(Lith_GUI_Button(g, "Buy", 111, 205, !Lith_ShopCanBuy(p, &upgr->info->shopdef, upgr)))
      Lith_UpgrBuy(p, upgr);
   
   if(Lith_GUI_Button(g, upgr->active ? "Deactivate" : "Activate", 111 + btndefault.w + 2, 205, !upgr->owned))
      Lith_UpgrToggle(p, upgr);
   
   HudMessageF("CNFONT", "Auto-Groups");
   HudMessagePlain(g->hid--, 255, 205, TICSECOND);
   
   for(int i = 0; i < NUMAUTOGROUPS; i++)
      if(Lith_GUI_Checkbox_Id(g, i, upgr->autogroups[i], 225 + (i * 20), 215, Lith_AutoGroupNames[i]))
      {
         upgr->autogroups[i] = !upgr->autogroups[i];
         p->saveData();
      }
}

// EOF

