/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
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

static str const upgrcateg[UC_MAX] = {
   [UC_Body] = sLANG "CAT_BODY",
   [UC_Weap] = sLANG "CAT_WEAP",
   [UC_Extr] = sLANG "CAT_EXTR",
   [UC_Down] = sLANG "CAT_DOWN"
};

static void GUIUpgradesList(struct gui_state *g, struct player *p)
{
   if(G_Button(g, .x = 90, 213, Pre(btnprev)))
      if(CBIState(g)->upgrfilter-- <= 0)
         CBIState(g)->upgrfilter = UC_MAX;

   if(G_Button(g, .x = 90 + gui_p.btnprev.w, 213, Pre(btnnext)))
      if(CBIState(g)->upgrfilter++ >= UC_MAX)
         CBIState(g)->upgrfilter = 0;

   i32 numbtns = p->upgrmax + UC_MAX;
   i32 filter  = CBIState(g)->upgrfilter - 1;

   if(filter != -1)
   {
      numbtns = 0;
      for(i32 i = 0; i < p->upgrmax; i++)
         if(p->upgrades[i].info->category == filter)
            numbtns++;

      PrintTextFmt("Filter: %S", L(upgrcateg[filter]));
   }
   else
      PrintTextStr(st_filter_all);
   PrintText(s_cbifont, CR_WHITE, 15,1, 215,1);

   G_ScrollBegin(g, &CBIState(g)->upgrscr, 15, 36, gui_p.btnlist.w, 178, gui_p.btnlist.h * numbtns);

   i32 curcategory = -1;

   for(i32 i = 0, y = -gui_p.btnlist.h; i < p->upgrmax; i++)
   {
      struct upgrade *upgr = &p->upgrades[i];

      if(filter != -1)
         {if(upgr->info->category != filter) continue;}
      else if(upgr->info->category != curcategory)
      {
         curcategory = upgr->info->category;
         y += gui_p.btnlist.h;
         PrintText_str(L(upgrcateg[curcategory]), s_cbifont, CR_WHITE, g->ox + 4,1, y + g->oy + 1,1);
      }

      y += gui_p.btnlist.h;

      if(G_ScrollOcclude(g, &CBIState(g)->upgrscr, y, gui_p.btnlist.h))
         continue;

      char const *color;
      if(!upgr->owned && !P_Shop_CanBuy(p, &upgr->info->shopdef, upgr))
         color = "u";
      else switch(upgr->info->key)
      {
      case UPGR_TorgueMode: color = "g"; break;
      case UPGR_DarkCannon: color = "m"; break;
      default:              color = nil; break;
      }

      struct gui_pre_btn const *preset;
           if(upgr->active) preset = &gui_p.btnlistactivated;
      else if(upgr->owned)  preset = &gui_p.btnlistactive;
      else                  preset = &gui_p.btnlistsel;

      char *name = LanguageC(LANG "UPGRADE_TITLE_%S", upgr->info->name);

      i32 *upgrsel = &CBIState(g)->upgrsel;
      if(G_Button_Id(g, i, name, 0, y, i == *upgrsel, .color = color, .preset = preset))
         *upgrsel = i;
   }

   G_ScrollEnd(g, &CBIState(g)->upgrscr);
}

