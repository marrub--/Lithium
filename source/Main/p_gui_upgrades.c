// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"
#include "lith_world.h"

static __str const upgrcateg[UC_MAX] = {
   [UC_Body] = "LITH_CAT_BODY",
   [UC_Weap] = "LITH_CAT_WEAP",
   [UC_Extr] = "LITH_CAT_EXTR",
   [UC_Down] = "LITH_CAT_DOWN"
};

static void GUIUpgradesList(gui_state_t *g, struct player *p)
{
   if(Lith_GUI_Button(g, .x = 90, 213, Pre(btnprev)))
      if(CBIState(g)->upgrfilter-- <= 0)
         CBIState(g)->upgrfilter = UC_MAX;

   if(Lith_GUI_Button(g, .x = 90 + guipre.btnprev.w, 213, Pre(btnnext)))
      if(CBIState(g)->upgrfilter++ >= UC_MAX)
         CBIState(g)->upgrfilter = 0;

   int numbtns = p->upgrmax + UC_MAX;
   int filter  = CBIState(g)->upgrfilter - 1;

   if(filter != -1)
   {
      numbtns = 0;
      for(int i = 0; i < p->upgrmax; i++)
         if(p->upgrades[i].info->category == filter)
            numbtns++;

      PrintTextFmt("Filter: %S", L(upgrcateg[filter]));
   }
   else
      PrintTextStr("Filter: \CjAll");
   PrintText("cbifont", CR_WHITE, 15,1, 215,1);

   Lith_GUI_ScrollBegin(g, &CBIState(g)->upgrscr, 15, 36, guipre.btnlist.w, 178, guipre.btnlist.h * numbtns);

   int curcategory = -1;

   for(int i = 0, y = -guipre.btnlist.h; i < p->upgrmax; i++)
   {
      upgrade_t *upgr = &p->upgrades[i];

      if(filter != -1)
         {if(upgr->info->category != filter) continue;}
      else if(upgr->info->category != curcategory)
      {
         curcategory = upgr->info->category;
         y += guipre.btnlist.h;
         PrintTextStr(L(upgrcateg[curcategory]));
         PrintText("cbifont", CR_WHITE, g->ox + 4,1, y + g->oy + 1,1);
      }

      y += guipre.btnlist.h;

      if(Lith_GUI_ScrollOcclude(g, &CBIState(g)->upgrscr, y, guipre.btnlist.h))
         continue;

      char const *color;
      if(!upgr->owned && !p->canBuy(&upgr->info->shopdef, upgr))
         color = c"u";
      else switch(upgr->info->key)
      {
      case UPGR_TorgueMode: color = c"g"; break;
      case UPGR_DarkCannon: color = c"m"; break;
      default:              color = null;
      }

      gui_button_preset_t const *preset;
           if(upgr->active) preset = &guipre.btnlistactivated;
      else if(upgr->owned)  preset = &guipre.btnlistactive;
      else                  preset = &guipre.btnlistsel;

      char *name = LanguageC(c"LITH_UPGRADE_TITLE_%S", upgr->info->name);

      int *upgrsel = &CBIState(g)->upgrsel;
      if(Lith_GUI_Button_Id(g, i, name, 0, y, i == *upgrsel, .color = color, .preset = preset))
         *upgrsel = i;
   }

   Lith_GUI_ScrollEnd(g, &CBIState(g)->upgrscr);
}

