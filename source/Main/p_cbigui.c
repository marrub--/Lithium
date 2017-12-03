// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
// vim: columns=120
#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"
#include "lith_world.h"

#define HasUpgr(name) if(world.cbiupgr[name])

// Static Functions ----------------------------------------------------------|

//
// CBITab_Marine
//
static void CBITab_Marine(gui_state_t *g, player_t *p)
{
   __str name;
   int ram;

   if(world.cbiupgr[cupg_hasupgr2])
   {
      name = Language("LITH_TXT_INFO_TITLE_CBIUpgr2");
      ram  = 150;

      DrawSpritePlain("lgfx/UI/CPU1.png", g->hid--, .1, .1, TICSECOND);
   }
   else if(world.cbiupgr[cupg_hasupgr1])
   {
      name = Language("LITH_TXT_INFO_TITLE_CBIUpgr1");
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

   int y = 70;
   #define Info(...) \
      do { \
         HudMessageF("CBIFONT", __VA_ARGS__); \
         HudMessagePlain(g->hid--, 23.1, y + .1, TICSECOND); \
         y += 10; \
      } while(0)

   Info("Performance: %i\CbPr", world.cbiperf);
   Info("In use: %i\CbPr", p->cbi.pruse);
   Info("RAM: %iTiB", ram);

   y += 20;

   HasUpgr(cupg_armorinter) Info("Has Armor Interface");
   HasUpgr(cupg_weapninter) Info("Has Weapon Modification Device");
   HasUpgr(cupg_weapninte2) Info("Has Weapon Refactoring Device");
   HasUpgr(cupg_rdistinter) Info("Has Reality Distortion Interface");

   #undef Info

   HasUpgr(cupg_armorinter) DrawSpritePlain("lgfx/UI/ArmorInter.png", g->hid--, 300.2, 48*1 + .1 - 20, TICSECOND);
   HasUpgr(cupg_weapninter) DrawSpritePlain("lgfx/UI/WeapnInter.png", g->hid--, 300.2, 48*2 + .1 - 20, TICSECOND);
   HasUpgr(cupg_weapninte2) DrawSpritePlain("lgfx/UI/WeapnInte2.png", g->hid--, 300.2, 48*3 + .1 - 20, TICSECOND);
   HasUpgr(cupg_rdistinter) DrawSpritePlain("lgfx/UI/RDistInter.png", g->hid--, 300.2, 48*4 + .1 - 20, TICSECOND);
}

//
// CBITab_CyberMage
//
static void CBITab_CyberMage(gui_state_t *g, player_t *p)
{
   DrawSpritePlain("lgfx/UI/CPU2.png", g->hid--, .1, .1, TICSECOND);
   HudMessageF("CBIFONT", "AOF 5900001 Rev7 CPU");
   HudMessagePlain(g->hid--, 20.1, 60.1, TICSECOND);

   int y = 70;
   #define Info(...) \
      do { \
         HudMessageF("CBIFONT", __VA_ARGS__); \
         HudMessagePlain(g->hid--, 23.1, y + .1, TICSECOND); \
         y += 10; \
      } while(0)

   Info("Performance: 34\CbPr");
   Info("In use: %i\CbPr", p->cbi.pruse);
   Info("RAM: 19TiB");

   y += 20;

                              Info("\CiBlade Spell Driver \Cbinstalled.");
                              Info("\CiDelear Spell Driver \Cbinstalled.");
   HasUpgr(cupg_c_slot3spell) Info("\CiFeuer Spell Driver \Cbinstalled.");
   HasUpgr(cupg_c_slot4spell) Info("\CiRomyetya Spell Driver \Cbinstalled.");
   HasUpgr(cupg_c_slot5spell) Info("\CiHulgyon Spell Driver \Cbinstalled.");
   HasUpgr(cupg_c_slot6spell) Info("\CiHosh'danma Spell Driver \Cbinstalled.");
   HasUpgr(cupg_c_slot7spell) Info("\CgCercle de la Mort Spell Driver \Cbinstalled.");
   HasUpgr(cupg_c_rdistinter) Info("Reality Distortion Interface \Cbinstalled.");

   #undef Info

                              DrawSpritePlain("lgfx/UI/Slot1Spell.png", g->hid--, 300.2, 48*1+.1-20, TICSECOND);
                              DrawSpritePlain("lgfx/UI/Slot2Spell.png", g->hid--, 300.2, 48*2+.1-20, TICSECOND);
   HasUpgr(cupg_c_slot3spell) DrawSpritePlain("lgfx/UI/Slot3Spell.png", g->hid--, 300.2, 48*3+.1-20, TICSECOND);
   HasUpgr(cupg_c_slot4spell) DrawSpritePlain("lgfx/UI/Slot4Spell.png", g->hid--, 300.2, 48*4+.1-20, TICSECOND);
   HasUpgr(cupg_c_slot5spell) DrawSpritePlain("lgfx/UI/Slot5Spell.png", g->hid--, 252.2, 48*1+.1-20, TICSECOND);
   HasUpgr(cupg_c_slot6spell) DrawSpritePlain("lgfx/UI/Slot6Spell.png", g->hid--, 252.2, 48*2+.1-20, TICSECOND);
   HasUpgr(cupg_c_slot7spell) DrawSpritePlain("lgfx/UI/Slot7Spell.png", g->hid--, 252.2, 48*3+.1-20, TICSECOND);
   HasUpgr(cupg_c_rdistinter) DrawSpritePlain("lgfx/UI/RDistInter.png", g->hid--, 252.2, 48*4+.1-20, TICSECOND);
}

//
// Lith_CBITab_CBI
//
static void Lith_CBITab_CBI(gui_state_t *g, player_t *p)
{
   switch(p->pclass)
   {
   case pcl_marine:    CBITab_Marine   (g, p); break;
   case pcl_cybermage: CBITab_CyberMage(g, p); break;
   }
}

//
// AttrBar
//
static void AttrBar(gui_state_t *g, int x, int y, int w, __str gfx)
{
   Lith_GUI_Clip(g, x, y, w * 2, 8);
   DrawSpritePlain(gfx, g->hid--, x+.1, y+.1, TICSECOND);
   Lith_GUI_ClipRelease(g);
}

//
// DrawAttr
//
static void DrawAttr(gui_state_t *g, int x, int y, player_t *p, int at)
{
   unsigned attr = p->attr.attrs[at];
   __str    name = p->attr.names[at];
   fixed    helptrns = 0.5;

   if(p->attr.points) {
      if(Lith_GUI_Button_Id(g, at, .x = x-42, y-2, .preset = &guipre.btnprev, .slide = true))
         if(p->attr.sup.attrs[at] > attr)
      {
         p->attr.sup.points++;
         p->attr.sup.attrs[at]--;
      }

      if(Lith_GUI_Button_Id(g, at, .x = x-42 + guipre.btnprev.w, y-2, .preset = &guipre.btnnext, .slide = true))
         if(p->attr.sup.points && p->attr.sup.attrs[at] < ATTR_MAX)
      {
         p->attr.sup.points--;
         p->attr.sup.attrs[at]++;
      }
   }

   HudMessageF("CHFONT", "%.3S", name);
   HudMessageParams(0, g->hid--, CR_WHITE, x+.1-24, y+.1, TICSECOND);

   DrawSpritePlain("lgfx/UI/AttrBar1.png", g->hid--, x+.1, y+.1, TICSECOND);

   unsigned satr = p->attr.sup.attrs[at];

   if(satr != attr)
      AttrBar(g, x, y, satr, "lgfx/UI/AttrBar3.png");

   AttrBar(g, x, y, attr, "lgfx/UI/AttrBar2.png");

   if(attr > ATTR_VIS_MAX) {
      int vatr = attr - ATTR_VIS_MAX;
      AttrBar(g, x, y, (vatr / (float)ATTR_VIS_DIFF) * ATTR_VIS_MAX, "lgfx/UI/AttrBar4.png");
      helptrns += 0.3;
   }

   HudMessageF("CHFONT", "%u/%i", satr, ATTR_VIS_MAX);
   HudMessageParams(0, g->hid--, CR_WHITE, x+202.1, y+.1, TICSECOND);

   static __str const helpstrs[at_max] = {
      [at_acc] = "Weapon damage",
      [at_def] = "Armor efficiency",
      [at_str] = "Health capacity",
      [at_vit] = "Health pickup efficiency",
      [at_stm] = "Health regeneration",
      [at_luk] = "Random chance",
      [at_rge] = "Damage buff when hit"
   };

   HudMessageF("CHFONT", "%S", helpstrs[at]);
   HudMessageParams(HUDMSG_ALPHA, g->hid--, CR_WHITE, x+1.1, y+1.1, TICSECOND, helptrns);

   HudMessageF("CHFONT", "%u/%i", satr, ATTR_VIS_MAX);
   HudMessageParams(0, g->hid--, CR_WHITE, x+202.1, y+.1, TICSECOND);
}

//
// StatusInfo
//
static void StatusInfo(gui_state_t *g, int x, int y, __str left, __str right)
{
   ACS_SetFont("CHFONT");
   HudMessage("%S", left);
   HudMessageParams(0, g->hid--, CR_WHITE, x+  .1, y+.1, TICSECOND);
   HudMessage("%S", right);
   HudMessageParams(0, g->hid--, CR_WHITE, x+80.2, y+.1, TICSECOND);
}

//
// Lith_CBITab_Status
//
static void Lith_CBITab_Status(gui_state_t *g, player_t *p)
{
   int x = 30, y = 40;
   ACS_SetFont("CHFONT");
   HudMessage("%S", p->name);
   HudMessageParams(0, g->hid--, CR_WHITE, x+.1, y+.1, TICSECOND);
   y += 10;
   HudMessage("%S", p->classname);
   HudMessageParams(0, g->hid--, CR_WHITE, x+.1, y+.1, TICSECOND);
   StatusInfo(g, x, y += 10, "Lv.",  StrParam("%u", p->attr.level));
   StatusInfo(g, x, y += 10, "HP",   StrParam("%i/%i", p->health, p->maxhealth));
   if(p->pclass & pcl_magicuser)
      StatusInfo(g, x, y += 10, "MP", StrParam("%i/%i", p->mana, p->manamax));
   StatusInfo(g, x, y += 10, "EXP",  StrParam("%lu", p->attr.exp));
   StatusInfo(g, x, y += 10, "Next", StrParam("%lu", p->attr.expnext));
   x = 20;
   if(p->pclass & pcl_magicuser) y += 20;
   else                          y += 30;
   if(p->attr.points) {
      HudMessage("Divide %u points among your attributes.", p->attr.sup.points);
      HudMessageParams(0, g->hid--, CR_WHITE, x+.1, y+.1, TICSECOND);
   }
   x  = 60;
   y += 10;
   for(int i = 0; i < at_max; i++, y += 10)
      DrawAttr(g, x, y, p, i);
   if(p->attr.points)
      if(Lith_GUI_Button(g, "Apply", 255, 208))
         p->attr.cur = p->attr.sup;
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_PlayerUpdateCBIGUI
//
[[__call("ScriptS")]]
void Lith_PlayerUpdateCBIGUI(player_t *p)
{
   gui_state_t *g = &p->cbi.guistate;

   p->cbi.theme = p->getCVarI("lith_gui_theme");

   if(p->cbi.theme != p->cbi.oldtheme)
   {
      switch((p->cbi.oldtheme = p->cbi.theme))
      {
      default:
      case cbi_theme_green:    p->cbi.guistate.gfxprefix = "lgfx/UI_Green/";    break;
      case cbi_theme_rose:     p->cbi.guistate.gfxprefix = "lgfx/UI_Rose/";     break;
      case cbi_theme_umi:      p->cbi.guistate.gfxprefix = "lgfx/UI_Umi/";      break;
      case cbi_theme_ender:    p->cbi.guistate.gfxprefix = "lgfx/UI_Ender/";    break;
      case cbi_theme_orange:   p->cbi.guistate.gfxprefix = "lgfx/UI_Orange/";   break;
      case cbi_theme_grey:     p->cbi.guistate.gfxprefix = "lgfx/UI_Grey/";     break;
      case cbi_theme_bassilla: p->cbi.guistate.gfxprefix = "lgfx/UI_Bassilla/"; break;
      case cbi_theme_ghost:    p->cbi.guistate.gfxprefix = "lgfx/UI_Ghost/";    break;
      case cbi_theme_winxp:    p->cbi.guistate.gfxprefix = "lgfx/UI_WinXP/";    break;
      }
   }

   Lith_GUI_Begin(g, hid_end_cbi, 320, 240);

   if(!p->indialogue)
      Lith_GUI_UpdateState(g, p);

   DrawSpriteAlpha(StrParam("%SBackground.png", g->gfxprefix), g->hid--, 0.1, 0.1, TICSECOND, 0.7);

   if(Lith_GUI_Button(g, .x = 296, 13, .preset = &guipre.btnexit))
      p->useGUI(GUI_CBI);

   static __str tabnames[cbi_tab_max] = {"Upgrades", "CBI", "Status", "Shop", "Info", "Settings"};
   for(int i = 0; i < cbi_tab_max; i++)
      if(Lith_GUI_Button_Id(g, i, tabnames[i], guipre.btntab.w * i + 13, 13, i == g->st[st_maintab].i, .preset = &guipre.btntab))
         g->st[st_maintab].i = i;

   extern void Lith_CBITab_Upgrades(gui_state_t *g, player_t *p);
   extern void Lith_CBITab_Shop    (gui_state_t *g, player_t *p);
   extern void Lith_CBITab_BIP     (gui_state_t *g, player_t *p);
   extern void Lith_CBITab_Settings(gui_state_t *g, player_t *p);

   switch(g->st[st_maintab].i)
   {
   case cbi_tab_upgrades: Lith_CBITab_Upgrades(g, p); break;
   case cbi_tab_cbi:      Lith_CBITab_CBI     (g, p); break;
   case cbi_tab_status:   Lith_CBITab_Status  (g, p); break;
   case cbi_tab_shop:     Lith_CBITab_Shop    (g, p); break;
   case cbi_tab_bip:      Lith_CBITab_BIP     (g, p); break;
   case cbi_tab_settings: Lith_CBITab_Settings(g, p); break;
   }

   Lith_GUI_End(g);
}

//
// Lith_PlayerResetCBIGUI
//
void Lith_PlayerResetCBIGUI(player_t *p)
{
   p->cbi.guistate.cx = 320 / 2;
   p->cbi.guistate.cy = 240 / 2;

   Lith_GUI_Init(&p->cbi.guistate, st_max);

   p->cbi.guistate.st[st_upgrselold].i = -1;

   p->cbi.oldtheme = -1;
}

// Scripts -------------------------------------------------------------------|

//
// Lith_KeyOpenCBI
//
[[__call("ScriptS"), __extern("ACS"), __script("Net")]]
void Lith_KeyOpenCBI(void)
{
   if(ACS_Timer() < 10) return;

   withplayer(LocalPlayer) {
      p->bip.curcategory = BIPC_MAIN;
      p->bip.curpage = null;
      p->useGUI(GUI_CBI);
   }
}

// EOF

