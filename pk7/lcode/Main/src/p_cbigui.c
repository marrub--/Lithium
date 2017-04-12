#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"


//----------------------------------------------------------------------------
// Static Functions
//

static void Lith_CBITab_CBI(gui_state_t *g, player_t *p)
{
   __str name;
   int ram;
   
   if(p->cbi.hasUpgr2)
   {
      name = "KSKK Spec. BC-0265 Super High-Grade CPU";
      ram  = 150;
      
      DrawSpritePlain("lgfx/UI/CPU1.png", g->hid--, .1, .1, TICSECOND);
   }
   else if(p->cbi.hasUpgr1)
   {
      name = "KSKK Spec. Z6808 High-Grade CPU";
      ram  = 100;
      
      DrawSpritePlain("lgfx/UI/CPU2.png", g->hid--, .1, .1, TICSECOND);
   }
   else
   {
      name = "OFMD Spec. Nodea 541 Basic CPU";
      ram  = 50;
      
      DrawSpritePlain("lgfx/UI/CPU3.png", g->hid--, .1, .1, TICSECOND);
   }
   
   HudMessageF("CBIFONT", "%S", name);
   HudMessagePlain(g->hid--, 20.1, 60.1, TICSECOND);
   
   {
   int y = 70;
   #define Info(...) \
      HudMessageF("CBIFONT", __VA_ARGS__); \
      HudMessagePlain(g->hid--, 23.1, y + .1, TICSECOND); \
      y += 10
   
   Info("Performance: %i\CbPr", p->cbi.perf);
   Info("In use: %i\CbPr", p->cbi.pruse);
   Info("RAM: %iTiB", ram);
   
   y += 20;
   
   if(p->cbi.hasArmorInter) Info("Has Armor Interface");
   if(p->cbi.hasWeapnInter) Info("Has Weapon Modification Device");
   if(p->cbi.hasWeapnInte2) Info("Has Weapon Refactoring Device");
   if(p->cbi.hasRDistInter) Info("Has Reality Distortion Interface");
   
   #undef Info
   }
   
   if(p->cbi.hasArmorInter) DrawSpritePlain("lgfx/UI/ArmorInter.png", g->hid--, 300.2, 48*1 + .1 - 20, TICSECOND);
   if(p->cbi.hasWeapnInter) DrawSpritePlain("lgfx/UI/WeapnInter.png", g->hid--, 300.2, 48*2 + .1 - 20, TICSECOND);
   if(p->cbi.hasWeapnInte2) DrawSpritePlain("lgfx/UI/WeapnInte2.png", g->hid--, 300.2, 48*3 + .1 - 20, TICSECOND);
   if(p->cbi.hasRDistInter) DrawSpritePlain("lgfx/UI/RDistInter.png", g->hid--, 300.2, 48*4 + .1 - 20, TICSECOND);
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_PlayerUpdateCBIGUI
//
[[__call("ScriptS")]]
void Lith_PlayerUpdateCBIGUI(player_t *p)
{
   gui_state_t *g = &p->cbi.guistate;
   
   p->cbi.theme = Lith_GetPCVarInt(p, "lith_gui_theme");
   
   if(p->cbi.theme != p->cbi.oldtheme)
   {
      switch((p->cbi.oldtheme = p->cbi.theme))
      {
      default:
      case cbi_theme_green:  p->cbi.guistate.gfxprefix = "lgfx/UI_Green/";  break;
      case cbi_theme_rose:   p->cbi.guistate.gfxprefix = "lgfx/UI_Rose/";   break;
      case cbi_theme_umi:    p->cbi.guistate.gfxprefix = "lgfx/UI_Umi/";    break;
      case cbi_theme_ender:  p->cbi.guistate.gfxprefix = "lgfx/UI_Ender/";  break;
      case cbi_theme_orange: p->cbi.guistate.gfxprefix = "lgfx/UI_Orange/"; break;
      case cbi_theme_grey:   p->cbi.guistate.gfxprefix = "lgfx/UI_Grey/";   break;
      }
   }
   
   Lith_GUI_Begin(g, hid_end_cbi, 320, 240);
   Lith_GUI_UpdateState(g, p);
   
   DrawSpriteAlpha(StrParam("%SBackground.png", g->gfxprefix), g->hid--, 0.1, 0.1, TICSECOND, 0.7);
   
   if(Lith_GUI_Button(g, .x = 296, 13, .preset = &btnexit))
      p->useGUI(GUI_CBI);
   
   static __str tabnames[cbi_tab_max] = {"Upgrades", "CBI", "Shop", "Info", "Settings", "Log"};
   for(int i = 0; i < cbi_tab_max; i++)
      if(Lith_GUI_Button_Id(g, i, tabnames[i], btntab.w * i + 13, 13, i == g->st[st_maintab].i, .preset = &btntab))
         g->st[st_maintab].i = i;
   
   extern void Lith_CBITab_Upgrades(gui_state_t *g, player_t *p);
   extern void Lith_CBITab_Shop    (gui_state_t *g, player_t *p);
   extern void Lith_CBITab_BIP     (gui_state_t *g, player_t *p);
   extern void Lith_CBITab_Settings(gui_state_t *g, player_t *p);
   extern void Lith_CBITab_Log     (gui_state_t *g, player_t *p);
   
   switch(g->st[st_maintab].i)
   {
   case cbi_tab_upgrades:   Lith_CBITab_Upgrades(g, p); break;
   case cbi_tab_cbi:        Lith_CBITab_CBI     (g, p); break;
   case cbi_tab_shop:       Lith_CBITab_Shop    (g, p); break;
   case cbi_tab_bip:        Lith_CBITab_BIP     (g, p); break;
   case cbi_tab_settings:   Lith_CBITab_Settings(g, p); break;
   case cbi_tab_log:        Lith_CBITab_Log     (g, p); break;
   }
   
   Lith_GUI_End(g);
}

//
// Lith_PlayerResetCBIGUI
//
void Lith_PlayerResetCBIGUI(player_t *p)
{
   p->cbi.guistate.cx = 320 / 2;
   p->cbi.guistate.cy = 200 / 2;
   
   Lith_GUI_Init(&p->cbi.guistate, st_max);
   
   p->cbi.guistate.st[st_upgrselold].i = -1;
   
   p->cbi.oldtheme = -1;
}


//----------------------------------------------------------------------------
// Scripts
//

//
// Lith_KeyOpenCBI
//
[[__call("ScriptS"), __extern("ACS"), __script("Net")]]
void Lith_KeyOpenCBI(void)
{
   if(ACS_Timer() < 10) return;
   
   player_t *p = Lith_LocalPlayer;
   
   if(p->dead)
      return;
   
   p->bip.curcategory = BIPC_MAIN;
   p->bip.curpage     = null;
   p->bip.curpagenum  = -1;
   
   p->useGUI(GUI_CBI);
}

// EOF

