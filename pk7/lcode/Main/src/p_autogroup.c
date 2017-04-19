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
   __str name;
   int total = 0;
   
   for(int i = 0; i < p->upgrmax; i++)
      for(int j = 0; j < NUMAUTOGROUPS; j++)
   {
      upgrade_t *upgr = &p->upgrades[i];
      
      if(upgr->autogroups[j] && p->autobuy[j] && Lith_UpgrBuy(p, upgr, true))
      {
         total++;
         name = Language("LITH_TXT_UPGRADE_TITLE_%S", upgr->info->name);
         upgr->toggle(p);
      }
   }
   
   if(total)
   {
      ACS_LocalAmbientSound("player/cbi/auto/buy", 127);
      
      if(total != 1)
         p->log("> Auto-bought \Cj%i\C- upgrades.", total);
      else
         p->log("> Auto-bought \Cj%S\C-.", name);
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
   if(ACS_Timer() < 10) return;

   if(group < 1 || group > NUMAUTOGROUPS) return;
   
   group -= 1;
   
   player_t *p = Lith_LocalPlayer;
   
   int success = 0, total = 0;
   for(int i = 0; i < p->upgrmax; i++)
      if(!p->upgrades[i].owned && p->upgrades[i].autogroups[group])
      {  total++;
         if(Lith_UpgrBuy(p, &p->upgrades[i], false))
         {  success++;
            p->upgrades[i].toggle(p);}}
   
   char color;
   
   if(success) {color = 'j'; ACS_LocalAmbientSound("player/cbi/auto/buy",     127);}
   else        {color = 'g'; ACS_LocalAmbientSound("player/cbi/auto/invalid", 127);}
   
   if(total)
      p->log("> Bought \C%c%i/%i\C- upgrade%S in group %S\C-.",
         color, success, total, total != 1 ? "s" : "", Lith_AutoGroupNames[group]);
   else
      p->logH("> No items to buy in group %S\C-.", Lith_AutoGroupNames[group]);
}

//
// Lith_KeyToggleAutoGroup
//
[[__call("ScriptS"), __extern("ACS"), __script("Net")]]
void Lith_KeyToggleAutoGroup(int group)
{
   if(ACS_Timer() < 10) return;

   if(group < 1 || group > NUMAUTOGROUPS) return;
   
   group -= 1;
   
   player_t *p = Lith_LocalPlayer;
   
   int total = 0;
   for(int i = 0; i < p->upgrmax; i++)
      if(p->upgrades[i].owned && p->upgrades[i].autogroups[group])
      {  total++;
         p->upgrades[i].toggle(p);}
   
   if(total)
   {
      ACS_LocalAmbientSound("player/cbi/auto/toggle", 127);
      p->logH("> Toggled group %S\C-.", Lith_AutoGroupNames[group]);
   }
   else
   {
      ACS_LocalAmbientSound("player/cbi/auto/invalid", 127);
      p->logH("> No items to toggle in group %S\C-.", Lith_AutoGroupNames[group]);
   }
}

// EOF

