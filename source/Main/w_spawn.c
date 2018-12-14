// Copyright © 2018 Alison Sanderson, all rights reserved.
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
   {1000, OBJ "Armor_Standard_Upper"},
   {2000, OBJ "Armor_Standard_Lower"},
   {2500, OBJ "Armor_Teflon_Upper"},
   {3000, OBJ "Armor_Teflon_Lower"},
   {3200, OBJ "Armor_Aerodynamic_Upper"},
   {3400, OBJ "Armor_Aerodynamic_Lower"},
};

static struct witem const barmor[] = {
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
   {base+200, OBJ "Scheelite1"}, \
   {base+400, OBJ "Nambulite1"}, \
   {base+500, OBJ "Lepidolite1"}, \
   {base+600, OBJ "Petalite1"}, \
   {base+700, OBJ "Tourmaline1"}

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
   BonusItemsBig(5000)

static struct witem const hbonus[] = {
   {2000, OBJ "HealthBonus1"},
   {4000, OBJ "HealthBonus2"},
   BonusItems(6000),
};

static struct witem const abonus[] = {
   BonusItems(1000),
};

static struct witem const clip[] = {
   {2000, OBJ "Radio1"},
   {4000, OBJ "Radio2"},
   BonusItems(6000),
};

static struct witem const clipbx[] = {
   {4000, OBJ "BoxOfAmmo1"},
   BonusItemsBig(4000),
};

// Static Functions ----------------------------------------------------------|

static __str RandomWeighted(struct witem const *list, int count)
{
   int sel = ACS_Random(1, list[count - 1].weight);

   for(int i = 0; i < count; i++)
      if(sel <= list[i].weight) return list[i].item;

   return "shush, this will return before this";
}

// Extern Functions ----------------------------------------------------------|

script ext("ACS")
void Lith_RandomSpawn(int rsn)
{
   __str item;

   switch(rsn)
   {
   case lrsn_garmor: item = RandomWeighted(garmor, countof(garmor)); break;
   case lrsn_barmor: item = RandomWeighted(barmor, countof(barmor)); break;
   case lrsn_hbonus: item = RandomWeighted(hbonus, countof(hbonus)); break;
   case lrsn_abonus: item = RandomWeighted(abonus, countof(abonus)); break;
   case lrsn_clip:   item = RandomWeighted(clip,   countof(clip  )); break;
   case lrsn_clipbx: item = RandomWeighted(clipbx, countof(clipbx)); break;
   }

   SCallI("Lith_RandomSpawn", "Set", item);
}

// EOF
