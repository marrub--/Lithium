#include "lith_upgrades_common.h"


//----------------------------------------------------------------------------
// Static Functions
//

//
// HUD_IndicatorLine
//
static void HUD_IndicatorLine(player_t *p, int time, __str image, int hid, int yadd)
{
   int pos = (8 + time) % 78;
   
   if(pos < 11)
      yadd += 11 - (pos % 12);
   
   DrawSpriteFade(image, hid, 77.1 - pos, 175.1 + yadd, 0.2, 0.7);
}

//
// HUD_Weapons
//
static void HUD_Weapons(player_t *p)
{
   DrawSpritePlain("H_W1", hid_weaponbg, 80.1, 200.2, TICSECOND);
   
   for(int i = weapon_min; i < weapon_max; i++)
      if(p->weapons & (1 << i))
   {
      fixed x = (10 * (i - weapon_min)) + 80.1;
      fixed y = 200.2;
      HudMessageF("INDEXFONT_DOOM", "%i", i + 1);
      HudMessageParams(HUDMSG_PLAIN, hid_weapontextE + i, p->weapontype == i ? CR_YELLOW : CR_BLUE, x + 5, y - 2, TICSECOND);
   }
}

//
// HUD_Ammo
//
static void HUD_Ammo(player_t *p)
{
   __str ammotype = "H_A2";
   __str count;
   
   switch(p->weapontype)
   {
   case weapon_rifle:
   {
      int addy = p->upgrades[UPGR_RifleModes].active ? 0 : 16;
      DrawSpritePlain("H_W3", hid_riflemodebg, 241.2, 200.2 + addy, TICSECOND);
      DrawSpritePlain(StrParam("H_W%i", (rifle_firemode_max - p->riflefiremode) + 3), hid_riflemode, 241.2, 168.2 + (p->riflefiremode * 16) + addy, TICSECOND);
      goto load;
   }
   case weapon_shotgun:
      if(!p->upgrades[UPGR_GaussShotty].active) break;
   case weapon_pistol:
   load:
      ammotype = "H_A1";
   case weapon_launcher: case weapon_plasma: case weapon_bfg:
   {
      if(p->weapontype == weapon_pistol)
         count = StrParam("%i/7", 7 - ACS_CheckInventory("Lith_PistolShotsFired"));
      else if(p->weapontype == weapon_shotgun)
         count = StrParam("%i/10", 10 - ACS_CheckInventory("Lith_GaussShotsFired"));
      else if(p->weapontype == weapon_rifle)
         count = StrParam("%i/40", 40 - ACS_CheckInventory("Lith_RifleShotsFired"));
      else if(p->weapontype == weapon_launcher)
         count = StrParam("%i", ACS_CheckInventory("Lith_RocketAmmo"));
      else if(p->weapontype == weapon_plasma)
         count = StrParam("%i", ACS_CheckInventory("Lith_PlasmaAmmo"));
      else if(p->weapontype == weapon_bfg)
         count = StrParam("%i", ACS_CheckInventory("Lith_CannonAmmo"));
      
      DrawSpritePlain("H_B2", hid_ammobg, 320.2, 200.2, TICSECOND);
      
      HudMessageF("BIGFONT", "%S", count);
      HudMessageParams(HUDMSG_PLAIN, hid_ammo, CR_RED, 318.2, 200.2, TICSECOND);
      
      DrawSpritePlain(ammotype, hid_ammotype, 320.2, 200.2, TICSECOND);
      
      break;
   }
   }
}

//
// HUD_Health
//
static void HUD_Health(player_t *p)
{
   static __str weapongfx[weapon_max] = {
      [weapon_unknown]  = "H_D27",
      [weapon_pistol]   = "H_D24",
      [weapon_shotgun]  = "H_D23",
      [weapon_rifle]    = "H_D22",
      [weapon_launcher] = "H_D21",
      [weapon_plasma]   = "H_D25",
      [weapon_bfg]      = "H_D26"
   };
   
   DrawSpritePlain(p->berserk ? "H_B4" : "H_B1", hid_healthbg, 0.1, 200.2, TICSECOND);
   
   HudMessageF("BIGFONT", p->dead ? "---" : "%i", p->health);
   HudMessageParams(HUDMSG_PLAIN, hid_health, CR_RED, 2.1, 200.2, TICSECOND);
   
   if(!p->dead)
   {
      if(p->health < p->old.health)
      {
         fixed ft = minmax((p->old.health - p->health) / 30.0, 0.1, 3.0);
         HudMessageF("BIGFONT", "%i", p->health);
         HudMessageParams(HUDMSG_FADEOUT, hid_healthhit, CR_YELLOW, 2.1, 200.2, 0.1, ft);
      }
      else if(p->health > p->old.health)
      {
         HudMessageF("BIGFONT", "%i", p->health);
         HudMessageParams(HUDMSG_FADEOUT, hid_healthhit, CR_PURPLE, 2.1, 200.2, 0.1, 0.2);
      }
   }
   
   HUD_IndicatorLine(p, p->ticks, weapongfx[p->weapontype], hid_healthbg_fxS - (p->ticks % 32), 12);
}

