#include "lith_upgrades_common.h"


//----------------------------------------------------------------------------
// Static Functions
//

//
// HUD_Ammo
//
static void HUD_Ammo(player_t *p)
{
   invweapon_t  const *wep  = p->weapon.cur;
   weaponinfo_t const *info = wep->info;

   __str typegfx = null;

   if(wep->ammotype & AT_NMag)
   {
      int max = ACS_GetMaxInventory(0, wep->magclass);
      int cur = ACS_CheckInventory(wep->magclass);

      typegfx = "lgfx/HUD_I/Mag.png";
      HudMessageF("LHUDFONT", "\C[Lith_Purple]%i/%i", max - cur, max);
      HudMessageParams(HUDMSG_FADEOUT, hid_ammo1, CR_PURPLE, 242.1, 188.0, TICSECOND, 0.35);
   }

   if(wep->ammotype & AT_Ammo && !(wep->info->flags & wf_magic))
   {
      int x = 0;

      if(wep->ammotype & AT_NMag) {
         DrawSpriteFade("lgfx/HUD_I/AmmoExtend.png", hid_ammobg2, 242.2, 197.2, TICSECOND, 0.35);
         x = -58;
      }

      typegfx = "lgfx/HUD_I/Ammo.png";
      HudMessageF("LHUDFONT", "\C[Lith_Purple]%i", ACS_CheckInventory(wep->ammoclass));
      HudMessageParams(HUDMSG_FADEOUT, hid_ammo2, CR_PURPLE, x+242.1, 188.0, TICSECOND, 0.35);
   }

   static int const ncolor[] = {
      CR_DARKGRAY,
      CR_GRAY,
      CR_WHITE
   };

   Lith_HUD_DrawWeaponSlots(p, ncolor, countof(ncolor), 'g', 323, 178);
   DrawSpritePlain("lgfx/HUD_I/AmmoWepsBack.png", hid_ammobg1, 320.2, 199.2, TICSECOND);

   if(typegfx)
      DrawSpriteFade(typegfx, hid_ammotype, 309, 189, TICSECOND, 0.25);
}

//
// HUD_HealthArmor
//
static void HUD_HealthArmor(player_t *p)
{
   static __str const armorgfx[ARM_max] = {
      [ARM_unknown] = "lgfx/HUD/H_D27.png",
      [ARM_none]    = "lgfx/HUD/H_D28.png",
      [ARM_bonus]   = "lgfx/HUD/H_D23.png",
      [ARM_green]   = "lgfx/HUD/H_D24.png",
      [ARM_blue]    = "lgfx/HUD/H_D25.png"
   };

   DrawSpritePlain("lgfx/HUD_I/HPAPBack.png", hid_armorbg, 0.1, 200.2, TICSECOND);

   if(p->dead) HudMessageF("LHUDFONT", "[Disabled]");
   else        HudMessageF("LHUDFONT", "\C[Lith_Purple]%i", p->health);
   HudMessageParams(0, hid_health, CR_PURPLE, 21.1, 172.0, TICSECOND);

   HudMessageF("LHUDFONT", "\C[Lith_Purple]%i", p->armor);
   HudMessageParams(0, hid_armor, CR_PURPLE, 21.1, 190.0, TICSECOND);

   DrawSpriteFade(armorgfx[p->armortype], hid_armorbg_fxS - (p->ticks % 42), 20.1 + p->ticks % 42, 181.1, 0.2, 0.7);
}

//
// HUD_Score
//
static void HUD_Score(player_t *p)
{
   HudMessageF("CHFONT", "\C[Lith_Purple]%S \CnScore", Lith_ScoreSep(p->score));
   HudMessageParams(HUDMSG_PLAIN, hid_score, CR_WHITE, 2.1, 3.1, 0.1);
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
void Upgr_HeadsUpDis3_Activate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = true;
}

//
// Disable
//
void Upgr_HeadsUpDis3_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = false;
}

//
// Render
//
void Upgr_HeadsUpDis3_Render(player_t *p, upgrade_t *upgr)
{
   // Log
   Lith_HUD_Log(p);

   // Inventory
   HUD_KeyInd(p);

   if(p->getCVarI("lith_hud_showscore"))
      HUD_Score(p);

   // Status
   HUD_Ammo(p);
   HUD_HealthArmor(p);
}

// EOF

