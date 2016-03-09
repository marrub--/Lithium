#include "lith_common.h"
#include "lith_player.h"
#include "lith_list.h"
#include "lith_hudid.h"
#include "lith_hud.h"

// ---------------------------------------------------------------------------
// HUD Scripts.
//

[[__call("ScriptI")]]
void Lith_RenderHUDWaves(player_t *p)
{
   fixed health = (fixed)p->health / (fixed)p->maxhealth;
   int frame = min(health * 4.0, 4.0) + 1.0;
   int timer = ACS_Timer();
   int pos;
   
   ACS_SetHudSize(320, 200, false);
   
   // Sine (health)
   pos = (10 + timer) % 160;
   DrawSpriteFade(StrParam("H_D1%i", frame),
      hid_scope_sineS - pos,
      20.1 + roundk(ACS_Sin(pos / 32.0) * 7.0, 0),
      5.1 + pos,
      1.5, 0.3);
   
   // Square
   {
      fixed a = ACS_Cos(pos / 32.0);
      
      pos = (7 + timer) % 160;
      DrawSpriteFade(roundk(a, 2) != 0.0 ? "H_D16" : "H_D46",
         hid_scope_squareS - pos,
         20.1 + (a >= 0) * 7.0,
         5.1 + pos,
         1.9, 0.1);
   }
   
   ACS_SetActorProperty(0, APROP_RenderStyle, STYLE_Subtract);
   ACS_SetActorPropertyFixed(0, APROP_Alpha, ACS_GetCVarFixed("lith_weapons_scopealpha"));
   
   // Triangle
   pos = (5 + timer) % 160;
   DrawSpriteFade("H_D14", hid_scope_triS - pos, 20.1 + abs((pos % 16) - 8), 5.1 + pos, 1.2, 0.2);
}

[[__call("ScriptI")]]
void Lith_RenderHUDStringStack(player_t *p)
{
   size_t i = 0;
   
   if((ACS_Timer() % 5) == 0)
   {
      DList_InsertBack(p->hudstrstack, (listdata_t){ .str = StrParam("%.4X", Random(0x0000, 0x7FFF)) });
      DList_DeleteFront(p->hudstrstack);
   }
   
   ACS_SetHudSize(320, 200, false);
   ACS_SetFont("CONFONT");
   
   for(slist_t *rover = p->hudstrstack->head; rover; rover = rover->next, i++)
   {
      HudMessage("%S", rover->data.str);
      HudMessageParams(HUDMSG_ALPHA | HUDMSG_ADDBLEND,
         hid_scope_stringstackS - i,
         CR_RED,
         320.2,
         0.1 + (i * 9),
         0.0, 0.5);
   }
}

