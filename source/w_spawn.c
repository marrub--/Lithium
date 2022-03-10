// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Random spawners.                                                         │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "w_world.h"

struct witem
{
   i32 weight;
   cstr item;
};

static
struct witem const garmor[] = {
   {1000, OBJ "Armor_Standard_Upper"},
   {2000, OBJ "Armor_Standard_Lower"},
   {2500, OBJ "Armor_Teflon_Upper"},
   {3000, OBJ "Armor_Teflon_Lower"},
   {3200, OBJ "Armor_Aerodynamic_Upper"},
   {3400, OBJ "Armor_Aerodynamic_Lower"},
};

static
struct witem const barmor[] = {
   { 3000, OBJ "Armor_Improved_Upper"},
   { 6000, OBJ "Armor_Improved_Lower"},
   { 7000, OBJ "Armor_Teflon_Upper"},
   { 8000, OBJ "Armor_Teflon_Lower"},
   { 9000, OBJ "Armor_Dendrite_Upper"},
   {10000, OBJ "Armor_Dendrite_Lower"},
   {11000, OBJ "Armor_HazMat_Upper"},
   {12000, OBJ "Armor_HazMat_Lower"},
   {12100, OBJ "Armor_PinkSilver_Upper"},
   {12200, OBJ "Armor_PinkSilver_Lower"},
   {12300, OBJ "Armor_Midori_Upper"},
   {12400, OBJ "Armor_Midori_Lower"},
};

#define BonusItemsBig(base) \

#define BonusItems(base) \
   {base+   0, OBJ "Coin2"}, \
   {base+1000, OBJ "Coin5"}, \
   {base+2500, OBJ "ScoreChip1"}, \
   {base+3000, OBJ "ScoreChip2"}, \
   {base+3400, OBJ "Ruby4"}, \
   {base+3800, OBJ "Sapphire4"}, \
   {base+4200, OBJ "Amethyst6"}, \
   {base+4600, OBJ "Diamond1"}, \
   {base+5000, OBJ "Emerald2"}, \
   {base+5200, OBJ "Scheelite1"}, \
   {base+5400, OBJ "Nambulite1"}, \
   {base+5500, OBJ "Lepidolite1"}, \
   {base+5600, OBJ "Petalite1"}, \
   {base+5700, OBJ "Tourmaline1"}

static
struct witem const hbonus[] = {
   {2000, OBJ "HealthBonus1"},
   {4000, OBJ "HealthBonus2"},
   BonusItems(6000),
};

static
struct witem const abonus[] = {
   BonusItems(1000),
};

static
cstr RandomWeighted(struct witem const *l, i32 c) {
   i32 sel = ACS_Random(1, l[c - 1].weight);

   for(i32 i = 0; i < c; i++) if(sel <= l[i].weight) return l[i].item;

   return nil;
}

script_str ext("ACS") addr(OBJ "RandomSpawn")
void Sc_RandomSpawn(i32 rsn) {
   cstr item;

   switch(rsn) {
   case lrsn_garmor: item = RandomWeighted(garmor, countof(garmor)); break;
   case lrsn_barmor: item = RandomWeighted(barmor, countof(barmor)); break;
   case lrsn_hbonus: item = RandomWeighted(hbonus, countof(hbonus)); break;
   case lrsn_abonus: item = RandomWeighted(abonus, countof(abonus)); break;
   default: return;
   }

   SCallV(so_RandomSpawn, sm_Set, fast_strdup(item));
}

/* EOF */
