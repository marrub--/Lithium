#include "lith_upgrades_common.h"

#define ForUpgrade() \
   for(int i = 0; i < UPGR_MAX; i++) \
      __with(upgrade_t *upgr = &p->upgrades[i];)


//----------------------------------------------------------------------------
// Static Objects
//

#define A(n) .Activate   = Upgr_##n##_Activate
#define D(n) .Deactivate = Upgr_##n##_Deactivate
#define U(n) .Update     = Upgr_##n##_Update
#define E(n) .Enter      = Upgr_##n##_Enter

static upgradeinfo_t const upgrade_info[UPGR_MAX] = {
// {"Name-------", Cost------, Auto-, BIP-----------, UC_Cat-, Score, Callbacks...},
   {"JetBooster",  0         , true , "JetBooster",   UC_Body, -0.15, U(JetBooster)},
   {"ReflexWetw",  0         , true , "ReflexWetw",   UC_Body, -0.15, A(ReflexWetw), D(ReflexWetw), U(ReflexWetw)},
   {"CyberLegs",   1520000   , false, "CyberLegs",    UC_Body,  0.00, A(CyberLegs),  D(CyberLegs),  U(CyberLegs)},
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
   {"7777777",     82354300  , false, null,           UC_Extr,  0.10, A(7777777),  D(7777777),  U(7777777)},
   {"lolsords",    1000000   , false, null,           UC_Extr,  0.20, A(lolsords), D(lolsords), U(lolsords)},
   
   {"Implying",    0         , false, null,           UC_Down,  0.20, U(Implying)},
   {"UNCEUNCE",    0         , false, null,           UC_Down,  0.30, A(UNCEUNCE), D(UNCEUNCE), U(UNCEUNCE)},
};

#undef A
#undef D
#undef U
#undef E


//----------------------------------------------------------------------------
// External Functions
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

bool Upgr_CanBuy(player_t *p, upgrade_t *upgr)
{
   return !upgr->owned && (p->score - Lith_PlayerDiscount(upgr->info->cost)) >= 0;
}

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

void Lith_PlayerInitUpgrades(player_t *p)
{
   for(int i = 0; i < UPGR_MAX; i++)
   {
      upgrade_t *upgr = &p->upgrades[i];
      memset(upgr, 0, sizeof(upgr));
      
      upgr->info = &upgrade_info[i];
      
      if(upgr->info->cost == 0)
         Upgr_SetOwned(p, upgr);
   }
}

void Lith_PlayerUpdateUpgrades(player_t *p)
{
   ForUpgrade()
      if(upgr->active && upgr->info->Update)
         upgr->info->Update(p, upgr);
}

void Lith_PlayerEnterUpgrades(player_t *p)
{
   ForUpgrade()
      if(upgr->active && upgr->info->Enter)
         upgr->info->Enter(p, upgr);
}

void Lith_PlayerDeinitUpgrades(player_t *p)
{
   ForUpgrade()
      if(upgr->active)
         upgr->wasactive = true,  Upgr_ToggleActive(p, upgr);
}

void Lith_PlayerReinitUpgrades(player_t *p)
{
   ForUpgrade()
      if(upgr->wasactive)
         upgr->wasactive = false, Upgr_ToggleActive(p, upgr);
}

void Lith_PlayerLoseUpgrades(player_t *p)
{
   ForUpgrade()
      if(upgr->info->cost != 0 && upgr->owned)
   {
      upgr->owned = false;
      p->upgradesowned--;
   }
}

// EOF

