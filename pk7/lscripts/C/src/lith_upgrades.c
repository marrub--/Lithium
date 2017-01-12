#include "lith_upgrades_common.h"

#define ForUpgrade() \
   for(int i = 0; i < UPGR_MAX; i++) \
      __with(upgrade_t *upgr = &p->upgrades[i];)

#define DefnCallback(name) \
   ForUpgrade() \
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
   {"HeadsUpDisp", 0         , true , "HeadsUpDisp",  UC_Body, -0.10, R(HeadsUpDisp)},
   {"JetBooster",  0         , true , "JetBooster",   UC_Body, -0.10, A(JetBooster),   U(JetBooster), R(JetBooster)},
   {"ReflexWetw",  0         , true , "ReflexWetw",   UC_Body, -0.10, ADU(ReflexWetw), R(ReflexWetw)},
   {"CyberLegs",   1520000   , false, "CyberLegs",    UC_Body,  0.00, ADU(CyberLegs)},
   {"ReactArmour", 3200200   , false, "Yh0",          UC_Body,  0.00, D(ReactArmour)},
   {"DefenseNuke", 580030    , false, "DefenseNuke",  UC_Body,  0.00, E(DefenseNuke)},
   {"Adrenaline",  1801000   , false, "Adrenaline",   UC_Body,  0.00, U(Adrenaline)},
   
   {"GaussShotty", 779430    , false, "ShotgunUpgr",  UC_Weap,  0.00},
   {"RifleModes",  340100    , false, "RifleUpgr",    UC_Weap,  0.00, D(RifleModes), U(RifleModes)},
   {"ChargeRPG",   1150000   , false, "LauncherUpgr", UC_Weap,  0.00},
   {"PlasLaser",   3400000   , false, "PlasmaUpgr",   UC_Weap,  0.00},
   {"PunctCannon", 5600700   , false, "CannonUpgr",   UC_Weap,  0.00, D(PunctCannon)},
   
   {"TorgueMode",  800000000 , false, null,           UC_Extr,  0.00},
// {"RetroWeps",   9999990   , false, null,           UC_Extr,  0.00},
   {"7777777",     82354300  , false, null,           UC_Extr,  0.10, ADU(7777777)},
   {"lolsords",    1000000   , false, null,           UC_Extr,  0.20, ADU(lolsords)},
   
   {"Implying",    0         , false, null,           UC_Down,  0.20, U(Implying)},
   {"UNCEUNCE",    0         , false, null,           UC_Down,  0.30, ADU(UNCEUNCE)},
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
   ForUpgrade()
      if(upgr->active)
         upgr->wasactive = true,  Upgr_ToggleActive(p, upgr);
}

//
// Lith_PlayerReinitUpgrades
//
void Lith_PlayerReinitUpgrades(player_t *p)
{
   ForUpgrade()
      if(upgr->wasactive)
         upgr->wasactive = false, Upgr_ToggleActive(p, upgr);
}

//
// Lith_PlayerLoseUpgrades
//
void Lith_PlayerLoseUpgrades(player_t *p)
{
   ForUpgrade()
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
   ForUpgrade()
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

// EOF

