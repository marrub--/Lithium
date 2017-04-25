#include "lith_upgrades_common.h"


//----------------------------------------------------------------------------
// Static Functions
//

//
// HUD_Weapons
//
static void HUD_Weapons(player_t *p)
{
   DrawSpritePlain("lgfx/HUD_C/Bar.png", hid_weaponbg, 320.2, 181.2, TICSECOND);
   
   for(int i = 1; i < SLOT_MAX; i++)
      if(p->weapon.slot[i])
   {
      fixed x = 323.2 - (8 * (SLOT_MAX - i));
      fixed y = 180.2;
      
      HudMessageF("LHUDFONTSMALL", "%i", i);
      HudMessageParams(0, hid_weapontextE + i, CR_DARKRED, x, y, TICSECOND);
      
      if(p->weapon.cur->info->slot == i)
      {
         HudMessageF("LHUDFONTSMALL", "\Cg%i", i);
         HudMessageFade(hid_weaponcurE + i, x, y, TICSECOND, 0.2);
      }
   }
}

//
// HUD_Ammo
//
static void HUD_Ammo(player_t *p)
{
   invweapon_t  const *wep  = p->weapon.cur;
   weaponinfo_t const *info = wep->info;
   
   __str typegfx;
   
   if(wep->ammotype == AT_Mag)
   {
      int max = ACS_GetMaxInventory(0, wep->ammoclass);
      int cur = ACS_CheckInventory(wep->ammoclass);
      
      typegfx = "lgfx/HUD_C/MAG.png";
      
      HudMessageF("LHUDFONT", "%i/%i", max - cur, max);
   }
   else if(wep->ammotype == AT_Ammo)
   {
      typegfx = "lgfx/HUD_C/AMMO.png";
      
      HudMessageF("LHUDFONT", "%i", ACS_CheckInventory(wep->ammoclass));
   }
   else
      return;
   
   HudMessageParams(0, hid_ammo, CR_DARKRED, 242.1, 190.0, TICSECOND);
   
   DrawSpritePlain("lgfx/HUD_C/SplitFront.png", hid_ammobg, 320.2, 199.2, TICSECOND);
   DrawSpritePlain(typegfx, hid_ammotype, 316.2, 196.2, TICSECOND);
   
   if(p->weapontype == weapon_c_smg)
   {
      DrawSpritePlain("lgfx/HUD_C/BarSmall.png", hid_rifleheatbg, 320.2, 171.2, TICSECOND);
      ACS_SetHudClipRect(320-63, 171-9, (ACS_CheckInventory("Lith_SMGHeat")/500.k) * 63, 9);
      DrawSpritePlain("lgfx/HUD_C/HeatBar.png", hid_rifleheat, 320.2, 171.2, TICSECOND);
      ACS_SetHudClipRect(0, 0, 0, 0);
   }
}

//
// HUD_Health
//
static void HUD_Health(player_t *p)
{
   DrawSpritePlain("lgfx/HUD_C/SplitBack.png", hid_healthbg, 0.1, 200.2, TICSECOND);
   
   if(p->dead) HudMessageF("LHUDFONT", "---");
   else        HudMessageF("LHUDFONT", "%i", p->health);
   HudMessageParams(0, hid_health, CR_DARKRED, 21.1, 190.0, TICSECOND);
   
   DrawSpritePlain("lgfx/HUD_C/VIT.png", hid_healthtxt, 2.1, 198.2, TICSECOND);
}

//
// HUD_Armor
//
static void HUD_Armor(player_t *p)
{
   DrawSpritePlain("lgfx/HUD_C/SplitBack.png", hid_armorbg, 0.1, 182.2, TICSECOND);
   
   HudMessageF("LHUDFONT", "%i", p->armor);
   HudMessageParams(0, hid_armor, CR_DARKRED, 21.1, 172.0, TICSECOND);
   
   DrawSpritePlain("lgfx/HUD_C/ARM.png", hid_armortxt, 3.1, 178.2, TICSECOND);
}

//
// HUD_Score
//
static void HUD_Score(player_t *p)
{
   HudMessageF("CNFONT", "\Ca%S\Cnscr", Lith_ScoreSep(p->score));
   HudMessageParams(HUDMSG_PLAIN, hid_score, CR_WHITE, 160.0, 3.1, 0.1);
}

//
// HUD_KeyInd
//
static void HUD_KeyInd(player_t *p)
{
   if(p->keys.redskull)    DrawSpriteAlpha("H_KS1", hid_key_redskull,    180.2, 11.1, 0.1, 0.8);
   if(p->keys.yellowskull) DrawSpriteAlpha("H_KS2", hid_key_yellowskull, 170.2, 11.1, 0.1, 0.8);
   if(p->keys.blueskull)   DrawSpriteAlpha("H_KS3", hid_key_blueskull,   160.2, 10.1, 0.1, 0.8);
   if(p->keys.redcard)     DrawSpriteAlpha("H_KC1", hid_key_red,         150.2, 10.1, 0.1, 0.8);
   if(p->keys.yellowcard)  DrawSpriteAlpha("H_KC2", hid_key_yellow,      140.2, 11.1, 0.1, 0.8);
   if(p->keys.bluecard)    DrawSpriteAlpha("H_KC3", hid_key_blue,        130.2, 11.1, 0.1, 0.8);
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Activate
//
void Upgr_HeadsUpDis2_Activate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = true;
}

//
// Disable
//
void Upgr_HeadsUpDis2_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = false;
}

//
// Render
//
void Upgr_HeadsUpDis2_Render(player_t *p, upgrade_t *upgr)
{
   // Log
   Lith_HUD_Log(p);
   
   // Inventory
   HUD_KeyInd(p);
   
   if(Lith_GetPCVarInt(p, "lith_hud_showscore"))
      HUD_Score(p);
   if(Lith_GetPCVarInt(p, "lith_hud_showweapons"))
      HUD_Weapons(p);
   
   // Status
   HUD_Ammo(p);
   HUD_Health(p);
   HUD_Armor(p);
}

// EOF

