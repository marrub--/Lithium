#include "lith_upgrades_common.h"
#include "lith_world.h"

#define UData UData_Magic(upgr)


//----------------------------------------------------------------------------
// Static Functions
//

//
// UpdateMagicUI
//
[[__call("ScriptS")]]
static void UpdateMagicUI(player_t *p, upgrade_t *upgr)
{
   struct magic_info {
      int st;
      int x, y;
      __str name;
   };

   static struct magic_info const minf[] = {
      {cupg_c_slot3spell, 160,  50, "Delear"  },
      {cupg_c_slot5spell,  80, 100, "Hulgyon" },
      {cupg_c_slot6spell, 240, 100, "StarShot"},
   };

   gui_state_t *g = &UData.gst;

   Lith_GUI_Begin(g, hid_end_dialogue, 320, 240);
   Lith_GUI_UpdateState(g, p);

   DrawSpritePlain("lgfx/UI/MagicSelectBack.png", g->hid--, 0.1, 0.1, TICSECOND);

   bool any = false;
   for(int i = 0; i < countof(minf); i++)
   {
      struct magic_info const *m = &minf[i];

      if(!world.cbiupgr[m->st])
         continue;

      gui_button_preset_t btnpre = {BtnDefault, .w = 80, .h = 80};

      btnpre.gfx = StrParam("%S.png",    m->name);
      btnpre.hot = StrParam("%SSel.png", m->name);

      __str name = Language("LITH_TXT_INFO_SHORT_%S", m->name);
      if(Lith_GUI_Button_Id(g, i, name, m->x - 40, m->y - 40, .preset = &btnpre)) {
         __str cn = StrParam("Lith_%S", m->name);
         ACS_GiveInventory(cn, 1);
         ACS_SetWeapon(cn);
      }

      any = true;
   }

   if(!any) {
      HudMessageF("CBIFONT", "No Spells Available");
      HudMessagePlain(g->hid--, g->w/2, g->h/2, TICSECOND);
   }

   Lith_GUI_End(g);
}

//
// Lith_GivePlayerZ
//
[[__call("ScriptS")]]
static void Lith_GivePlayerZ(int tid, player_t *p, __str name)
{
   while(ACS_ThingCount(T_NONE, tid)) {
      ACS_SetUserVariable(tid, name, p->z);
      ACS_Delay(1);
   }
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_SetMagicUI
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_SetMagicUI(bool on)
{
   player_t *p = LocalPlayer;
   upgrade_t *upgr = p->getUpgr(UPGR_Magic);

   if(on && !p->indialogue)
   {
      p->indialogue = UData.ui = true;
      p->semifrozen++;
      ACS_SetPlayerProperty(0, true, PROP_INSTANTWEAPONSWITCH);
      UData.gst.gfxprefix = "lgfx/UI/";
      UData.gst.cx = 320/2;
      UData.gst.cy = 240/2;
      Lith_GUI_Init(&UData.gst, 0);
   }
   else if(!on && UData.ui)
   {
      p->indialogue = UData.ui = false;
      p->semifrozen--;
      ACS_SetPlayerProperty(0, false, PROP_INSTANTWEAPONSWITCH);
      memset(&UData.gst, 0, sizeof(UData.gst));
   }
}

//
// Update
//
[[__call("ScriptS")]]
void Upgr_Magic_Update(player_t *p, upgrade_t *upgr)
{
   fixed manaperc = p->mana / (fixed)p->manamax;

   if(UData.manaperc < 1 && manaperc == 1)
      ACS_LocalAmbientSound("player/manafull", 127);

   UData.manaperc = manaperc;

   if(manaperc < 0.5 && ACS_Timer() % 5 == 0)
      ACS_GiveInventory("Lith_MagicAmmo", 1);

   if(p->weapontype != weapon_c_fist) {
      if(p->buttons & BT_USER4 && !(p->old.buttons & BT_USER4))
         Lith_SetMagicUI(true);
      else if(!(p->buttons & BT_USER4) && p->old.buttons & BT_USER4)
         Lith_SetMagicUI(false);
   }

   if(UData.ui)
      UpdateMagicUI(p, upgr);

   if(manaperc >= 0.7)
      for(int i = 0; i < 5 * manaperc; i++)
   {
      fixed dst = ACS_RandomFixed(32, 56);
      fixed ang = ACS_RandomFixed(0, 1);
      int tid = ACS_UniqueTID();
      int x   = ACS_Cos(ang) * dst;
      int y   = ACS_Sin(ang) * dst;
      int z   = ACS_Random(8, 48);
      ACS_Spawn("Lith_ManaLeak", p->x + x, p->y + y, p->z + z, tid);
      ACS_SetUserVariable(tid, "user_x", x);
      ACS_SetUserVariable(tid, "user_y", y);
      ACS_SetActorPropertyFixed(tid, APROP_Alpha, manaperc);
      Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_MASTER, p->tid);
      Lith_GivePlayerZ(tid, p, "user_z");
   }
}

//
// Render
//
void Upgr_Magic_Render(player_t *p, upgrade_t *upgr)
{
   int hprc = ceilk(min(UData.manaperc,       0.5) * 2 * 33);
   int fprc = ceilk(max(UData.manaperc - 0.5, 0.0) * 2 * 33);

   DrawSpritePlain("lgfx/HUD_C/MagicIcon.png", hid_magicsymbol, 75.1, 199.2, TICSECOND);
   DrawSpritePlain("lgfx/HUD_C/BarVert.png",   hid_magicammobg, 67.1, 199.2, TICSECOND);

   ACS_SetHudClipRect(68, 198 - hprc, 5, hprc);
   DrawSpritePlain("lgfx/HUD_C/ManaBar1.png",  hid_magicammo1,  68.1, 198.2, TICSECOND);
   ACS_SetHudClipRect(0, 0, 0, 0);

   ACS_SetHudClipRect(68, 198 - fprc, 5, fprc);
   DrawSpritePlain("lgfx/HUD_C/ManaBar2.png",  hid_magicammo2,  68.1, 198.2, TICSECOND);
   ACS_SetHudClipRect(0, 0, 0, 0);
}

// EOF

