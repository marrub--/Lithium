// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_world.h"

// Types ---------------------------------------------------------------------|

struct witem
{
   int weight;
   __str item;
};

// Static Objects ------------------------------------------------------------|

static struct witem const garmor[] = {
   {1000, "Lith_Armor_Standard_Upper"},
   {2000, "Lith_Armor_Standard_Lower"},
   {2500, "Lith_Armor_Teflon_Upper"},
   {3000, "Lith_Armor_Teflon_Lower"},
   {3200, "Lith_Armor_Aerodynamic_Upper"},
   {3400, "Lith_Armor_Aerodynamic_Lower"},
};

static struct witem const barmor[] = {
   { 3000, "Lith_Armor_Improved_Upper"},
   { 6000, "Lith_Armor_Improved_Lower"},
   { 7000, "Lith_Armor_Teflon_Upper"},
   { 8000, "Lith_Armor_Teflon_Lower"},
   { 9000, "Lith_Armor_Dendrite_Upper"},
   {10000, "Lith_Armor_Dendrite_Lower"},
   {11000, "Lith_Armor_HazMat_Upper"},
   {12000, "Lith_Armor_HazMat_Lower"},
   {12100, "Lith_Armor_PinkSilver_Upper"},
   {12200, "Lith_Armor_PinkSilver_Lower"},
   {12300, "Lith_Armor_Midori_Upper"},
   {12400, "Lith_Armor_Midori_Lower"},
};

static struct witem const hbonus[] = {
   { 2000, "Lith_HealthBonus1"},
   { 4000, "Lith_HealthBonus2"},
   { 6000, "Lith_Coin2"},
   { 7000, "Lith_Coin5"},
   { 8500, "Lith_ScoreChip1"},
   { 9000, "Lith_ScoreChip2"},
   { 9400, "Lith_Ruby4"},
   { 9800, "Lith_Sapphire4"},
   {10200, "Lith_Amethyst6"},
   {10600, "Lith_Diamond1"},
   {11000, "Lith_Emerald2"},
   {11200, "Lith_Scheelite1"},
   {11400, "Lith_Nambulite1"},
   {11500, "Lith_Lepidolite1"},
   {11600, "Lith_Petalite1"},
   {11700, "Lith_Tourmaline1"},
};

// Static Functions ----------------------------------------------------------|

//
// RandomWeighted
//
static __str RandomWeighted(struct witem const *list, int count)
{
   int sel = ACS_Random(1, list[count - 1].weight);

   for(int i = 0; i < count; i++)
      if(sel <= list[i].weight) return list[i].item;
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_RandomSpawn
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_RandomSpawn(int rsn)
{
   __str item;

   switch(rsn)
   {
   case lrsn_garmor: item = RandomWeighted(garmor, countof(garmor)); break;
   case lrsn_barmor: item = RandomWeighted(barmor, countof(barmor)); break;
   case lrsn_hbonus: item = RandomWeighted(hbonus, countof(hbonus)); break;
// case lrsn_abonus: item = RandomWeighted(abonus, countof(abonus)); break;
   }

   ACS_ScriptCall("Lith_RandomSpawn", "Set", item);
}

// EOF