static void GUIUpgradeRequirements(gui_state_t *g, struct player *p, upgrade_t *upgr)
{
   int y = 0;

   #define Req(name) \
   { \
      PrintTextStr("Requires " name "."); \
      PrintText("cbifont", CR_RED, 111,1, 200 + y,2); \
      y -= 10; \
   }

   if(CheckRequires_AI)  Req("Armor Interface")
   if(CheckRequires_WMD) Req("Weapon Modification Device")
   if(CheckRequires_WRD) Req("Weapon Refactoring Device")
   if(CheckRequires_RDI) Req("Reality Distortion Interface")
   if(CheckRequires_RA)  Req("Reactive Armor")

   #undef Req

   // Performance rating
   if(upgr->info->perf && p->pclass != pcl_cybermage)
   {
      bool over = upgr->info->perf + p->cbi.pruse > world.cbiperf;

      if(upgr->active)
         PrintTextFmt("Disabling saves \Cn%i\CbPerf\C-.", upgr->info->perf);
      else if(over)
         PrintTextFmt("Activating requires \Ca%i\CbPerf\C-.", upgr->info->perf);
      else
         PrintTextFmt("Activating will use \Cj%i\CbPerf\C-.", upgr->info->perf);

      PrintText("cbifont", CR_WHITE, 111,1, 200 + y,2);
      y -= 10;
   }

   // Score multiplier
   if(upgr->info->scoreadd != 0)
   {
      char  cr;
      __str op;
      bool  chk;

      if(upgr->active) {chk = upgr->info->scoreadd > 0; op = "Disabling";}
      else             {chk = upgr->info->scoreadd < 0; op = "Enabling" ;}

      int perc = abs(ceilk(100.0 * upgr->info->scoreadd));
      if(chk) {cr = 'a'; perc = 100 - perc;}
      else    {cr = 'n'; perc = 100 + perc;}

      PrintTextFmt("%S will multiply score by \C%c%i\C-%%", op, cr, perc);
      PrintText("cbifont", CR_WHITE, 111,1, 200 + y,2);
      y -= 10;
   }
}

static void GUIUpgradeDescription(gui_state_t *g, struct player *p, upgrade_t *upgr)
{
   SetClipW(111, 30, 190, 170, 184);

   // Cost
   __str mark;
   __str cost;

   switch(upgr->info->key)
   {
   case UPGR_lolsords:   mark = "\Cjfolds"; break;
   case UPGR_TorgueMode: mark = "\Cd$";     break;
   default:              mark = "\Cnscr";   break;
   }

   if(upgr->info->cost) cost = StrParam("%S%S", scoresep(p->getCost(&upgr->info->shopdef)), mark);
   else                 cost = L("LITH_FREE");

   PrintTextStr(cost);
   PrintText("cbifont", CR_WHITE, 111,1, 30,1);

   // Category
   PrintTextStr(L(upgrcateg[upgr->info->category]));
   PrintText("cbifont", CR_WHITE, 111,1, 40,1);

   // Effect
   ifauto(__str, effect, LanguageNull("LITH_UPGRADE_EFFEC_%S", upgr->info->name))
      PrintTextFmt("%S %S", L("LITH_EFFECT"), effect);

   static int const crs[] = {CR_RED, CR_ORANGE, CR_YELLOW, CR_GREEN, CR_BLUE, CR_PURPLE, CR_DARKRED};
   PrintText("cbifont",
      upgr->info->key == UPGR_UNCEUNCE ? crs[ACS_Timer() / 4 % countof(crs)] : CR_WHITE, 111,1, 50,1);

   ClearClip();
}

static void GUIUpgradeButtons(gui_state_t *g, struct player *p, upgrade_t *upgr)
{
   if(Lith_GUI_Button(g, LC(c"LITH_BUY"), 111, 205, !p->canBuy(&upgr->info->shopdef, upgr)))
      Lith_UpgrBuy(p, upgr, false);

   if(Lith_GUI_Button(g, upgr->active ? LC(c"LITH_DEACTIVATE") : LC(c"LITH_ACTIVATE"), 111 + guipre.btndef.w + 2, 205, !upgr->canUse(p)))
      upgr->toggle(p);
}

void Lith_CBITab_Upgrades(gui_state_t *g, struct player *p)
{
   GUIUpgradesList(g, p);

   upgrade_t *upgr = &p->upgrades[CBIState(g)->upgrsel];

   GUIUpgradeDescription (g, p, upgr);
   GUIUpgradeButtons     (g, p, upgr);
   GUIUpgradeRequirements(g, p, upgr);
}

// EOF
