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
};

static struct witem const barmor[] = {
   {2000, "Lith_Armor_Improved_Upper"},
   {4000, "Lith_Armor_Improved_Lower"},
   {5000, "Lith_Armor_Teflon_Upper"},
   {6000, "Lith_Armor_Teflon_Lower"},
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
   case lrsn_greenarmor: item = RandomWeighted(garmor, countof(garmor)); break;
   case lrsn_bluearmor:  item = RandomWeighted(barmor, countof(barmor)); break;
   }

   ACS_ScriptCall("Lith_RandomSpawn", "Set", item);
}

// EOF
