// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ CBI tab GUI.                                                             │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"

#define Upgr(name) if(get_bit(wl.cbiupgr, name))

#define InfoStart i32 y  = 57
#define InfoSep       y += 20

#define Info(...) \
   ( \
      BeginPrintFmt(__VA_ARGS__), \
      PrintText(sf_smallfnt, g->defcr, g->ox+10,1, g->oy+y,1), \
      y += 10 \
   )

#define Slot(name, x, y) PrintSprite(name, g->ox+287-x*48,2, g->oy+48*y-33,1)
#define CPU(num)         PrintSprite(sp_UI_CPU##num, g->ox-13,1, g->oy-13,1, _u_alpha, 0.8)

static
void CBITab_Marine(struct gui_state *g) {
   i32 ram;
   str name;

        Upgr(cupg_m_cpu2) {CPU(1); ram = 150; name = sl_cbi_cpu3;}
   else Upgr(cupg_m_cpu1) {CPU(2); ram = 100; name = sl_cbi_cpu2;}
   else                   {CPU(3); ram =  50; name = sl_cbi_cpu1;}

   PrintText_str(name, sf_smallfnt, g->defcr, g->ox+7,1, g->oy+47,1);

   InfoStart;

   Info(tmpstr(sl_cbi_perf), wl.cbiperf);
   Info(tmpstr(sl_cbi_puse), pl.cbi.pruse);
   Info(tmpstr(sl_cbi_wram), ram);

   InfoSep;

   Info(tmpstr(sl_cbi_interfaces));
   Upgr(cupg_m_armorinter) Info("> %S", sl_cbi_armorinter);
   Upgr(cupg_m_weapninter) Info("> %S", sl_cbi_weapninter);
   Upgr(cupg_m_weapninte2) Info("> %S", sl_cbi_weapninte2);
   Upgr(cupg_rdistinter)   Info("> %S", sl_cbi_rdistinter);

   Upgr(cupg_m_armorinter) Slot(sp_UI_ArmorInter, 0, 1);
   Upgr(cupg_m_weapninter) Slot(sp_UI_WeapnInter, 0, 2);
   Upgr(cupg_m_weapninte2) Slot(sp_UI_WeapnInte2, 0, 3);
   Upgr(cupg_rdistinter)   Slot(sp_UI_RDistInter, 0, 4);
}

static
void CBITab_CyberMage(struct gui_state *g) {
   CPU(2);
   PrintText_str(sl_cbi_cpu4, sf_smallfnt, g->defcr, g->ox+7,1, g->oy+47,1);

   InfoStart;

   Info(tmpstr(sl_cbi_perf), 34);
   Info(tmpstr(sl_cbi_puse), pl.cbi.pruse);
   Info(tmpstr(sl_cbi_wram), 19);

   InfoSep;

   Info(tmpstr(sl_cbi_interfaces));
                           Info("> %S", sl_cbi_slot1spell);
                           Info("> %S", sl_cbi_slot2spell);
   Upgr(cupg_c_slot3spell) Info("> %S", sl_cbi_slot3spell);
   Upgr(cupg_c_slot4spell) Info("> %S", sl_cbi_slot4spell);
   Upgr(cupg_c_slot5spell) Info("> %S", sl_cbi_slot5spell);
   Upgr(cupg_c_slot6spell) Info("> %S", sl_cbi_slot6spell);
   Upgr(cupg_c_slot7spell) Info("> %S", sl_cbi_slot7spell);
   Upgr(cupg_rdistinter)   Info("> %S", sl_cbi_rdistinter);

                           Slot(sp_UI_Slot1Spell, 0, 1);
                           Slot(sp_UI_Slot2Spell, 0, 2);
   Upgr(cupg_c_slot3spell) Slot(sp_UI_Slot3Spell, 0, 3);
   Upgr(cupg_c_slot4spell) Slot(sp_UI_Slot4Spell, 0, 4);
   Upgr(cupg_c_slot5spell) Slot(sp_UI_Slot5Spell, 1, 1);
   Upgr(cupg_c_slot6spell) Slot(sp_UI_Slot6Spell, 1, 2);
   Upgr(cupg_c_slot7spell) Slot(sp_UI_Slot7Spell, 1, 3);
   Upgr(cupg_rdistinter)   Slot(sp_UI_RDistInter, 1, 4);
}

static
void CBITab_DarkLord(struct gui_state *g) {
   CPU(3Blue);
   #define Rune(_gfx, _l, _x, _y) \
      ( \
         PrintSprite(sp_UI_Rune##_gfx, g->ox+_x*96+50,0, g->oy+_y*47+38,1), \
         PrintText_str(sl_cbi_rune_##_l, sf_lmidfont, CR_WHITE, g->ox+_x*96+50,4, g->oy+_y*47+38,2) \
      )
   Rune(Empty, rkz, 1, 0);
   Upgr(cupg_d_zaruk)   Rune(Zrk,  zrk,  1, 1);
   Upgr(cupg_d_zakwu)   Rune(Zkw,  zkw,  0, 2);
   Upgr(cupg_dimdriver) Rune(Zzkr, zzkr, 1, 2);
   Upgr(cupg_d_zikr)    Rune(Zkr,  zkr,  2, 2);
   Upgr(cupg_d_shield)  Rune(Rkw,  rkw,  1, 3);
   PrintText_str(sl_cbi_dl_status, sf_lmidfont, CR_WHITE, g->ox+4,1, g->oy+24,1);
}

void P_CBI_TabCBI(struct gui_state *g) {
   switch(pl.pclass) {
   case pcl_marine:    CBITab_Marine   (g); break;
   case pcl_cybermage: CBITab_CyberMage(g); break;
   case pcl_darklord:  CBITab_DarkLord (g); break;
   default:
      BeginPrintStrL("not implemented");
      PrintText(sf_smallfnt, rainbowcr(), g->ox+7,1, g->oy+47,1);
      break;
   }
}

/* EOF */
