// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Upgrades tab GUI.                                                        │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"
#include "w_world.h"

static
void GUIUpgradesList(struct gui_state *g) {
   if(G_Button(g, .x = 77, 200, Pre(btnprev))) {
      if(pl.cbi.st.upgrfilter-- <= 0) {
         pl.cbi.st.upgrfilter = _uc_max;
      }
   }

   if(G_Button(g, .x = 77 + gui_p.btnprev.w, 200, Pre(btnnext))) {
      if(pl.cbi.st.upgrfilter++ >= _uc_max) {
         pl.cbi.st.upgrfilter = 0;
      }
   }

   i32 numbtns = 0;
   i32 filter  = pl.cbi.st.upgrfilter - 1;

   str filter_name;

   if(filter != -1) {
      filter_name = ns(lang(sa_upgr_categ[filter]));

      for_upgrade(upgr) {
         if(upgr->category == filter) {
            numbtns++;
         }
      }
   } else {
      filter_name = sl_cat_all;

      for_upgrade(upgr) {
         numbtns++;
      }

      numbtns += _uc_max;
   }

   ACS_BeginPrint();
   _p(sl_cat_filter);
   _c(' ');
   _p(filter_name);
   PrintText(sf_smallfnt, g->defcr, g->ox+2,1, g->oy+202,1);

   G_ScrBeg(g, &pl.cbi.st.upgrscr, 2, 23, gui_p.btnlist.w, 178, gui_p.btnlist.h * numbtns);

   i32 curcategory = -1;

   i32 y = -gui_p.btnlist.h;
   for_upgrade(upgr) {
      if(filter != -1) {
         if(upgr->category != filter) continue;
      } else if(upgr->category != curcategory) {
         curcategory = upgr->category;
         y += gui_p.btnlist.h;
         PrintText_str(ns(lang(sa_upgr_categ[curcategory])), sf_lmidfont, g->defcr, g->ox + 40,4, y + g->oy + 1,1);
      }

      y += gui_p.btnlist.h;

      if(G_ScrOcc(g, &pl.cbi.st.upgrscr, y, gui_p.btnlist.h)) {
         continue;
      }

      i32 color;
      if(!get_bit(upgr->flags, _ug_owned) && !P_Shop_CanBuy(&upgr->shopdef)) {
         color = CR_DARKGRAY;
      } else {
         switch(upgr->key) {
            case UPGR_TorgueMode: color = CR_RED;   break;
            case UPGR_Cannon_C:   color = CR_BLACK; break;
            default:              color = -1;       break;
         }
      }

      struct gui_pre_btn const *pre;
      /**/ if(get_bit(upgr->flags, _ug_active)) pre = &gui_p.btnlistactivated;
      else if(get_bit(upgr->flags, _ug_owned )) pre = &gui_p.btnlistactive;
      else                                      pre = &gui_p.btnlistsel;

      i32 *upgrsel = &pl.cbi.st.upgrsel;
      str  upgrnam = lang(fast_strdup2(LANG "UPGRADE_TITLE_", upgr->name));
      if(!upgrnam) {
         upgrnam = fast_strdup(upgr->name);
         color = CR_BLACK;
      }
      if(G_Button_HId(g, _i, tmpstr(upgrnam), 0, y, _i == *upgrsel, .color = color, .preset = pre)) {
         *upgrsel = _i;
      }

      for(i32 i = 0; i < 4; i++) {
         if(get_bit(upgr->agroups, i)) {
            PrintSprite(sa_groups[i], g->ox + pre->w - 9,1, g->oy + y + 1,1, _u_alpha, 0.5k);
         }
      }
   }

   G_ScrEnd(g, &pl.cbi.st.upgrscr);
}

