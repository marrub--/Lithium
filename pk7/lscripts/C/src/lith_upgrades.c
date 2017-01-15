#include "lith_upgrades_common.h"

#define ForUpgrade(name) \
   for(int i = 0; i < UPGR_MAX; i++) \
      __with(upgrade_t *name = &p->upgrades[i];)

#define DefnCallback(name) \
   ForUpgrade(upgr) \
      if(upgr->active && upgr->info->name) \
         upgr->info->name(p, upgr);


//----------------------------------------------------------------------------
// Static Objects
//

#define A(n) .Activate   = Upgr_##n##_Activate
#define D(n) .Deactivate = Upgr_##n##_Deactivate
#define U(n) .Update     = Upgr_##n##_Update
#define E(n) .Enter      = Upgr_##n##_Enter
#define R(n) .Render     = Upgr_##n##_Render
#define AD(n) A(n), D(n)
#define ADU(n) AD(n), U(n)

static upgradeinfo_t const upgradeinfo[UPGR_MAX] = {
// {"Name-------", Cost------, Auto-, BIP-----------, UC_Cat-, Score, Callbacks...},
   {"HeadsUpDisp", 0         , true , "HeadsUpDisp",  UC_Body, -0.05, R(HeadsUpDisp)},
   {"JetBooster",  0         , true , "JetBooster",   UC_Body, -0.05, A(JetBooster),   U(JetBooster), R(JetBooster)},
   {"ReflexWetw",  0         , true , "ReflexWetw",   UC_Body, -0.05, ADU(ReflexWetw), R(ReflexWetw)},
   {"CyberLegs",   1520000   , false, "CyberLegs",    UC_Body,  0.00, ADU(CyberLegs)},
   {"ReactArmour", 3200200   , false, "Yh0",          UC_Body,  0.00, D(ReactArmour)},
   {"DefenseNuke", 580030    , false, "DefenseNuke",  UC_Body,  0.00, E(DefenseNuke)},
   {"Adrenaline",  1801000   , false, "Adrenaline",   UC_Body,  0.00, U(Adrenaline)},
   
   {"GaussShotty", 1779430   , false, "ShotgunUpgr",  UC_Weap,  0.00, weapon_shotgun},
   {"RifleModes",  340100    , false, "RifleUpgr",    UC_Weap,  0.00, weapon_rifle, D(RifleModes), U(RifleModes)},
   {"ChargeRPG",   1050000   , false, "LauncherUpgr", UC_Weap,  0.00, weapon_launcher},
   {"PlasLaser",   2300000   , false, "PlasmaUpgr",   UC_Weap,  0.00, weapon_plasma},
   {"PunctCannon", 5600700   , false, "CannonUpgr",   UC_Weap,  0.00, weapon_bfg, D(PunctCannon)},
   
   {"TorgueMode",  80000000  , false, null,           UC_Extr,  0.00},
// {"RetroWeps",   9999990   , false, null,           UC_Extr,  0.00},
   {"7777777",     82354300  , false, null,           UC_Extr,  0.10, ADU(7777777)},
   {"lolsords",    1000000   , false, null,           UC_Extr,  0.20, ADU(lolsords)},
   
   {"Implying",    0         , false, null,           UC_Down,  0.20, U(Implying)},
   {"UNCEUNCE",    0         , false, null,           UC_Down,  0.30, ADU(UNCEUNCE)},
   {"InstaDeath",  0         , false, null,           UC_Down,  0.50, U(InstaDeath)},
};


//----------------------------------------------------------------------------
// Static Functions
//

[[__call("ScriptS")]]
static void RenderProxy(player_t *p, upgrade_t *upgr)
{
   ACS_SetHudSize(320, 200);
   upgr->info->Render(p, upgr);
}

//----------------------------------------------------------------------------
// External Functions
//

//
// Upgr_ToggleActive
//
void Upgr_ToggleActive(player_t *p, upgrade_t *upgr)
{
   if(!upgr->owned) return;
   
   upgr->active = !upgr->active;
   
   if(upgr->active && upgr->info->wepclass)
      ForUpgrade(other)
         if(other != upgr && other->active && other->info->wepclass == upgr->info->wepclass)
            Upgr_ToggleActive(p, other);
   
   if(upgr->active && upgr->info->Activate)
   {
      upgr->info->Activate(p, upgr);
      p->scoremul += upgr->info->scoreadd;
   }
   else if(!upgr->active && upgr->info->Deactivate)
   {
      upgr->info->Deactivate(p, upgr);
      p->scoremul -= upgr->info->scoreadd;
   }
}

//
// Upgr_SetOwned
//
void Upgr_SetOwned(player_t *p, upgrade_t *upgr)
{
   if(upgr->owned)
   {
      Log("Upgr_SetOwned: YOU CANNOT BUY THESE THINGS");
      return;
   }
   
   upgr->owned = true;
   p->upgradesowned++;
   
   if(upgr->info->bipunlock)
      Lith_UnlockBIPPage(&p->bip, upgr->info->bipunlock);
   
   if(upgr->info->auto_activate)
      Upgr_ToggleActive(p, upgr);
}

