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

#define BonusItemsBig(base) \
   {base+200, "Lith_Scheelite1"}, \
   {base+400, "Lith_Nambulite1"}, \
   {base+500, "Lith_Lepidolite1"}, \
   {base+600, "Lith_Petalite1"}, \
   {base+700, "Lith_Tourmaline1"}

#define BonusItems(base) \
   {base+   0, "Lith_Coin2"}, \
   {base+1000, "Lith_Coin5"}, \
   {base+2500, "Lith_ScoreChip1"}, \
   {base+3000, "Lith_ScoreChip2"}, \
   {base+3400, "Lith_Ruby4"}, \
   {base+3800, "Lith_Sapphire4"}, \
   {base+4200, "Lith_Amethyst6"}, \
   {base+4600, "Lith_Diamond1"}, \
   {base+5000, "Lith_Emerald2"}, \
   BonusItemsBig(5000)

static struct witem const hbonus[] = {
   {2000, "Lith_HealthBonus1"},
   {4000, "Lith_HealthBonus2"},
   BonusItems(6000),
};

static struct witem const abonus[] = {
   BonusItems(1000),
};

static struct witem const clip[] = {
   {2000, "Lith_Radio1"},
   {4000, "Lith_Radio2"},
   BonusItems(6000),
};

static struct witem const clipbx[] = {
   {4000, "Lith_BoxOfAmmo1"},
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

   scall("Lith_RandomSpawn", "Set", item);
}

// EOF