[[__call("ScriptI")]]
void Lith_PlayerHUD(player_t *p)
{
   static __str const weapongfx[weapon_max] = {
      [weapon_unknown]        = "H_D27",
      [weapon_pistol]         = "H_D24",
      [weapon_shotgun]        = "H_D23",
      [weapon_supershotgun]   = "H_D23",
      [weapon_combatrifle]    = "H_D22",
      [weapon_rocketlauncher] = "H_D21",
      [weapon_plasmarifle]    = "H_D25",
      [weapon_bfg9000]        = "H_D26"
   };
   
   static __str const armorgfx[armor_max] = {
      [armor_unknown] = "H_D27",
      [armor_none]    = "H_D28",
      [armor_bonus]   = "H_D23",
      [armor_green]   = "H_D24",
      [armor_blue]    = "H_D25"
   };
   
   int time            = ACS_Timer();
   int hid_healthbg_fx = hid_healthbg_fxS - (time % 32);
   int hid_armorbg_fx  = hid_armorbg_fxS  - (time % 32);
   int hid_slideind_fx = hid_slideind_fxS - (time % 11);
   
   ACS_SetHudSize(320, 200, false);
   
   // ACK
   if(ACS_PlayerCount() > 1 && ACS_GetCVar("lith_sv_ack"))
   {
      HudMessageF("SMALLFNT", "Out of sync with: ZDoom");
      HudMessageParams(HUDMSG_PLAIN, hid_ack, CR_GREEN, 0.5, 0.0, 0.1);
   }
   
   // Jet
   if(p->rocketcharge != rocketcharge_max)
   {
      fixed rocket = p->rocketcharge / (fixed)rocketcharge_max;
      int max = (hid_jetS - hid_jetE) * rocket;
      
      DrawSpriteFade("H_B3", hid_jetbg, 320.2, 80.1, 0.0, 0.5);
      HudMessageF("SMALLFNT", "J\nE\nT");
      HudMessageParams(HUDMSG_FADEOUT, hid_jettext, CR_RED, 305.2, 150.2, 0.1, 0.5);
      
      for(int i = 0; i < max; i++)
      {
         DrawSprite(max < 4 ? "H_C2" : "H_C1",
            HUDMSG_FADEOUT | HUDMSG_ADDBLEND | HUDMSG_NOTWITHFULLMAP | HUDMSG_ALPHA,
            hid_jetS - i,
            320.2,
            150.1 - (i * 5),
            0.1, 0.5, 0.5);
      }
   }
   
   // Weapon Display
   if(ACS_GetCVar("lith_hud_showweapons"))
   {
      HudMessageF("SMALLFNT", "Weapons");
      HudMessageParams(HUDMSG_ALPHA, hid_weapontext, CR_LIGHTBLUE, 80.1, 192.2, 0.1, 0.3);
      DrawSpritePlain("H_W1", hid_weaponbg, 80.1, 200.2, 0.1);
      
      for(int i = 0; i < 8; i++)
         if(p->weapons & (1 << i))
         {
            fixed x = (10 * i) + 80.1;
            fixed y = 200.2;
            DrawSpritePlain("H_W2", hid_weapon1 + i, x, y, 0.1);
            HudMessageF("SMALLFNT", "%i", i + 1);
            HudMessageParams(HUDMSG_PLAIN, hid_weapon1text + i, CR_BLUE, x + 5, y - 2, 0.1);
         }
   }
   
   // Ammo
   if(p->weapontype == weapon_pistol || p->weapontype == weapon_supershotgun)
   {
      int count;
      
      if(p->weapontype == weapon_pistol)
         count = 7 - ACS_CheckInventory("Lith_PistolShotsFired");
      else if(p->weapontype == weapon_supershotgun)
         count = ACS_CheckInventory("Lith_SuperShotgunCharge");
      
      DrawSpritePlain("H_B2", hid_ammobg, 320.2, 200.2, 0.1);
      
      HudMessageF("BIGFONT", "%i", count);
      HudMessageParams(HUDMSG_PLAIN, hid_ammo, CR_RED, 318.2, 200.2, 0.1);
   }
   else if(p->weapontype == weapon_combatrifle)
   {
      DrawSpritePlain("H_W3", hid_ammobg, 320.2, 200.2, 0.1);
      DrawSpritePlain(StrParam("H_W%i", (rifle_firemode_max - p->riflefiremode) + 3),
         hid_ammo,
         320.2,
         168.2 + (p->riflefiremode * 16),
         0.1);
   }
   
   // Health
   DrawSpritePlain(p->berserk ? "H_B4" : "H_B1", hid_healthbg, 0.1, 200.2, 0.0);
   
   HudMessageF("BIGFONT", p->health < 0 ? "---" : "%i", p->health);
   HudMessageParams(HUDMSG_PLAIN, hid_health, CR_RED, 2.1, 200.2, 0.0);
   
   if(p->health > 0 && p->health < p->prevhealth)
   {
      fixed ft = minmax((p->prevhealth - p->health) / 30.0, 0.1, 3.0);
      HudMessageF("BIGFONT", "%i", p->health);
      HudMessageParams(HUDMSG_FADEOUT, hid_healthhit, CR_YELLOW, 2.1, 200.2, 0.1, ft);
   }
   else if(p->health > p->prevhealth)
   {
      HudMessageF("BIGFONT", "%i", p->health);
      HudMessageParams(HUDMSG_FADEOUT, hid_healthhit, CR_PURPLE, 2.1, 200.2, 0.1, 0.2);
   }
   
   // Weapon indicator
   {
      int time78 = time % 78;
      DrawSpriteFade(weapongfx[p->weapontype],
         hid_healthbg_fx,
         77.1 - time78,
         187.1 + (time78 < 11 ? (11 - (time78 % 12)) : 0),
         0.2,
         0.7);
   }
   
   // Slide indicator
   {
      int time11 = time % 11;
      float slide = p->slidecharge / (float)slidecharge_max;
      
      DrawSprite(slide != 1.0f ? "H_D21" : "H_D24",
         HUDMSG_FADEOUT | HUDMSG_ALPHA,
         hid_slideind_fx,
         77.1 - time11,
         188.1 + (11 - time11),
         (fixed)(0.3f * slide),
         (fixed)(0.6f * slide),
         0.8);
   }
   
   // Armor
   DrawSpritePlain("H_B1", hid_armorbg, 0.1, 188.2, 0.0);
   
   HudMessageF("BIGFONT", "%i", p->armor);
   HudMessageParams(HUDMSG_PLAIN, hid_armor, CR_GREEN, 2.1, 188.2, 0.0);
   
   if(p->armor < p->prevarmor)
   {
      fixed ft = minmax((p->prevarmor - p->armor) / 30.0, 0.1, 3.0);
      HudMessageF("BIGFONT", "%i", p->armor);
      HudMessageParams(HUDMSG_FADEOUT, hid_armorhit, CR_YELLOW, 2.1, 188.2, 0.1, ft);
   }
   else if(p->armor > p->prevarmor)
   {
      HudMessageF("BIGFONT", "%i", p->armor);
      HudMessageParams(HUDMSG_FADEOUT, hid_armorhit, CR_PURPLE, 2.1, 188.2, 0.1, 0.2);
   }
   
   // Armor indicator
   {
      int time78 = (8 + time) % 78;
      DrawSpriteFade(armorgfx[p->armortype],
         hid_armorbg_fx,
         77.1 - time78,
         175.1 + (time78 < 11 ? (11 - (time78 % 12)) : 0),
         0.2,
         0.7);
   }
   
   // Score
   if(ACS_GetCVar("lith_hud_showscore"))
   {
      HudMessageF("SMALLFNT", "Score");
      HudMessageParams(HUDMSG_ALPHA, hid_scorelabel, CR_LIGHTBLUE, 320.2, 18.1, 0.1, 0.3);
      
      HudMessageF("SMALLFNT", "%lli", p->score);
      HudMessageParams(HUDMSG_PLAIN, hid_score, CR_WHITE, 320.2, 22.1, 0.1);
      
      if(p->score > p->prevscore)
      {
         HudMessageF("SMALLFNT", "%lli", p->score);
         HudMessageParams(HUDMSG_FADEOUT, hid_scorehit, CR_ORANGE, 320.2, 22.1, 0.1, 0.2);
      }
      else if(p->score < p->prevscore)
      {
         fixed ft = minmax((p->prevscore - p->score) / 3000.0, 0.1, 3.0);
         HudMessageF("SMALLFNT", "%lli", p->score);
         HudMessageParams(HUDMSG_FADEOUT, hid_scorehit, CR_YELLOW, 320.2, 22.1, 0.1, ft);
      }
      
      if(p->scoreaccumtime > 0)
      {
         HudMessageF("SMALLFNT", "%+lli", p->scoreaccum);
         HudMessageParams(HUDMSG_FADEOUT, hid_scoreaccum, CR_WHITE, 320.2, 30.1, 0.1, 0.4);
      }
      else if(p->scoreaccumtime < 0)
      {
         HudMessageF("SMALLFNT", "%lli", p->score);
         HudMessageParams(HUDMSG_FADEOUT, hid_scorehit, CR_PURPLE, 320.2, 22.1, 0.1, 0.2);
      }
   }
   
   // Keys
   if(p->keys & key_red)
      DrawSpritePlain("H_RKEY", hid_key_red, 0.1, 140.1, 0.1);
   if(p->keys & key_yellow)
      DrawSpritePlain("H_YKEY", hid_key_yellow, 0.1, 148.1, 0.1);
   if(p->keys & key_blue)
      DrawSpritePlain("H_BKEY", hid_key_blue, 0.1, 156.1, 0.1);
}


//
// ---------------------------------------------------------------------------