//
// Upgr_CanBuy
//
bool Upgr_CanBuy(player_t *p, upgrade_t *upgr)
{
   return !upgr->owned && (p->score - Lith_PlayerDiscount(upgr->info->cost)) >= 0;
}

//
// Upgr_Buy
//
void Upgr_Buy(player_t *p, upgrade_t *upgr)
{
   if(!Upgr_CanBuy(p, upgr))
   {
      Log("Upgr_Buy: YOU CANNOT BUY THESE THINGS");
      return;
   }
   
   Lith_TakeScore(p, Lith_PlayerDiscount(upgr->info->cost));
   Upgr_SetOwned(p, upgr);
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
         Upgr_SetOwned(p, upgr);
   }
}

//
// Lith_PlayerDeinitUpgrades
//
void Lith_PlayerDeinitUpgrades(player_t *p)
{
   ForUpgrade(upgr)
      if(upgr->active)
         upgr->wasactive = true,  Upgr_ToggleActive(p, upgr);
}

//
// Lith_PlayerReinitUpgrades
//
void Lith_PlayerReinitUpgrades(player_t *p)
{
   ForUpgrade(upgr)
      if(upgr->wasactive)
         upgr->wasactive = false, Upgr_ToggleActive(p, upgr);
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
   DefnCallback(Update);
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
   DefnCallback(Enter);
}

//
// Lith_CBITab_Upgrades
//
void Lith_CBITab_Upgrades(gui_state_t *g, player_t *p)
{
   static __str const upgrcateg[UC_MAX] = {
      [UC_Body] = "\ChBody",
      [UC_Weap] = "\CaWeapon",
      [UC_Extr] = "\CfExtra",
      [UC_Down] = "\CtDowngrade"
   };
   
   Lith_GUI_ScrollBegin(g, st_upgrscr, 15, 30, btnlist.w, 152, btnlist.h * UPGR_MAX);
   
   for(int i = 0; i < UPGR_MAX; i++)
   {
      int y = btnlist.h * i;
      
      if(Lith_GUI_ScrollOcclude(g, st_upgrscr, y, btnlist.h))
         continue;
      
      __str name = Language("LITH_TXT_UPGRADE_TITLE_%S", p->upgrades[i].info->name);
      
      if(Lith_GUI_Button_Id(g, i, name, 0, y, i == g->st[st_upgrsel].i, .preset = &btnlist))
         g->st[st_upgrsel].i = i;
      
      HudMessageF("CBIFONT", "%.3S", upgrcateg[p->upgrades[i].info->category]);
      HudMessagePlain(g->hid--, g->ox + 2.1, g->oy + y + 1.1, TICSECOND);
   }
   
   Lith_GUI_ScrollEnd(g, st_upgrscr);
   
   int sel = g->st[st_upgrsel].i;
   upgrade_t *upgr = &p->upgrades[sel];
   
   __str mark;
   switch(sel)
   {
   case UPGR_lolsords:   mark = "\Cjfolds"; break;
   case UPGR_TorgueMode: mark = "\Cd$";     break;
   default:              mark = "\Cnscr";   break;
   }
   
   __str cost = upgr->info->cost ? StrParam("%lli%S", Lith_PlayerDiscount(upgr->info->cost), mark) : "---";
   int   yofs = 0;
   
   ACS_SetHudClipRect(111, 30, 184, 150, 184);
   
   HudMessageF("CBIFONT", "%LS: %S", "LITH_COST", cost);
   HudMessagePlain(g->hid--, 111.1, 30.1, TICSECOND);
   
   HudMessageF("CBIFONT", "%LS: %S", "LITH_CATEGORY", upgrcateg[upgr->info->category]);
   HudMessagePlain(g->hid--, 111.1, 40.1, TICSECOND);
   
   if(upgr->info->scoreadd != 0.0)
   {
      __str color = upgr->info->scoreadd < 0 ? "\Ca" : "\Cn";
      HudMessageF("CBIFONT", "%LS: %S%i\Cl%%", "LITH_SCOREMULT", color, ceilk(100.0 * (upgr->info->scoreadd + 1.0)));
      HudMessagePlain(g->hid--, 111.1, 50.1, TICSECOND);
      yofs = 10;
   }
   
   if(sel != UPGR_UNCEUNCE)
      HudMessageF        ("CBIFONT", "%S", Language("LITH_TXT_UPGRADE_DESCR_%S", upgr->info->name));
   else
      HudMessageRainbowsF("CBIFONT", "%S", Language("LITH_TXT_UPGRADE_DESCR_%S", upgr->info->name));
   
   HudMessagePlain(g->hid--, 111.1, 50.1 + yofs, TICSECOND);
   
   ACS_SetHudClipRect(0, 0, 0, 0);
   
   if(Lith_GUI_Button(g, "Buy", 259, 170, !Upgr_CanBuy(p, upgr)))
      Upgr_Buy(p, upgr);
   
   if(Lith_GUI_Button(g, upgr->active ? "Deactivate" : "Activate", 209, 170, !upgr->owned))
      Upgr_ToggleActive(p, upgr);
}

// EOF

