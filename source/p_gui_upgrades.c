/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Upgrades tab GUI.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"
#include "w_world.h"

cstr upgrcateg[] = {
   [UC_Body] = LANG "CAT_BODY",
   [UC_Weap] = LANG "CAT_WEAP",
   [UC_Extr] = LANG "CAT_EXTR",
   [UC_Down] = LANG "CAT_DOWN",
};

static void GUIUpgradesList(struct gui_state *g, struct player *p)
{
   if(G_Button(g, .x = 90, 213, Pre(btnprev)))
      if(CBIState(g)->upgrfilter-- <= 0)
         CBIState(g)->upgrfilter = UC_MAX;

   if(G_Button(g, .x = 90 + gui_p.btnprev.w, 213, Pre(btnnext)))
      if(CBIState(g)->upgrfilter++ >= UC_MAX)
         CBIState(g)->upgrfilter = 0;

   i32 numbtns = 0;
   i32 filter  = CBIState(g)->upgrfilter - 1;

   /* TODO */
   cstr filter_name = "All";

   if(filter != -1) {
      filter_name = LC(upgrcateg[filter]);

      for_upgrade(upgr)
         if(upgr->info->category == filter)
            numbtns++;
   } else {
      for_upgrade(upgr)
         numbtns++;
      numbtns += UC_MAX;
   }

   /* TODO */
   PrintTextFmt("Filter: %s", filter_name);
   PrintText(s_smallfnt, g->defcr, 15,1, 215,1);

   G_ScrollBegin(g, &CBIState(g)->upgrscr, 15, 36, gui_p.btnlist.w, 178, gui_p.btnlist.h * numbtns);

   i32 curcategory = -1;

   i32 y = -gui_p.btnlist.h;
   for_upgrade(upgr) {
      if(filter != -1) {
         if(upgr->info->category != filter) continue;
      } else if(upgr->info->category != curcategory) {
         curcategory = upgr->info->category;
         y += gui_p.btnlist.h;
         PrintTextChS(LC(upgrcateg[curcategory]));
         PrintText(s_lmidfont, g->defcr, g->ox + 40,4, y + g->oy + 1,1);
      }

      y += gui_p.btnlist.h;

      if(G_ScrollOcclude(g, &CBIState(g)->upgrscr, y, gui_p.btnlist.h))
         continue;

      cstr color;
      if(!get_bit(upgr->flags, _ug_owned) && !P_Shop_CanBuy(p, &upgr->info->shopdef, upgr)) {
         color = "u";
      } else {
         switch(upgr->info->key) {
            case UPGR_TorgueMode: color = "g"; break;
            case UPGR_DarkCannon: color = "m"; break;
            default:              color = nil; break;
         }
      }

      struct gui_pre_btn const *pre;
      /**/ if(get_bit(upgr->flags, _ug_active)) pre = &gui_p.btnlistactivated;
      else if(get_bit(upgr->flags, _ug_owned )) pre = &gui_p.btnlistactive;
      else                                      pre = &gui_p.btnlistsel;

      char *name = LanguageC(LANG "UPGRADE_TITLE_%S", upgr->info->name);

      i32 *upgrsel = &CBIState(g)->upgrsel;
      if(G_Button_Id(g, _i, name, 0, y, _i == *upgrsel, .color = color, .preset = pre))
         *upgrsel = _i;

      for(i32 i = 0; i < 4; i++) {
         StrAry(gfxs,
                s":UI:Group1", s":UI:Group2", s":UI:Group3", s":UI:Group4");
         if(get_bit(upgr->agroups, i)) {
            PrintSprite(gfxs[i], g->ox + pre->w - 9,1, g->oy + y + 1,1);
         }
      }
   }

   G_ScrollEnd(g, &CBIState(g)->upgrscr);
}

