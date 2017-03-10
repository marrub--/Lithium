#include "lith_common.h"
#include "lith_player.h"


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_CheckAutoBuy
//
void Lith_CheckAutoBuy(player_t *p)
{
   int total = 0;
   
   for(int i = 0; i < UPGR_MAX; i++)
      for(int j = 0; j < NUMAUTOGROUPS; j++)
   {
      upgrade_t *upgr = &p->upgrades[i];
      
      if(upgr->autogroups[j] && p->autobuy[j])
         if(Lith_UpgrBuy(p, &p->upgrades[i]))
         {  total++;
            Lith_UpgrToggle(p, &p->upgrades[i]);}
   }
   
   if(total)
   {
      ACS_LocalAmbientSound("player/cbi/auto/buy", 127);
      Lith_Log(p, "> Auto-bought \Cj%i\C- upgrade%S.", total, total != 1 ? "s" : "");
   }
}


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
         {  success++;
            Lith_UpgrToggle(p, &p->upgrades[i]);}}
   
   char color;
   
   if(success) {color = 'j'; ACS_LocalAmbientSound("player/cbi/auto/buy",     127);}
   else        {color = 'g'; ACS_LocalAmbientSound("player/cbi/auto/invalid", 127);}
   
   if(total)
      Lith_Log(p, "> Bought \C%c%i/%i\C- upgrade%S in group %S\C-.",
         color, success, total, total != 1 ? "s" : "", Lith_AutoGroupNames[group]);
   else
      Lith_LogH(p, "> No items to buy in group %S\C-.", Lith_AutoGroupNames[group]);
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
      Lith_LogH(p, "> Toggled group %S\C-.", Lith_AutoGroupNames[group]);
   }
   else
   {
      ACS_LocalAmbientSound("player/cbi/auto/invalid", 127);
      Lith_LogH(p, "> No items to toggle in group %S\C-.", Lith_AutoGroupNames[group]);
   }
}

// EOF

