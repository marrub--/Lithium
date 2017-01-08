#include "lith_common.h"
#include "lith_player.h"
#include "lith_list.h"
#include "lith_hudid.h"
#include "lith_hud.h"

#include <stdio.h>

#define LOG_TIME 140


//----------------------------------------------------------------------------
// Static Functions
//

[[__call("ScriptS")]] static void HUD_Log(player_t *p);
[[__call("ScriptS")]] static void HUD_StringStack(player_t *p);
[[__call("ScriptS")]] static void HUD_Waves(player_t *p);
static void HUD_Scope(player_t *p);
static void HUD_Jet(player_t *p);
static void HUD_Weapons(player_t *p);
static void HUD_Ammo(player_t *p);
static void HUD_Health(player_t *p);
static void HUD_SlideInd(player_t *p, int time, int hid);
static void HUD_Armor(player_t *p);
static void HUD_IndicatorBar(player_t *p, int time, __str image, int hid, int yadd);
static void HUD_Score(player_t *p);
static void HUD_KeyInd(player_t *p);


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_PlayerHUD
//
[[__call("ScriptS")]]
void Lith_PlayerHUD(player_t *p)
{
   static __str weapongfx[weapon_max] = {
      [weapon_unknown]        = "H_D27",
      [weapon_pistol]         = "H_D24",
      [weapon_shotgun]        = "H_D23",
      [weapon_rifle]    = "H_D22",
      [weapon_launcher] = "H_D21",
      [weapon_plasma]    = "H_D25",
      [weapon_bfg]        = "H_D26"
   };
   
   static __str armorgfx[armor_max] = {
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
   
   ACS_SetHudSize(320, 200);
   
   HUD_Log(p);
   HUD_Scope(p);
   HUD_Jet(p);
   HUD_Weapons(p);
   HUD_Ammo(p);
   HUD_Health(p);
   HUD_IndicatorBar(p, time, weapongfx[p->weapontype], hid_healthbg_fx, 12);
   HUD_SlideInd(p, time, hid_slideind_fx);
   HUD_Armor(p);
   HUD_IndicatorBar(p, time, armorgfx[p->armortype], hid_armorbg_fx, 0);
   HUD_Score(p);
   HUD_KeyInd(p);
}

//
// Lith_Log
//
void Lith_Log(player_t *p, __str fmt, ...)
{
   logdata_t *logdata = null;
   
   for(int i = 0; i < LOG_MAX; i++)
      if(p->logdata[i].time == 0)
         logdata = &p->logdata[i];
   
   if(!logdata)
   {
      logdata = p->log->head->data.vp;
      DList_DeleteFront(p->log);
   }
   
   va_list vl;
   
   ACS_BeginPrint(); va_start(vl, fmt); __vnprintf_str(fmt, vl); va_end(vl);
   logdata->info = ACS_EndStrParam();
   logdata->time = LOG_TIME;
   
   DList_InsertBack(p->log, (listdata_t){.vp = logdata});
}


//----------------------------------------------------------------------------
// Static Functions
//

//
// HUD_Log
//
[[__call("ScriptS")]]
static void HUD_Log(player_t *p)
{
   int i = 0;
   ACS_SetHudSize(480, 300);
   for(slist_t *rover = p->log->head; rover;)
   {
      logdata_t *logdata = rover->data.vp;
      
      if(logdata->time == 0)
      {
         slist_t *next = rover->next;
         DList_Remove(p->log, rover);
         rover = next;
         continue;
      }
      else logdata->time--;
      
      HudMessageF("LOGFONT", "%S", logdata->info);
      HudMessageParams(HUDMSG_NOWRAP|HUDMSG_FADEOUT, hid_logE + i, CR_GREEN, 0.1, 262.2 - (10 * i), TICSECOND, 0.1);
      
      if(logdata->time > LOG_TIME - 10)
      {
         HudMessageF("LOGFONT", "%S", logdata->info);
         HudMessageParams(HUDMSG_NOWRAP|HUDMSG_FADEOUT|HUDMSG_ADDBLEND, hid_logAddE + i, CR_GREEN, 0.1, 262.2 - (10 * i), TICSECOND, 0.1);
      }
      
      rover = rover->next;
      i++;
   }
}

//
// HUD_StringStack
//
[[__call("ScriptS")]]
static void HUD_StringStack(player_t *p)
{
   size_t i = 0;
   
   if((ACS_Timer() % 3) == 0)
   {
      DList_InsertBack(p->hudstrstack, (listdata_t){ .str = StrParam("%.4X", Random(0x0000, 0x7FFF)) });
      DList_DeleteFront(p->hudstrstack);
   }
   
   ACS_SetHudSize(320, 200);
   ACS_SetFont("CONFONT");
   
   for(slist_t *rover = p->hudstrstack->head; rover; rover = rover->next, i++)
   {
      HudMessage("%S", rover->data.str);
      HudMessageParams(HUDMSG_ALPHA | HUDMSG_ADDBLEND, hid_scope_stringstackS - i, CR_RED, 320.2, 0.1 + (i * 9), 0.0, 0.5);
   }
}

//
// HUD_Waves
//
[[__call("ScriptS")]]
static void HUD_Waves(player_t *p)
{
   fixed health = (fixed)p->health / (fixed)p->maxhealth;
   int frame = minmax(health * 4, 1, 5);
   int timer = ACS_Timer();
   int pos;
   
   ACS_SetHudSize(320, 200);
   
   // Sine (health)
   pos = (10 + timer) % 160;
   DrawSpriteFade(StrParam("H_D1%i", frame),
      hid_scope_sineS - pos,
      300.1 + roundk(sink(pos / 32.0) * 7.0, 0),
      5.1 + pos,
      1.5, 0.3);
   
   // Square
   {
      fixed a = cosk(pos / 32.0);
      
      pos = (7 + timer) % 160;
      DrawSpriteFade(roundk(a, 2) != 0.0 ? "H_D16" : "H_D46",
         hid_scope_squareS - pos,
         300.1 + (a >= 0) * 7.0,
         5.1 + pos,
         1.9, 0.1);
   }
   
   ACS_SetActorProperty(0, APROP_RenderStyle, STYLE_Subtract);
   ACS_SetActorPropertyFixed(0, APROP_Alpha, ACS_GetUserCVarFixed(p->number, "lith_weapons_scopealpha"));
   
   // Triangle
   pos = (5 + timer) % 160;
   DrawSpriteFade("H_D14", hid_scope_triS - pos, 300.1 + abs((pos % 16) - 8), 5.1 + pos, 1.2, 0.2);
}

//
// HUD_Scope
//
static void HUD_Scope(player_t *p)
{
   if(p->old.scopetoken && !p->scopetoken)
   {
      if(p->hudstrstack)
      {
         DList_Free(p->hudstrstack);
         p->hudstrstack = null;
      }
      
      for(int i = hid_scope_clearS; i <= hid_scope_clearE; i++)
      {
         HudMessage("");
         HudMessagePlain(i, 0.0, 0.0, 0.0);
      }
   }
   else if(p->scopetoken && !p->old.scopetoken)
   {
      p->hudstrstack = DList_Create();
      
      for(int i = 0; i < hudstrstack_max; i++)
         DList_InsertBack(p->hudstrstack, (listdata_t){
            .str = StrParam("%x", Random(0x1000, 0x7FFF))
         });
   }
   
   if(p->scopetoken)
   {
      HUD_Waves(p);
      HUD_StringStack(p);
      
      ACS_SetActorProperty(0, APROP_RenderStyle, STYLE_Subtract);
      ACS_SetActorPropertyFixed(0, APROP_Alpha, ACS_GetUserCVarFixed(p->number, "lith_weapons_scopealpha"));
   }
   else
   {
      ACS_SetActorProperty(0, APROP_RenderStyle, STYLE_Translucent);
      ACS_SetActorPropertyFixed(0, APROP_Alpha, ACS_GetUserCVarFixed(p->number, "lith_weapons_alpha"));
   }
}

//
// HUD_Jet
//
static void HUD_Jet(player_t *p)
{
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
}

//
// HUD_Weapons
//
static void HUD_Weapons(player_t *p)
{
   DrawSpritePlain("H_W1", hid_weaponbg, 80.1, 200.2, 0.1);
   
   if(ACS_GetUserCVar(p->number, "lith_hud_showweapons"))
      for(int i = weapon_min; i < weapon_max; i++)
         if(p->weapons & (1 << i))
   {
      fixed x = (10 * (i - weapon_min)) + 80.1;
      fixed y = 200.2;
      HudMessageF("INDEXFONT_DOOM", "%i", i);
      HudMessageParams(HUDMSG_PLAIN, hid_weapontextE + i, p->weapontype == i ? CR_YELLOW : CR_BLUE, x + 5, y - 2, 0.1);
   }
}

//
// HUD_Ammo
//
static void HUD_Ammo(player_t *p)
{
   switch(p->weapontype)
   {
   case weapon_pistol: case weapon_launcher: case weapon_plasma: case weapon_bfg:
   {
      int count;
      
           if(p->weapontype == weapon_pistol)   count = 7 - ACS_CheckInventory("Lith_PistolShotsFired");
      else if(p->weapontype == weapon_launcher) count =     ACS_CheckInventory("Lith_RocketAmmo");
      else if(p->weapontype == weapon_plasma)   count =     ACS_CheckInventory("Lith_PlasmaAmmo");
      else if(p->weapontype == weapon_bfg)      count =     ACS_CheckInventory("Lith_CannonAmmo");
      
      DrawSpritePlain("H_B2", hid_ammobg, 320.2, 200.2, 0.1);
      
      HudMessageF("BIGFONT", "%i", count);
      HudMessageParams(HUDMSG_PLAIN, hid_ammo, CR_RED, 318.2, 200.2, 0.1);
      break;
   }
   case weapon_rifle:
   {
      int addy = p->upgrades[UPGR_RifleModes].active ? 0 : 16;
      DrawSpritePlain("H_W3", hid_ammobg, 320.2, 200.2 + addy, 0.1);
      DrawSpritePlain(StrParam("H_W%i", (rifle_firemode_max - p->riflefiremode) + 3), hid_ammo, 320.2, 168.2 + (p->riflefiremode * 16) + addy, 0.1);
      break;
   }
   }
}

//
// HUD_Health
//
static void HUD_Health(player_t *p)
{
   DrawSpritePlain(p->berserk ? "H_B4" : "H_B1", hid_healthbg, 0.1, 200.2, 0.0);
   
   HudMessageF("BIGFONT", p->dead ? "---" : "%i", p->health);
   HudMessageParams(HUDMSG_PLAIN, hid_health, CR_RED, 2.1, 200.2, 0.0);
   
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
}

//
// HUD_SlideInd
//
static void HUD_SlideInd(player_t *p, int time, int hid)
{
   int time11 = time % 11;
   float slide = p->slidecharge / (float)slidecharge_max;
   
   DrawSprite(slide != 1.0f ? "H_D21" : "H_D24",
      HUDMSG_FADEOUT | HUDMSG_ALPHA,
      hid,
      77.1 - time11,
      188.1 + (11 - time11),
      (fixed)(0.3f * slide),
      (fixed)(0.6f * slide),
      0.8);
}

//
// HUD_Armor
//
static void HUD_Armor(player_t *p)
{
   DrawSpritePlain("H_B1", hid_armorbg, 0.1, 188.2, 0.0);
   
   HudMessageF("BIGFONT", "%i", p->armor);
   HudMessageParams(HUDMSG_PLAIN, hid_armor, CR_GREEN, 2.1, 188.2, 0.0);
   
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
}

//
// HUD_IndicatorBar
//
static void HUD_IndicatorBar(player_t *p, int time, __str image, int hid, int yadd)
{
   int pos = (8 + time) % 78;
   
   if(pos < 11) yadd += 11 - (pos % 12);
   
   DrawSpriteFade(image, hid, 77.1 - pos, 175.1 + yadd, 0.2, 0.7);
}

//
// HUD_Score
//
static void HUD_Score(player_t *p)
{
   if(ACS_GetUserCVar(p->number, "lith_hud_showscore"))
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
}

//
// HUD_KeyInd
//
static void HUD_KeyInd(player_t *p)
{
   if(p->keys.redskull)    DrawSpritePlain("H_KS1", hid_key_redskull, 8.1, 144.1, 0.1);
   if(p->keys.yellowskull) DrawSpritePlain("H_KS2", hid_key_yellowskull, 8.1, 152.1, 0.1);
   if(p->keys.blueskull)   DrawSpritePlain("H_KS3", hid_key_blueskull, 8.1, 160.1, 0.1);
   if(p->keys.redcard)     DrawSpritePlain("H_KC1", hid_key_red, 0.1, 140.1, 0.1);
   if(p->keys.yellowcard)  DrawSpritePlain("H_KC2", hid_key_yellow, 0.1, 148.1, 0.1);
   if(p->keys.bluecard)    DrawSpritePlain("H_KC3", hid_key_blue, 0.1, 156.1, 0.1);
}

// EOF