static void GUIUpgradeRequirements(struct gui_state *g, struct player *p, struct upgrade *upgr)
{
   i32 y = 0;

   #define Req(name) { \
      ACS_BeginPrint(); \
      PrintChrSt(LC(LANG name)); \
      ACS_PrintChar(' '); \
      PrintChrSt(LC(LANG "REQUIRED")); \
      PrintText(s_smallfnt, CR_RED, 111,1, 200 + y,2); \
      y -= 10; \
   }

   if(RequiresButDontHave_AI)  Req("CBI_ArmorInter")
   if(RequiresButDontHave_WMD) Req("CBI_WeapnInter")
   if(RequiresButDontHave_WRD) Req("CBI_WeapnInte2")
   if(RequiresButDontHave_RDI) Req("CBI_RDistInter")
   if(RequiresButDontHave_RA)  Req("CBI_ReactArmor")

   #undef Req

   /* Performance rating */
   if(upgr->info->perf && p->pclass != pcl_cybermage)
   {
      bool over = upgr->info->perf + p->cbi.pruse > cbiperf;

      if(get_bit(upgr->flags, _ug_active))
         PrintTextFmt(LC(LANG "SHOP_DISABLE_SAVES"), upgr->info->perf);
      else if(over)
         PrintTextFmt(LC(LANG "SHOP_CANT_ACTIVATE"), upgr->info->perf);
      else
         PrintTextFmt(LC(LANG "SHOP_ACTIVATE_USES"), upgr->info->perf);

      PrintText(s_smallfnt, g->defcr, 111,1, 200 + y,2);
      y -= 10;
   }

   /* Score multiplier */
   if(upgr->info->scoreadd != 0)
   {
      char cr;
      cstr op;
      bool chk;

      if(get_bit(upgr->flags, _ug_active)) {
         chk = upgr->info->scoreadd > 0;
         op = LC(LANG "SHOP_MUL_DISABLE");
      } else {
         chk = upgr->info->scoreadd < 0;
         op = LC(LANG "SHOP_MUL_ENABLE");
      }

      i32 perc = fastabs(ceilk(100.0 * upgr->info->scoreadd));
      if(chk) {cr = 'a'; perc = 100 - perc;}
      else    {cr = 'n'; perc = 100 + perc;}

      PrintTextFmt(op, cr, perc);
      PrintText(s_smallfnt, g->defcr, 111,1, 200 + y,2);
      y -= 10;
   }
}

static void GUIUpgradeDescription(struct gui_state *g, struct player *p, struct upgrade *upgr)
{
   SetClipW(111, 30, 190, 170, 184);

   /* Cost */
   cstr mark;
   str cost;

   switch(upgr->info->key) {
   case UPGR_lolsords:   mark = "\Cjfolds"; break;
   case UPGR_TorgueMode: mark = "\Cd$";     break;
   default:              mark = "\Cnscr";   break;
   }

   if(upgr->info->cost) cost = StrParam("%s%s", scoresep(P_Shop_Cost(p, &upgr->info->shopdef)), mark);
   else                 cost = L(st_free);

   PrintText_str(cost, s_smallfnt, g->defcr, 111,1, 30,1);

   /* Category */
   PrintTextChS(LC(upgrcateg[upgr->info->category]));
   PrintText(s_smallfnt, g->defcr, 111,1, 40,1);

   /* Effect */
   ifauto(str, effect, LanguageNull(LANG "UPGRADE_EFFEC_%S", upgr->info->name))
      PrintTextFmt("%s %S", LC(LANG "EFFECT"), effect);

   static i32 const crs[] = {
      CR_RED, CR_ORANGE, CR_YELLOW, CR_GREEN, CR_BLUE, CR_PURPLE, CR_DARKRED
   };

   i32 cr = g->defcr;

   if(upgr->info->key == UPGR_UNCEUNCE)
      cr = crs[ACS_Timer() / 4 % countof(crs)];

   PrintText(s_smallfnt, cr, 111,1, 50,1);

   ClearClip();
}

static void GUIUpgradeButtons(struct gui_state *g, struct player *p, struct upgrade *upgr)
{
   /* Buy */
   if(G_Button(g, LC(LANG "BUY"), 111, 205, !P_Shop_CanBuy(p, &upgr->info->shopdef, upgr), .fill = {&CBIState(g)->buyfill, p->getCVarI(sc_gui_buyfiller)}))
      P_Upg_Buy(p, upgr, false);

   /* Activate */
   if(G_Button(g, get_bit(upgr->flags, _ug_active) ? LC(LANG "DEACTIVATE") : LC(LANG "ACTIVATE"), 111 + gui_p.btndef.w + 2, 205, !P_Upg_CanActivate(p, upgr)))
      P_Upg_Toggle(p, upgr);

   /* Groups */
   PrintTextChS(LC(LANG "AUTOGROUPS"));
   PrintText(s_smallfnt, g->defcr, 255,0, 205,0);

   for(i32 i = 0; i < 4; i++) {
      static i32 const crs[] = {CR_BRICK, CR_GREEN, CR_LIGHTBLUE, CR_GOLD};

      ACS_BeginPrint();
      ACS_PrintChar('1' + i);
      PrintText(s_lmidfont, crs[i], 220 + i * 20,2, 215,1);

      if(G_Checkbox_Id(g, i, get_bit(upgr->agroups, i), 221 + i * 20, 211)) {
         tog_bit(upgr->agroups, i);
         P_Data_Save(p);
      }
   }
}

void P_CBI_TabUpgrades(struct gui_state *g, struct player *p)
{
   GUIUpgradesList(g, p);

   if(CBIState(g)->upgrsel == UPGR_MAX) {
      for_upgrade(upgr) {
         if(get_bit(upgr->flags, _ug_available)) {
            CBIState(g)->upgrsel = _i;
            break;
         }
      }
   }

   struct upgrade *upgr = &p->upgrades[CBIState(g)->upgrsel];

   GUIUpgradeDescription (g, p, upgr);
   GUIUpgradeButtons     (g, p, upgr);
   GUIUpgradeRequirements(g, p, upgr);
}

/* EOF */
