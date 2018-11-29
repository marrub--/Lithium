// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"
#include "lith_world.h"

#define UData UData_Magic(upgr)

// Types ---------------------------------------------------------------------|

struct magic_info {
   int st;
   int x, y;
   char const *name, *classname;
};

// Static Objects ------------------------------------------------------------|

static struct magic_info const minf[] = {
   {-1,                130, 180, c"Blade",    c"Lith_Blade"   },
   {-1,                 60, 140, c"Delear",   c"Lith_Delear"  },
   {cupg_c_slot3spell,  60,  60, c"Feuer",    c"Lith_Feuer"   },
   {cupg_c_slot4spell, 130,  10, c"Rend",     c"Lith_Rend"    },
   {cupg_c_slot5spell, 205,  60, c"Hulgyon",  c"Lith_Hulgyon" },
   {cupg_c_slot6spell, 205, 140, c"StarShot", c"Lith_StarShot"},
   {cupg_c_slot7spell, 130, 100, c"Cercle",   c"Lith_Cercle"  },
};

// Static Functions ----------------------------------------------------------|

script
static void GiveMagic(struct magic_info const *m)
{
   ACS_SetWeapon(l_strdup(m->classname));
}

script
static void UpdateMagicUI(struct player *p, upgrade_t *upgr)
{
   gui_state_t *g = &UData.gst;

   Lith_GUI_Begin(g, 320, 240);
   Lith_GUI_UpdateState(g, p);

   PrintSprite(":UI:MagicSelectBack", 0,1, 0,1);

   bool any = false;

   for(int i = 0; i < countof(minf); i++)
   {
      struct magic_info const *m = &minf[i];

      if(m->st != -1 && !world.cbiupgr[m->st]) continue;

      char gfx[18]; sprintf(gfx, c":UI:%s",    m->name);
      char hot[18]; sprintf(hot, c":UI:%sSel", m->name);

      gui_button_preset_t pre = {
         .gfx      = gfx,
         .hot      = hot,
         .snd      = c"player/cbi/buttonpress",
         .cdef     = c"d",
         .cact     = c"r",
         .chot     = c"k",
         .cdis     = c"m",
         .font     = c"cbifont",
         .external = true,
         .w        = 64,
         .h        = 64
      };

      char name[24]; sprintf(name, c"LITH_INFO_SHORT_%s", m->name);

      if(Lith_GUI_Button_FId(g, i + 1, name, m->x, m->y, .preset = &pre))
         GiveMagic(m);
   }

   Lith_GUI_End(g, gui_curs_outline);
}

script
static void GivePlayerZ(int tid, struct player *p, __str name)
{
   while(ACS_ThingCount(T_NONE, tid)) {
      SetMembI(tid, name, p->z);
      ACS_Delay(1);
   }
}

// Extern Functions ----------------------------------------------------------|

script ext("ACS")
void Lith_SetMagicUI(bool on)
{
   withplayer(LocalPlayer)
   {
      if(p->dead) return;

      upgrade_t *upgr = p->getUpgr(UPGR_Magic);

      if(on)
      {
         UData.ui = true;
         p->semifrozen++;

         UData.gst.gfxprefix = ":UI:";
         UData.gst.cx = 320/2;
         UData.gst.cy = 240/2;

         Lith_GUI_Init(&UData.gst);
      }
      else if(!on && UData.ui)
      {
         if(UData.gst.hot) GiveMagic(&minf[UData.gst.hot - 1]);

         UData.ui = false;
         p->semifrozen--;

         memset(&UData.gst, 0, sizeof UData.gst);
      }
   }
}

script
void Upgr_Magic_Update(struct player *p, upgrade_t *upgr)
{
   fixed manaperc = p->mana / (fixed)p->manamax;

   if(UData.manaperc < 1 && manaperc == 1)
      ACS_LocalAmbientSound("player/manafull", 127);

   UData.manaperc = manaperc;

   if(p->buttons & BT_USER4 && !(p->old.buttons & BT_USER4))
      Lith_SetMagicUI(true);
   else if(!(p->buttons & BT_USER4) && p->old.buttons & BT_USER4)
      Lith_SetMagicUI(false);

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
      SetMembI(tid, "user_x", x);
      SetMembI(tid, "user_y", y);
      SetPropK(tid, APROP_Alpha, manaperc / 2);
      Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_MASTER, p->tid);
      GivePlayerZ(tid, p, "user_z");
   }
}

stkcall
void Upgr_Magic_Render(struct player *p, upgrade_t *upgr)
{
   if(!p->hudenabled) return;

   int hprc = ceilk(min(UData.manaperc,       0.5) * 2 * 62);
   int fprc = ceilk(max(UData.manaperc - 0.5, 0.0) * 2 * 62);

   PrintSprite(":HUD_C:MagicIcon", 1,1, 213,2);
   PrintSprite(":HUD_C:BarSmall2", 1,1, 220,2);

   SetClip(2, 219-5, hprc, 5);
   PrintSprite(":HUD_C:ManaBar1", 2,1, 219,2);
   ClearClip();

   SetClip(2, 219-5, fprc, 5);
   PrintSprite(":HUD_C:ManaBar2", 2,1, 219,2);
   ClearClip();
}

// EOF

