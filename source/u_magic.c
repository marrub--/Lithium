// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Magic upgrade.                                                           │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"
#include "w_world.h"

#define udata pl.upgrdata.magic

struct magic_info {
   i32 st;
   i32 x, y;
   cstr name;
};

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

script static
void GiveMagic(struct magic_info const *m) {
   ACS_SetWeapon(fast_strdup2(OBJ, m->name));
}

script static
void UpdateMagicUI(void) {
   struct gui_state *g = &udata.gst;

   G_Begin(g, 320, 240);

   PrintSprite(sp_UI_MagicSelectBack, 0,1, 0,1);

   bool any = false;

   for(i32 i = 0; i < countof(minf); i++) {
      struct magic_info const *m = &minf[i];

      if(m->st != -1 && !get_bit(wl.cbiupgr, m->st)) continue;

      char gfx[32] = ":UI:"; faststrcat (gfx, m->name);
      char hot[32] = ":UI:"; faststrcat2(hot, m->name, "Sel");

      struct gui_pre_btn pre = {
         .gfx      = gfx,
         .hot      = hot,
         .cdef     = CR_GREEN,
         .cact     = CR_DARKRED,
         .chot     = CR_YELLOW,
         .cdis     = CR_BLACK,
         .font     = s"smallfnt",
         .snd      = s"player/cbi/buttonpress",
         .external = true,
         .w        = 64,
         .h        = 64
      };

      if(G_Button_FId(g, i + 1, tmpstr(lang(fast_strdup2(LANG "INFO_SHORT_", m->name))),
                      m->x, m->y, .preset = &pre))
      {
         GiveMagic(m);
      }
   }

   G_End(g, gui_curs_outline);
}

alloc_aut(0) script static
void GivePlayerZ(i32 tid) {
   while(ACS_ThingCount(T_NONE, tid)) {
      SetMembI(tid, sm_UserZ, pl.z);
      ACS_Delay(1);
   }
}

static
void SetMagicUI(bool on) {
   if(pl.dead) return;

   if(on) {
      udata.ui = true;
      pl.semifrozen++;

      udata.gst.gfxprefix = ":UI:";
      udata.gst.cx = 320/2;
      udata.gst.cy = 240/2;
   } else if(!on && udata.ui) {
      if(udata.gst.hot) GiveMagic(&minf[udata.gst.hot - 1]);

      udata.ui = false;
      pl.semifrozen--;

      fastmemset(&udata.gst, 0, sizeof udata.gst);
   }
}

Decl_Update void Upgr_Magic_Update(void) {
   k32 manaperc = pl.mana / (k32)pl.manamax;

   if(udata.manaperc < 1 && manaperc == 1) {
      AmbientSound(ss_player_jem_manafull, 1.0);
   }

   udata.manaperc = manaperc;

   if(P_ButtonPressed(BT_USER4)) {
      SetMagicUI(true);
   } else if(P_ButtonReleased(BT_USER4)) {
      SetMagicUI(false);
   }

   if(udata.ui) {
      UpdateMagicUI();
   }

   if(manaperc >= 0.7) {
      for(i32 i = 0; i < 5 * manaperc; i++) {
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
}

Decl_Render void Upgr_Magic_Render(void) {
   if(!pl.hudenabled) return;
   i32 hprc = ceilk(mink(udata.manaperc,        0.5k) * 2 * 62);
   i32 fprc = ceilk(maxk(udata.manaperc - 0.5k, 0.0k) * 2 * 62);
   PrintSprite(sp_HUD_C_MagicIcon, pl.hudlpos+1,1, 213,2);
   PrintSprite(sp_HUD_C_BarSmall2, pl.hudlpos+1,1, 220,2);
   PrintSpriteClip(sp_HUD_C_ManaBar1, pl.hudlpos+2,1, 219,2, 0,0,hprc,5);
   PrintSpriteClip(sp_HUD_C_ManaBar2, pl.hudlpos+2,1, 219,2, 0,0,fprc,5);
}

script_str ext("ACS") addr(OBJ "SetMagicUI")
void Z_SetMagicUI(bool on) {
   SetMagicUI(on);
}

/* EOF */
