#if 0
#include "lith_upgrades_common.h"

#define UData UData_HeadsUpDis3(upgr)

// Static Functions ----------------------------------------------------------|

//
// HUD_Ammo
//
static void HUD_Ammo(player_t *p, struct hud *h)
{
   HID(ammo1,      1);
   HID(ammo2,      1);
   HID(ammotype,   1);
   HID(ammotypebg, 1);
   HID(ammobg1,    1);
   HID(ammobg2,    1);

   invweapon_t const *wep = p->weapon.cur;

   __str typegfx = null;

   if(wep->ammotype & AT_NMag) {
      typegfx = "lgfx/HUD_I/MAG.png";
      HudMessageF("LHUDFONT", "\C[Lith_Purple]%i/%i",
         wep->magmax - wep->magcur, wep->magmax);
      HudMessageParams(HUDMSG_FADEOUT, ammo1, CR_PURPLE, 242.1, 218.0, TS, 0.35);
   }

   if(wep->ammotype & AT_Ammo && !(wep->info->flags & wf_magic))
   {
      int x = 0;

      if(wep->ammotype & AT_NMag) {
         DrawSpriteFade("lgfx/HUD_I/AmmoExtend.png", ammobg2, 242.2, 227.2, TS, 0.35);
         x = -58;
      }

      typegfx = "lgfx/HUD_I/AMMO.png";
      HudMessageF("LHUDFONT", "\C[Lith_Purple]%i", wep->ammocur);
      HudMessageParams(HUDMSG_FADEOUT, ammo2, CR_PURPLE, x+242.1, 218.0, TS, 0.35);
   }

   DrawSpritePlain("lgfx/HUD_I/AmmoWepsBack.png", ammobg1, 320.2, 229.2, TS);

   if(typegfx)
      DrawSpriteFade(typegfx, ammotype, 309, 219, TS, 0.25);
}

//
// HUD_HealthArmor
//
static void HUD_HealthArmor(player_t *p, struct hud *h, upgrade_t *upgr)
{
   HID(hp, 1);

   HID(arm,    1);
   HID(armbg,  1);
   HID(armfxE, 42);

   static __str const armorgfx[armor_max] = {
      [armor_unknown] = "lgfx/HUD/H_D27.png",
      [armor_none]    = "lgfx/HUD/H_D28.png",
      [armor_bonus]   = "lgfx/HUD/H_D23.png",
      [armor_green]   = "lgfx/HUD/H_D24.png",
      [armor_blue]    = "lgfx/HUD/H_D25.png"
   };

   DrawSpritePlain("lgfx/HUD_I/HPAPBack.png", armbg, 0.1, 230.2, TS);

   int health = (UData.healthi = lerpk(UData.healthi, p->health, 0.2)) + 0.5;
   if(p->dead) HudMessageF("LHUDFONT", "[Disabled]");
   else        HudMessageF("LHUDFONT", "\C[Lith_Purple]%i", health);
   HudMessageParams(0, hp, CR_PURPLE, 21.1, 202.0, TS);

   int armor = (UData.armori = lerpk(UData.armori, p->armor, 0.2)) + 0.5;
   HudMessageF("LHUDFONT", "\C[Lith_Purple]%i", armor);
   HudMessageParams(0, arm, CR_PURPLE, 21.1, 220.0, TS);

   DrawSpriteFade(armorgfx[p->armortype], armfxE + (p->ticks % 42), 20.1 + p->ticks % 42, 211.1, 0.2, 0.7);
}

// Extern Functions ----------------------------------------------------------|

//
// Activate
//
void Upgr_HeadsUpDis3_Activate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = true;

   p->hud.p   = p;
   p->hud.beg = hid_base_hud;
   p->hud.end = hid_end_hud;
}

//
// Disable
//
void Upgr_HeadsUpDis3_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = false;

   Lith_HUD_Clear(&p->hud);
}

//
// Render
//
void Upgr_HeadsUpDis3_Render(player_t *p, upgrade_t *upgr)
{
   struct hud *h = &p->hud;

   Lith_HUD_Begin(h);

   Lith_HUD_Log(h, CR_LIGHTBLUE, 0, -15);
   Lith_HUD_KeyInd(h, 20, 20, false, 0.8);

   Lith_HUD_WeaponSlots(h, CR_DARKGRAY, CR_GRAY, CR_WHITE, "g", 323, 208);

   score_t score;

   if(p->score < 0x20000000000000LL)
   {
      #pragma GDCC FIXED_LITERAL OFF
      score = (UData.scorei = lerp(UData.scorei, p->score, 0.3)) + 0.5;
   }
   else
      score = p->score;

   Lith_HUD_Score(h, "%S \CnScore", score, "CHFONT", "[Lith_Purple]", 2.1, 3.1);

   // Status
   HUD_Ammo(p, h);
   HUD_HealthArmor(p, h, upgr);

   Lith_HUD_End(h);
}
#endif

// EOF