static
void GUIUpgradeRequirements(struct gui_state *g, struct upgrade *upgr) {
   i32 y = 0;

   #define Req(name) statement({ \
      ACS_BeginPrint(); \
      ACS_PrintString(name); \
      ACS_PrintChar(' '); \
      ACS_PrintString(sl_required); \
      PrintText(sf_smallfnt, CR_RED, g->ox+98,1, g->oy+187 + y,2); \
      y -= 10; \
   })

   i32 req = P_Upg_CheckReqs(upgr);
   if(get_bit(req, _ur_ai))  Req(sl_cbi_armorinter);
   if(get_bit(req, _ur_wmd)) Req(sl_cbi_weapninter);
   if(get_bit(req, _ur_wrd)) Req(sl_cbi_weapninte2);
   if(get_bit(req, _ur_rdi)) Req(sl_cbi_rdistinter);
   if(get_bit(req, _ur_dim)) Req(sl_cbi_dimdriver);
   if(get_bit(req, _ur_ra))  Req(sl_cbi_reactarmor);

   #undef Req

   /* Performance rating */
   if(upgr->perf && pl.pclass != pcl_cybermage) {
      bool over = upgr->perf + pl.cbi.pruse > wl.cbiperf;
      cstr fmt;

      if(get_bit(upgr->flags, _ug_active)) {
         fmt = tmpstr(sl_shop_disable_saves);
      } else if(over) {
         fmt = tmpstr(sl_shop_cant_activate);
      } else {
         fmt = tmpstr(sl_shop_activate_uses);
      }

      BeginPrintFmt(fmt, upgr->perf);
      PrintText(sf_smallfnt, g->defcr, g->ox+98,1, g->oy+187 + y,2);
      y -= 10;
   }

   /* Score multiplier */
   if(upgr->scoreadd != 0) {
      char cr;
      cstr op;
      bool chk;

      if(get_bit(upgr->flags, _ug_active)) {
         chk = upgr->scoreadd > 0;
         op  = tmpstr(sl_shop_mul_disable);
      } else {
         chk = upgr->scoreadd < 0;
         op  = tmpstr(sl_shop_mul_enable);
      }

      i32 perc = fastabs(upgr->scoreadd);
      if(chk) {cr = 'a'; perc = 100 - perc;}
      else    {cr = 'n'; perc = 100 + perc;}

      BeginPrintFmt(op, cr, perc);
      PrintText(sf_smallfnt, g->defcr, g->ox+98,1, g->oy+187 + y,2);
      y -= 10;
   }
}

static
void GUIUpgradeDescription(struct gui_state *g, struct upgrade *upgr) {
   G_Clip(g, 98, 17, 190, 170, 184);

   /* Cost */
   cstr mark;

   switch(upgr->key) {
   case UPGR_lolsords:   mark = "\Cjfolds"; break;
   case UPGR_TorgueMode: mark = "\Cd$";     break;
   default:              mark = "\Cnscr";   break;
   }

   PrintText_str(upgr->cost
      ? fast_strdup2(scoresep(upgr->shopdef.cost), mark)
      : sl_free, sf_smallfnt, g->defcr, g->ox+98,1, g->oy+17,1);

   /* Category */
   PrintText_str(ns(lang(sa_upgr_categ[upgr->category])), sf_smallfnt, g->defcr, g->ox+98,1, g->oy+27,1);

   /* Effect */
   ACS_BeginPrint();
   ifauto(str, effect, lang_discrim(fast_strdup2(LANG "UPGRADE_EFFEC_", upgr->name))) {
      ACS_PrintString(effect);
   }
   i32 cr = upgr->key != UPGR_UNCEUNCE ? g->defcr : rainbowcr();
   PrintText(sf_smallfnt, cr, g->ox+98,1, g->oy+37,1);
   G_ClipRelease(g);
}

static
void GUIUpgradeButtons(struct gui_state *g, struct upgrade *upgr) {
   /* Buy */
   if(G_Button(g, tmpstr(sl_buy), 98, 192, !P_Shop_CanBuy(&upgr->shopdef), .fill = &pl.cbi.st.buyfill)) {
      P_Upg_Buy(upgr, false);
   }

   /* Activate */
   if(G_Button(g, tmpstr(get_bit(upgr->flags, _ug_active) ?
                         sl_deactivate :
                         sl_activate),
               98 + gui_p.btndef.w + 2, 192, !P_Upg_CanActivate(upgr)))
   {
      P_Upg_Toggle(upgr);
   }

   /* Groups */
   PrintText_str(sl_autogroups, sf_smallfnt, g->defcr, g->ox+242,0, g->oy+192,0);

   for(i32 i = 0; i < 4; i++) {
      static
      i32 const crs[] = {CR_BRICK, CR_GREEN, CR_LIGHTBLUE, CR_GOLD};

      ACS_BeginPrint();
      ACS_PrintChar('1' + i);
      PrintText(sf_lmidfont, crs[i], g->ox+207 + i * 20,2, g->oy+202,1);

      if(G_ChkBox_HId(g, i, get_bit(upgr->agroups, i), 208 + i * 20, 198)) {
         tog_bit(upgr->agroups, i);
         P_Data_Save();
      }
   }
}

void P_CBI_TabUpgrades(struct gui_state *g) {
   GUIUpgradesList(g);

   if(pl.cbi.st.upgrsel == UPGR_MAX) {
      for_upgrade(upgr) {
         if(get_bit(upgr->flags, _ug_available)) {
            pl.cbi.st.upgrsel = _i;
            break;
         }
      }
   }

   struct upgrade *upgr = &pl.upgrades[pl.cbi.st.upgrsel];

   GUIUpgradeDescription (g, upgr);
   GUIUpgradeButtons     (g, upgr);
   GUIUpgradeRequirements(g, upgr);
}

/* EOF */
