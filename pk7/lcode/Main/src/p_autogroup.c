#include "lith_common.h"
#include "lith_player.h"


//----------------------------------------------------------------------------
// Scripts
//

//
// Lith_KeyBuyAutoGroup
//
[[__call("ScriptS"), __extern("ACS"), __script("Net")]]
void Lith_KeyBuyAutoGroup(int group)
{
   if(group < 1 || group > NUMAUTOGROUPS) return;
   
   group -= 1;
   
   player_t *p = LocalPlayer;
   
   int success = 0, total = 0;
   for(int i = 0; i < UPGR_MAX; i++)
      if(!p->upgrades[i].owned && p->upgrades[i].autogroups[group])
      {  total++;
         if(Lith_UpgrBuy(p, &p->upgrades[i]))
            success++;}
   
   char color;
   
   if(success) {color = 'j'; ACS_LocalAmbientSound("player/cbi/auto/buy",     127);}
   else        {color = 'g'; ACS_LocalAmbientSound("player/cbi/auto/invalid", 127);}
   
   if(total)
      Lith_Log(p, "> Bought \C%c%i/%i\C- upgrades in group %S\C-.",
         color, success, total, Lith_AutoGroupNames[group]);
   else
      Lith_Log(p, "> No items to buy in group %S\C-.", Lith_AutoGroupNames[group]);
}

//
// Lith_KeyToggleAutoGroup
//
[[__call("ScriptS"), __extern("ACS"), __script("Net")]]
void Lith_KeyToggleAutoGroup(int group)
{
   if(group < 1 || group > NUMAUTOGROUPS) return;
   
   group -= 1;
   
   player_t *p = LocalPlayer;
   
   int total = 0;
   for(int i = 0; i < UPGR_MAX; i++)
      if(p->upgrades[i].owned && p->upgrades[i].autogroups[group])
      {  total++;
         Lith_UpgrToggle(p, &p->upgrades[i]);}
   
   if(total)
   {
      ACS_LocalAmbientSound("player/cbi/auto/toggle", 127);
      Lith_Log(p, "> Toggled group %S\C-.", Lith_AutoGroupNames[group]);
   }
   else
   {
      ACS_LocalAmbientSound("player/cbi/auto/invalid", 127);
      Lith_Log(p, "> No items to toggle in group %S\C-.", Lith_AutoGroupNames[group]);
   }
}

// EOF

