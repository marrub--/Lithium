#include "lith_common.h"
#include "lith_player.h"
#include "lith_upgrades.h"

// ---------------------------------------------------------------------------
// Data.
//

upgradeinfo_t const upgrade_info[UPGR_MAX] = {
   // Body
   [UPGR_JetBooster]  = { 0         , true,  null, null, null },
   [UPGR_CyberLegs]   = { 900000    , false, null, null, null },
   [UPGR_ReactArmour] = { 3200200   , false, null, null, null },
   [UPGR_Splitter]    = { 800000    , false, null, null, null },
   // Weapons
   [UPGR_GaussShotty] = { 770430    , false, null, null, null },
   [UPGR_RifleModes]  = { 340100    , false, null, null, null },
   [UPGR_ChargeNader] = { 850000    , false, null, null, null },
   [UPGR_PlasLaser]   = { 1400000   , false, null, null, null },
   [UPGR_OmegaRail]   = { 2600700   , false, null, null, null },
   // Downgrades
   [UPGR_SeriousMode] = { 0         , false, null, null, null },
   [UPGR_RetroWeps]   = { 0         , false, null, null, null },
   [UPGR_lolsords]    = { 0         , false, null, null, null },
   // :v
   [UPGR_Implying]    = { 0         , false, null, null, null },
   [UPGR_ZharkovMode] = { -100      , false, null, null, null },
};

static __str upgrade_enums[] = {
   #define U(en, name) [UPGR_##en] = #en,
   #include "lith_upgradenames.h"
   null
};

__str upgrade_names[] = {
   #define U(en, name) [UPGR_##en] = name,
   #include "lith_upgradenames.h"
   null
};

// ---------------------------------------------------------------------------
// Scripts.
//

void Upgr_ToggleActive(player_t *p, upgrade_t *upgr)
{
   upgr->active = !upgr->active;
   
   if(upgr->active && upgr->info->Activate)
      upgr->info->Activate(p, upgr);
   else if(!upgr->active && upgr->info->Deactivate)
      upgr->info->Deactivate(p, upgr);
}

void Upgr_SetOwned(player_t *p, upgrade_t *upgr)
{
   if(upgr->owned)
   {
      Log("Upgr_SetOwned: YOU CANNOT BUY THESE THINGS");
      return;
   }
   
   if(upgr->info->auto_activate)
      Upgr_ToggleActive(p, upgr);
   
   upgr->owned = true;
}

bool Upgr_CanBuy(player_t *p, upgrade_t *upgr)
{
   return !upgr->owned && (p->score - upgr->info->cost) >= 0;
}

void Upgr_Buy(player_t *p, upgrade_t *upgr)
{
   if(!Upgr_CanBuy(p, upgr))
   {
      Log("Upgr_Buy: YOU CANNOT BUY THESE THINGS");
      return;
   }
   
   p->score -= upgr->info->cost;
   Upgr_SetOwned(p, upgr);
}

void Lith_PlayerInitUpgrades(player_t *p)
{
   for(int i = 0; i < UPGR_MAX; i++)
   {
      upgrade_t *upgr = &p->upgrades[i];
      
      upgr->info = &upgrade_info[i];
      upgr->name = upgrade_names[i];
      upgr->description = Language("LITH_TXT_UPGRADE_%S", upgrade_enums[i]);
      
      if(upgr->info->cost == 0)
         Upgr_SetOwned(p, upgr);
   }
   
   p->upgrades_wasinit = true;
}

void Lith_PlayerUpdateUpgrades(player_t *p)
{
   for(int i = 0; i < UPGR_MAX; i++)
   {
      upgrade_t *upgr = &p->upgrades[i];
      if(upgr->active && upgr->info->Update)
         upgr->info->Update(p, upgr);
   }
}

