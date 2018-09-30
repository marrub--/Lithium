// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"
#include "lith_world.h"

#define ForUpgrade(name) \
   for(int _i = 0; _i < p->upgrmax; _i++) \
      __with(upgrade_t *name = &p->upgrades[_i];)

#define CheckRequires(a1, a2) (upgr->info->requires & a1 && !(a2))
#define CheckRequires_AI      CheckRequires(UR_AI,  world.cbiupgr[cupg_armorinter])
#define CheckRequires_WMD     CheckRequires(UR_WMD, world.cbiupgr[cupg_weapninter])
#define CheckRequires_WRD     CheckRequires(UR_WRD, world.cbiupgr[cupg_weapninte2])
#define CheckRequires_RDI     CheckRequires(UR_RDI, world.cbiupgr[cupg_rdistinter])
#define CheckRequires_RA      CheckRequires(UR_RA,  p->getUpgr(UPGR_ReactArmor)->owned)

// Static Objects ------------------------------------------------------------|

static upgradeinfo_t UpgrInfoEx[UPGR_EXTRA_NUM];

static __str const UpgrCateg[UC_MAX] = {
   [UC_Body] = "LITH_CAT_BODY",
   [UC_Weap] = "LITH_CAT_WEAP",
   [UC_Extr] = "LITH_CAT_EXTR",
   [UC_Down] = "LITH_CAT_DOWN"
};

static upgradeinfo_t *UpgrInfo;
static int UpgrMax = UPGR_BASE_MAX;

// Static Functions ----------------------------------------------------------|

//
// Lith_UpgrCanBuy
//
static bool Lith_UpgrCanBuy(struct player *p, shopdef_t const *, void *upgr)
{
   return !((upgrade_t *)upgr)->owned;
}

//
// Lith_UpgrShopBuy
//
static void Lith_UpgrShopBuy(struct player *p, shopdef_t const *, void *upgr)
{
   ((upgrade_t *)upgr)->setOwned(p);
}

//
// Lith_UpgrGive
//
static bool Lith_UpgrGive(struct player *, shopdef_t const *, void *upgr_, int tid)
{
   upgrade_t const *upgr = upgr_;

   ACS_SetUserVariable(tid, "user_upgradeid", upgr->info->key);

   switch(upgr->info->category)
   {
   case UC_Body: ACS_SetUserVariable(tid, "user_upgradebody", true); break;
   case UC_Weap: ACS_SetUserVariable(tid, "user_upgradeweap", true); break;
   default:      ACS_SetUserVariable(tid, "user_upgradeextr", true); break;
   }

   return true;
}

//
// CompUpgrInfo
//
static int CompUpgrInfo(void const *lhs, void const *rhs)
{
   upgradeinfo_t const *u1 = lhs, *u2 = rhs;
   int c1 = u1->category - u2->category;
   if(c1 != 0) return c1;
   else        return u1->key - u2->key;
}