//
// HUD_Armor
//
static void HUD_Armor(player_t *p)
{
   static __str armorgfx[armor_max] = {
      [armor_unknown] = "H_D27",
      [armor_none]    = "H_D28",
      [armor_bonus]   = "H_D23",
      [armor_green]   = "H_D24",
      [armor_blue]    = "H_D25"
   };
   
   DrawSpritePlain("H_B1", hid_armorbg, 0.1, 188.2, TICSECOND);
   
   HudMessageF("BIGFONT", "%i", p->armor);
   HudMessageParams(HUDMSG_PLAIN, hid_armor, CR_GREEN, 2.1, 188.2, TICSECOND);
   
   if(p->armor < p->old.armor)
   {
      fixed ft = minmax((p->old.armor - p->armor) / 30.0, 0.1, 3.0);
      HudMessageF("BIGFONT", "%i", p->armor);
      HudMessageParams(HUDMSG_FADEOUT, hid_armorhit, CR_YELLOW, 2.1, 188.2, 0.1, ft);
   }
   else if(p->armor > p->old.armor)
   {
      HudMessageF("BIGFONT", "%i", p->armor);
      HudMessageParams(HUDMSG_FADEOUT, hid_armorhit, CR_PURPLE, 2.1, 188.2, 0.1, 0.2);
   }
   
   HUD_IndicatorLine(p, p->ticks, armorgfx[p->armortype], hid_armorbg_fxS - (p->ticks % 32), 0);
}

//
// HUD_Score
//
static void HUD_Score(player_t *p)
{
   HudMessageF("CNFONT", "%lli\Cnscr", p->score);
   HudMessageParams(HUDMSG_PLAIN, hid_score, CR_WHITE, 320.2, 10.1, 0.1);
   
   if(p->score > p->old.score)
   {
      HudMessageF("CNFONT", "%lli\Cnscr", p->score);
      HudMessageParams(HUDMSG_FADEOUT, hid_scorehit, CR_ORANGE, 320.2, 10.1, 0.1, 0.2);
   }
   else if(p->score < p->old.score)
   {
      fixed ft = minmax((p->old.score - p->score) / 3000.0, 0.1, 3.0);
      HudMessageF("CNFONT", "%lli\Cnscr", p->score);
      HudMessageParams(HUDMSG_FADEOUT, hid_scorehit, CR_PURPLE, 320.2, 10.1, 0.1, ft);
   }
   
   if(p->scoreaccumtime > 0)
   {
      HudMessageF("CNFONT", "%+lli", p->scoreaccum);
      HudMessageParams(HUDMSG_FADEOUT, hid_scoreaccum, CR_WHITE, 320.2, 20.1, 0.1, 0.4);
   }
}

//
// HUD_KeyInd
//
static void HUD_KeyInd(player_t *p)
{
   if(p->keys.redskull)    DrawSpritePlain("H_KS1", hid_key_redskull,    8.1, 144.1, 0.1);
   if(p->keys.yellowskull) DrawSpritePlain("H_KS2", hid_key_yellowskull, 8.1, 152.1, 0.1);
   if(p->keys.blueskull)   DrawSpritePlain("H_KS3", hid_key_blueskull,   8.1, 160.1, 0.1);
   if(p->keys.redcard)     DrawSpritePlain("H_KC1", hid_key_red,         0.1, 140.1, 0.1);
   if(p->keys.yellowcard)  DrawSpritePlain("H_KC2", hid_key_yellow,      0.1, 148.1, 0.1);
   if(p->keys.bluecard)    DrawSpritePlain("H_KC3", hid_key_blue,        0.1, 156.1, 0.1);
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Render
//
void Upgr_HeadsUpDisp_Render(player_t *p, upgrade_t *upgr)
{
   // Log
   Lith_HUD_Log(p);
   
   // Inventory
   HUD_KeyInd(p);
   if(ACS_GetUserCVar(p->number, "lith_hud_showscore"))
      HUD_Score(p);
   if(ACS_GetUserCVar(p->number, "lith_hud_showweapons"))
      HUD_Weapons(p);
   
   // Status
   HUD_Ammo(p);
   HUD_Health(p);
   HUD_Armor(p);
}

// EOF

