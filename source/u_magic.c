/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
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

#define udata pl.upgrdata.magic

/* Types ------------------------------------------------------------------- */

struct magic_info {
   i32 st;
   i32 x, y;
   cstr name;
};

/* Static Objects ---------------------------------------------------------- */

static
struct magic_info const minf[] = {
   {-1,                130, 180, "Blade"   },
   {-1,                 60, 140, "Delear"  },
   {cupg_c_slot3spell,  60,  60, "Feuer"   },
   {cupg_c_slot4spell, 130,  10, "Rend"    },
   {cupg_c_slot5spell, 205,  60, "Hulgyon" },
   {cupg_c_slot6spell, 205, 140, "StarShot"},
   {cupg_c_slot7spell, 130, 100, "Cercle"  },
};

/* Static Functions -------------------------------------------------------- */

script static
void GiveMagic(struct magic_info const *m)
{
   ACS_SetWeapon(StrParam(OBJ "%s", m->name));
}

script static
void UpdateMagicUI(struct upgrade *upgr)
{
   struct gui_state *g = &udata.gst;

   G_Begin(g, 320, 240);
   G_UpdateState(g);

   PrintSprite(sp_UI_MagicSelectBack, 0,1, 0,1);

   bool any = false;

   for(i32 i = 0; i < countof(minf); i++)
   {
      struct magic_info const *m = &minf[i];

      if(m->st != -1 && get_bit(cbiupgr, m->st)) continue;

      char gfx[32] = ":UI:"; faststrcat (gfx, m->name);
      char hot[32] = ":UI:"; faststrcat2(hot, m->name, "Sel");

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

      if(G_Button_FId(g, i + 1, tmpstr(lang_fmt(LANG "INFO_SHORT_%s",
                                                m->name)),
                      m->x, m->y, .preset = &pre))
      {
         GiveMagic(m);
      }
   }

   G_End(g, gui_curs_outline);
}

alloc_aut(0) script static
void GivePlayerZ(i32 tid)
{
   while(ACS_ThingCount(T_NONE, tid)) {
      SetMembI(tid, sm_UserZ, pl.z);
      ACS_Delay(1);
   }
}

static
void SetMagicUI(bool on)
{
   if(pl.dead) return;

   if(on)
   {
      udata.ui = true;
      pl.semifrozen++;

      udata.gst.gfxprefix = ":UI:";
      udata.gst.cx = 320/2;
      udata.gst.cy = 240/2;
   }
   else if(!on && udata.ui)
   {
      if(udata.gst.hot) GiveMagic(&minf[udata.gst.hot - 1]);

      udata.ui = false;
      pl.semifrozen--;

      fastmemset(&udata.gst, 0, sizeof udata.gst);
   }
}

/* Extern Functions -------------------------------------------------------- */

script
void Upgr_Magic_Update(struct upgrade *upgr)
{
   k32 manaperc = pl.mana / (k32)pl.manamax;

   if(udata.manaperc < 1 && manaperc == 1)
      AmbientSound(ss_player_manafull, 1.0);

   udata.manaperc = manaperc;

   if(pl.buttons & BT_USER4 && !(pl.old.buttons & BT_USER4))
      SetMagicUI(true);
   else if(!(pl.buttons & BT_USER4) && pl.old.buttons & BT_USER4)
      SetMagicUI(false);

   if(udata.ui)
      UpdateMagicUI(upgr);

   if(manaperc >= 0.7)
      for(i32 i = 0; i < 5 * manaperc; i++)
   {
      k32 dst = ACS_RandomFixed(32, 56);
      k32 ang = ACS_RandomFixed(0, 1);
      i32 tid = ACS_UniqueTID();
      i32 x   = ACS_Cos(ang) * dst;
      i32 y   = ACS_Sin(ang) * dst;
      i32 z   = ACS_Random(8, 48);
      ACS_Spawn(so_ManaLeak, pl.x + x, pl.y + y, pl.z + z, tid);
      SetMembI(tid, sm_UserX, x);
      SetMembI(tid, sm_UserY, y);
      SetAlpha(tid, manaperc / 2);
      PtrSet(tid, AAPTR_DEFAULT, AAPTR_MASTER, pl.tid);
      GivePlayerZ(tid);
   }
}

void Upgr_Magic_Render(struct upgrade *upgr)
{
   if(!pl.hudenabled) return;

   i32 hprc = ceilk(min(udata.manaperc,        0.5k) * 2 * 62);
   i32 fprc = ceilk(max(udata.manaperc - 0.5k, 0.0k) * 2 * 62);

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

script_str ext("ACS") addr(OBJ "SetMagicUI")
void Sc_SetMagicUI(bool on)
{
   if(!P_None()) SetMagicUI(on);
}

/* EOF */
