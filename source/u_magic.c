/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Magic upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"
#include "w_world.h"

#define UData p->upgrdata.magic

/* Types ------------------------------------------------------------------- */

struct magic_info {
   i32 st;
   i32 x, y;
   cstr name;
};

/* Static Objects ---------------------------------------------------------- */

static struct magic_info const minf[] = {
   {-1,                130, 180, "Blade"   },
   {-1,                 60, 140, "Delear"  },
   {cupg_c_slot3spell,  60,  60, "Feuer"   },
   {cupg_c_slot4spell, 130,  10, "Rend"    },
   {cupg_c_slot5spell, 205,  60, "Hulgyon" },
   {cupg_c_slot6spell, 205, 140, "StarShot"},
   {cupg_c_slot7spell, 130, 100, "Cercle"  },
};

/* Static Functions -------------------------------------------------------- */

script
static void GiveMagic(struct magic_info const *m)
{
   ACS_SetWeapon(StrParam(OBJ "%s", m->name));
}

script
static void UpdateMagicUI(struct player *p, struct upgrade *upgr)
{
   struct gui_state *g = &UData.gst;

   G_Begin(g, 320, 240);
   G_UpdateState(g, p);

   PrintSprite(sp_UI_MagicSelectBack, 0,1, 0,1);

   bool any = false;

   for(i32 i = 0; i < countof(minf); i++)
   {
      struct magic_info const *m = &minf[i];

      if(m->st != -1 && !cbiupgr[m->st]) continue;

      char gfx[32] = ":UI:"; strcat(gfx, m->name);
      char hot[32] = ":UI:"; strcat(hot, m->name); strcat(hot, "Sel");

      struct gui_pre_btn pre = {
         .gfx      = gfx,
         .hot      = hot,
         .cdef     = "d",
         .cact     = "r",
         .chot     = "k",
         .cdis     = "m",
         .font     = s"smallfnt",
         .snd      = s"player/cbi/buttonpress",
         .external = true,
         .w        = 64,
         .h        = 64
      };

      char name[128]; LanguageCV(name, LANG "INFO_SHORT_%s", m->name);

      if(G_Button_FId(g, i + 1, name, m->x, m->y, .preset = &pre))
         GiveMagic(m);
   }

   G_End(g, gui_curs_outline);
}

script
static void GivePlayerZ(i32 tid, struct player *p)
{
   while(ACS_ThingCount(T_NONE, tid)) {
      SetMembI(tid, sm_UserZ, p->z);
      ACS_Delay(1);
   }
}

static void SetMagicUI(struct player *p, bool on)
{
   if(p->dead) return;

   struct upgrade *upgr = p->getUpgr(UPGR_Magic);

   if(on)
   {
      UData.ui = true;
      p->semifrozen++;

      UData.gst.gfxprefix = ":UI:";
      UData.gst.cx = 320/2;
      UData.gst.cy = 240/2;

      G_Init(&UData.gst);
   }
   else if(!on && UData.ui)
   {
      if(UData.gst.hot) GiveMagic(&minf[UData.gst.hot - 1]);

      UData.ui = false;
      p->semifrozen--;

      fastmemset(&UData.gst, 0, sizeof UData.gst);
   }
}

/* Extern Functions -------------------------------------------------------- */

script
void Upgr_Magic_Update(struct player *p, struct upgrade *upgr)
{
   StrEntON
   k32 manaperc = p->mana / (k32)p->manamax;

   if(UData.manaperc < 1 && manaperc == 1)
      ACS_LocalAmbientSound(ss_player_manafull, 127);

   UData.manaperc = manaperc;

   if(p->buttons & BT_USER4 && !(p->old.buttons & BT_USER4))
      SetMagicUI(p, true);
   else if(!(p->buttons & BT_USER4) && p->old.buttons & BT_USER4)
      SetMagicUI(p, false);

   if(UData.ui)
      UpdateMagicUI(p, upgr);

   if(manaperc >= 0.7)
      for(i32 i = 0; i < 5 * manaperc; i++)
   {
      k32 dst = ACS_RandomFixed(32, 56);
      k32 ang = ACS_RandomFixed(0, 1);
      i32 tid = ACS_UniqueTID();
      i32 x   = ACS_Cos(ang) * dst;
      i32 y   = ACS_Sin(ang) * dst;
      i32 z   = ACS_Random(8, 48);
      ACS_Spawn(so_ManaLeak, p->x + x, p->y + y, p->z + z, tid);
      SetMembI(tid, sm_UserX, x);
      SetMembI(tid, sm_UserY, y);
      SetPropK(tid, APROP_Alpha, manaperc / 2);
      PtrSet(tid, AAPTR_DEFAULT, AAPTR_MASTER, p->tid);
      GivePlayerZ(tid, p);
   }
}

stkcall
void Upgr_Magic_Render(struct player *p, struct upgrade *upgr)
{
   if(!p->hudenabled || p->dlg.active) return;

   i32 hprc = ceilk(min(UData.manaperc,       0.5) * 2 * 62);
   i32 fprc = ceilk(max(UData.manaperc - 0.5, 0.0) * 2 * 62);

   PrintSprite(sp_HUD_C_MagicIcon, 1,1, 213,2);
   PrintSprite(sp_HUD_C_BarSmall2, 1,1, 220,2);

   SetClip(2, 219-5, hprc, 5);
   PrintSprite(sp_HUD_C_ManaBar1, 2,1, 219,2);
   ClearClip();

   SetClip(2, 219-5, fprc, 5);
   PrintSprite(sp_HUD_C_ManaBar2, 2,1, 219,2);
   ClearClip();
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr("Lith_SetMagicUI")
void Sc_SetMagicUI(bool on)
{
   with_player(LocalPlayer) SetMagicUI(p, on);
}

/* EOF */