//
// SetDataPtr
//
stkcall
static void SetDataPtr(struct player *p, upgrade_t *upgr)
{
   upgr->dataptr = &p->upgrdata;
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_UpgradeRegister_
//
upgradeinfo_t *Lith_UpgradeRegister(upgradeinfo_t const *upgr)
{
   upgradeinfo_t *ui = &UpgrInfoEx[UpgrMax++ - UPGR_BASE_MAX];
   *ui = *upgr;
   return ui;
}

//
// Lith_GSReinit_Upgrade
//
void Lith_GSReinit_Upgrade(void)
{
   for(int i = 0; i < UpgrMax; i++)
   {
      upgradeinfo_t *ui = &UpgrInfo[i];

      // Set up static function pointers
      ui->Init = SetDataPtr; // this is set again by UpgrReinit

      ui->shopBuy    = Lith_UpgrShopBuy;
      ui->shopCanBuy = Lith_UpgrCanBuy;
      ui->shopGive   = Lith_UpgrGive;

      // Set up individual upgrades' function pointers
      switch(ui->key)
      {
      #define Ret(n) continue;
      #define Fn_F(n, cb) ui->cb = Upgr_##n##_##cb;
      #define Fn_S(n, cb) Fn_F(n, cb)
      #include "lith_upgradefuncs.h"
         continue;
      }
   }
}

//
// Lith_GSInit_Upgrade
//
void Lith_GSInit_Upgrade(void)
{
   UpgrInfo = Calloc(UpgrMax, sizeof(upgradeinfo_t));
   memmove(UpgrInfo, upgrinfobase, sizeof(upgrinfobase));

   for(int i = 0; i < countof(UpgrInfoEx); i++)
      if(UpgrInfoEx[i].name != null)
         UpgrInfo[UPGR_BASE_MAX + i] = UpgrInfoEx[i];

   qsort(UpgrInfo, UpgrMax, sizeof(upgradeinfo_t), CompUpgrInfo);

   for(int i = 0; i < UpgrMax; i++)
      UpgrInfo[i].id = i;

   Lith_GSReinit_Upgrade();
}

//
// Lith_UpgrSetOwned
//
void Lith_UpgrSetOwned(struct player *p, upgrade_t *upgr)
{
   if(upgr->owned) return;

   upgr->owned = true;
   p->upgradesowned++;

   if(upgr->info->category == UC_Body && upgr->info->cost == 0)
      upgr->toggle(p);
}

//
// Lith_PlayerInitUpgrades
//
script
void Lith_PlayerInitUpgrades(struct player *p)
{
   #define CheckPClass() (UpgrInfo[i].pclass & p->pclass)
   for(int i = 0; i < UpgrMax; i++)
      if(CheckPClass())
         p->upgrmax++;

   upgrademap_t_ctor(&p->upgrademap, p->upgrmax, 1);
   memset(p->upgrades, 0, sizeof p->upgrades[0] * countof(p->upgrades));

   for(int i = 0, j = 0; i < UpgrMax; i++)
      if(CheckPClass())
   {
      upgrade_t *upgr = &p->upgrades[j];

      UpgrInfo[i].Init(p, upgr);
      upgr->info = &UpgrInfo[i];

      p->upgrademap.insert(upgr);

      if(upgr->info->cost == 0 || world.dbgUpgr)
         Lith_UpgrBuy(p, upgr, true, true);

      j++;
   }

   p->upgrinit = true;
   #undef CheckPClass
}

//
// Lith_PlayerDeallocUpgrades
//
void Lith_PlayerDeallocUpgrades(struct player *p)
{
   upgrademap_t_dtor(&p->upgrademap);
   p->upgrmax = 0;

   for(int i = 0; i < countof(p->upgrades); i++)
      memset(&p->upgrades[i], 0, sizeof p->upgrades[i]);

   p->upgrinit = false;
}

//
// Lith_PlayerDeinitUpgrades
//
void Lith_PlayerDeinitUpgrades(struct player *p)
{
   ForUpgrade(upgr)
      if(upgr->active)
         upgr->wasactive = true,  upgr->toggle(p);
}

//
// Lith_PlayerReinitUpgrades
//
void Lith_PlayerReinitUpgrades(struct player *p)
{
   ForUpgrade(upgr)
      if(upgr->wasactive)
         upgr->wasactive = false, upgr->toggle(p);
}

//
// Lith_PlayerUpdateUpgrades
//
script
void Lith_PlayerUpdateUpgrades(struct player *p)
{
   if(Lith_IsPaused)
      return;

   ForUpgrade(upgr)
      if(upgr->active && upgr->info->Update)
         upgr->info->Update(p, upgr);
}

//
// Lith_PlayerRenderUpgrades
//
script
void Lith_PlayerRenderUpgrades(struct player *p)
{
   ForUpgrade(upgr) if(upgr->active && upgr->info->Render)
   {
      ACS_SetHudSize(320, 240);
      ACS_SetHudClipRect(0, 0, 0, 0);
      SetSize(320, 240);
      ClearClip();
      upgr->info->Render(p, upgr);
   }
}

//
// Lith_PlayerEnterUpgrades
//
void Lith_PlayerEnterUpgrades(struct player *p)
{
   ForUpgrade(upgr)
      if(upgr->active && upgr->info->Enter)
         upgr->info->Enter(p, upgr);
}

//
// Lith_UpgrCanActivate
//
bool Lith_UpgrCanActivate(struct player *p, upgrade_t *upgr)
{
   if(!upgr->active)
   {
      if((p->pclass == pcl_marine &&
         CheckRequires_AI  ||
         CheckRequires_WMD ||
         CheckRequires_WRD ||
         CheckRequires_RDI ||
         CheckRequires_RA) ||
         p->cbi.pruse + upgr->info->perf > world.cbiperf)
      {
         return false;
      }
   }

   return upgr->owned;
}

//
// Lith_UpgrToggle
//
bool Lith_UpgrToggle(struct player *p, upgrade_t *upgr)
{
   if(!upgr->canUse(p)) return false;

   upgr->active = !upgr->active;

   if(upgr->active) p->cbi.pruse += upgr->info->perf;
   else             p->cbi.pruse -= upgr->info->perf;

   if(upgr->active && upgr->info->group)
      ForUpgrade(other)
         if(other != upgr && other->active && other->info->group == upgr->info->group)
            other->toggle(p);

   if(upgr->active)
   {
      if(upgr->info->Activate)
         upgr->info->Activate(p, upgr);

      p->scoremul += upgr->info->scoreadd;
   }
   else
   {
      if(upgr->info->Deactivate)
         upgr->info->Deactivate(p, upgr);

      p->scoremul -= upgr->info->scoreadd;
   }

   return true;
}

// GUI -----------------------------------------------------------------------|

//
// GUIUpgradesList
//
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

      PrintTextFmt("Filter: %S", L(UpgrCateg[filter]));
   }
   else
      PrintTextStr("Filter: \CjAll");
   PrintText("CBIFONT", CR_WHITE, 15,1, 215,1);

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
         PrintTextStr(L(UpgrCateg[curcategory]));
         PrintText("CBIFONT", CR_WHITE, g->ox + 4,1, y + g->oy + 1,1);
      }

      y += guipre.btnlist.h;

      if(Lith_GUI_ScrollOcclude(g, &CBIState(g)->upgrscr, y, guipre.btnlist.h))
         continue;

      __str color;
      if(!upgr->owned && !p->canBuy(&upgr->info->shopdef, upgr))
         color = "u";
      else switch(upgr->info->key)
      {
      case UPGR_TorgueMode: color = "g"; break;
      case UPGR_DarkCannon: color = "m"; break;
      default:              color = null;
      }

      gui_button_preset_t const *preset;
           if(upgr->active) preset = &guipre.btnlistactivated;
      else if(upgr->owned)  preset = &guipre.btnlistactive;
      else                  preset = &guipre.btnlistsel;

      __str name = Language("LITH_UPGRADE_TITLE_%S", upgr->info->name);

      int *upgrsel = &CBIState(g)->upgrsel;
      if(Lith_GUI_Button_Id(g, i, name, 0, y, i == *upgrsel, .color = color, .preset = preset))
         *upgrsel = i;
   }

   Lith_GUI_ScrollEnd(g, &CBIState(g)->upgrscr);
}