static void GUIUpgradeRequirements(struct gui_state *g, struct player *p, struct upgrade *upgr)
{
   i32 y = 0;

   #define Req(name) \
   { \
      static str const text = s"Requires " name "."; \
      PrintText_str(text, s_cbifont, CR_RED, 111,1, 200 + y,2); \
      y -= 10; \
   }

   if(CheckRequires_AI)  Req("Armor Interface")
   if(CheckRequires_WMD) Req("Weapon Modification Device")
   if(CheckRequires_WRD) Req("Weapon Refactoring Device")
   if(CheckRequires_RDI) Req("Reality Distortion Interface")
   if(CheckRequires_RA)  Req("Reactive Armor")

   #undef Req

   /* Performance rating */
   if(upgr->info->perf && p->pclass != pcl_cybermage)
   {
      bool over = upgr->info->perf + p->cbi.pruse > world.cbiperf;

      if(upgr->active)
         PrintTextFmt("Disabling saves \Cn%i\CbPerf\C-.", upgr->info->perf); /* TODO */
      else if(over)
         PrintTextFmt("Activating requires \Ca%i\CbPerf\C-.", upgr->info->perf); /* TODO */
      else
         PrintTextFmt("Activating will use \Cj%i\CbPerf\C-.", upgr->info->perf); /* TODO */

      PrintText(s_cbifont, CR_WHITE, 111,1, 200 + y,2);
      y -= 10;
   }

   /* Score multiplier */
   if(upgr->info->scoreadd != 0)
   {
      char cr;
      str  op;
      bool chk;

      if(upgr->active) {chk = upgr->info->scoreadd > 0; op = st_disabling;} /* TODO */
      else             {chk = upgr->info->scoreadd < 0; op = st_enabling;}  /* TODO */

      i32 perc = fastabs(ceilk(100.0 * upgr->info->scoreadd));
      if(chk) {cr = 'a'; perc = 100 - perc;}
      else    {cr = 'n'; perc = 100 + perc;}

      PrintTextFmt("%S will multiply score by \C%c%i\C-%%", op, cr, perc); /* TODO */
      PrintText(s_cbifont, CR_WHITE, 111,1, 200 + y,2);
      y -= 10;
   }
}

static void GUIUpgradeDescription(struct gui_state *g, struct player *p, struct upgrade *upgr)
{
   SetClipW(111, 30, 190, 170, 184);

   /* Cost */
   char const *mark;
   str cost;

   switch(upgr->info->key) {
   case UPGR_lolsords:   mark = "\Cjfolds"; break;
   case UPGR_TorgueMode: mark = "\Cd$";     break;
   default:              mark = "\Cnscr";   break;
   }

   if(upgr->info->cost) cost = StrParam("%s%s", scoresep(P_Shop_Cost(p, &upgr->info->shopdef)), mark);
   else                 cost = L(st_free);

   PrintText_str(cost, s_cbifont, CR_WHITE, 111,1, 30,1);

   /* Category */
   PrintText_str(L(upgrcateg[upgr->info->category]), s_cbifont, CR_WHITE, 111,1, 40,1);

   /* Effect */
   ifauto(str, effect, LanguageNull(LANG "UPGRADE_EFFEC_%S", upgr->info->name))
      PrintTextFmt("%s %S", LC(LANG "EFFECT"), effect);

   static i32 const crs[] = {CR_RED, CR_ORANGE, CR_YELLOW, CR_GREEN, CR_BLUE, CR_PURPLE, CR_DARKRED};
   PrintText(s_cbifont,
      upgr->info->key == UPGR_UNCEUNCE ? crs[ACS_Timer() / 4 % countof(crs)] : CR_WHITE, 111,1, 50,1);

   ClearClip();
}

static void GUIUpgradeButtons(struct gui_state *g, struct player *p, struct upgrade *upgr)
{
   if(G_Button(g, LC(LANG "BUY"), 111, 205, !P_Shop_CanBuy(p, &upgr->info->shopdef, upgr)))
      P_Upg_Buy(p, upgr, false);

   if(G_Button(g, upgr->active ? LC(LANG "DEACTIVATE") : LC(LANG "ACTIVATE"), 111 + gui_p.btndef.w + 2, 205, !P_Upg_CanActivate(p, upgr)))
      P_Upg_Toggle(p, upgr);
}

void P_CBI_TabUpgrades(struct gui_state *g, struct player *p)
{
   GUIUpgradesList(g, p);

   struct upgrade *upgr = &p->upgrades[CBIState(g)->upgrsel];

   GUIUpgradeDescription (g, p, upgr);
   GUIUpgradeButtons     (g, p, upgr);
   GUIUpgradeRequirements(g, p, upgr);
}

/* EOF */