//
// GUIUpgradeRequirements
//
static void GUIUpgradeRequirements(gui_state_t *g, struct player *p, upgrade_t *upgr)
{
   int y = 0;

   #define Req(name) \
   { \
      PrintTextStr("Requires " name "."); \
      PrintText("CBIFONT", CR_RED, 111,1, 200 + y,2); \
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

      PrintText("CBIFONT", CR_WHITE, 111,1, 200 + y,2);
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
      PrintText("CBIFONT", CR_WHITE, 111,1, 200 + y,2);
      y -= 10;
   }
}

//
// GUIUpgradeDescription
//
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
   PrintText("CBIFONT", CR_WHITE, 111,1, 30,1);

   // Category
   PrintTextStr(L(UpgrCateg[upgr->info->category]));
   PrintText("CBIFONT", CR_WHITE, 111,1, 40,1);

   // Effect
   ifauto(__str, effect, LanguageNull("LITH_UPGRADE_EFFEC_%S", upgr->info->name))
      PrintTextFmt("%S %S", L("LITH_EFFECT"), effect);

   static int const crs[] = {CR_RED, CR_ORANGE, CR_YELLOW, CR_GREEN, CR_BLUE, CR_PURPLE, CR_DARKRED};
   PrintText("CBIFONT",
      upgr->info->key == UPGR_UNCEUNCE ? crs[ACS_Timer() / 4 % countof(crs)] : CR_WHITE, 111,1, 50,1);

   ClearClip();
}

//
// GUIUpgradeButtons
//
static void GUIUpgradeButtons(gui_state_t *g, struct player *p, upgrade_t *upgr)
{
   if(Lith_GUI_Button(g, L("LITH_BUY"), 111, 205, !p->canBuy(&upgr->info->shopdef, upgr)))
      Lith_UpgrBuy(p, upgr, false);

   if(Lith_GUI_Button(g, upgr->active ? L("LITH_DEACTIVATE") : L("LITH_ACTIVATE"), 111 + guipre.btndef.w + 2, 205, !upgr->canUse(p)))
      upgr->toggle(p);
}

//
// Lith_CBITab_Upgrades
//
void Lith_CBITab_Upgrades(gui_state_t *g, struct player *p)
{
   GUIUpgradesList(g, p);

   upgrade_t *upgr = &p->upgrades[CBIState(g)->upgrsel];

   GUIUpgradeDescription (g, p, upgr);
   GUIUpgradeButtons     (g, p, upgr);
   GUIUpgradeRequirements(g, p, upgr);
}

// EOF
