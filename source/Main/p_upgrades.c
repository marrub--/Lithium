// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"
#include "lith_world.h"

// Static Objects ------------------------------------------------------------|

static upgradeinfo_t g_upgrinfoex[UPGR_EXTRA_NUM];

static upgradeinfo_t *g_upgrinfo;
static int g_upgrmax = UPGR_BASE_MAX;

// Static Functions ----------------------------------------------------------|

static bool Lith_UpgrCanBuy(struct player *p, shopdef_t const *, void *upgr)
{
   return !((upgrade_t *)upgr)->owned;
}

static void Lith_UpgrShopBuy(struct player *p, shopdef_t const *, void *upgr)
{
   ((upgrade_t *)upgr)->setOwned(p);
}

static bool Lith_UpgrGive(struct player *, shopdef_t const *, void *upgr_, int tid)
{
   upgrade_t const *upgr = upgr_;

   setmem(tid, "user_upgradeid", upgr->info->key);

   switch(upgr->info->category)
   {
   case UC_Body: setmem(tid, "user_upgradebody", true); break;
   case UC_Weap: setmem(tid, "user_upgradeweap", true); break;
   default:      setmem(tid, "user_upgradeextr", true); break;
   }

   return true;
}

static int Compg_upgrinfo(void const *lhs, void const *rhs)
{
   upgradeinfo_t const *u1 = lhs, *u2 = rhs;
   int c1 = u1->category - u2->category;
   if(c1 != 0) return c1;
   else        return u1->key - u2->key;
}

stkcall
static void SetDataPtr(struct player *p, upgrade_t *upgr)
{
   upgr->dataptr = &p->upgrdata;
}

// Extern Functions ----------------------------------------------------------|

upgradeinfo_t *Lith_UpgradeRegister(upgradeinfo_t const *upgr)
{
   upgradeinfo_t *ui = &g_upgrinfoex[g_upgrmax++ - UPGR_BASE_MAX];
   *ui = *upgr;
   return ui;
}

void Lith_GSReinit_Upgrade(void)
{
   for(int i = 0; i < g_upgrmax; i++)
   {
      upgradeinfo_t *ui = &g_upgrinfo[i];

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

void Lith_GSInit_Upgrade(void)
{
   g_upgrinfo = Calloc(g_upgrmax, sizeof(upgradeinfo_t));
   memmove(g_upgrinfo, upgrinfobase, sizeof(upgrinfobase));

   for(int i = 0; i < countof(g_upgrinfoex); i++)
      if(g_upgrinfoex[i].name != null)
         g_upgrinfo[UPGR_BASE_MAX + i] = g_upgrinfoex[i];

   qsort(g_upgrinfo, g_upgrmax, sizeof(upgradeinfo_t), Compg_upgrinfo);

   for(int i = 0; i < g_upgrmax; i++)
      g_upgrinfo[i].id = i;

   Lith_GSReinit_Upgrade();
}

void Lith_UpgrSetOwned(struct player *p, upgrade_t *upgr)
{
   if(upgr->owned) return;

   upgr->owned = true;
   p->upgradesowned++;

   if(upgr->info->category == UC_Body && upgr->info->cost == 0)
      upgr->toggle(p);
}

script
void Lith_PlayerInitUpgrades(struct player *p)
{
   #define CheckPClass() (g_upgrinfo[i].pclass & p->pclass)
   for(int i = 0; i < g_upgrmax; i++)
      if(CheckPClass())
         p->upgrmax++;

   upgrademap_t_ctor(&p->upgrademap, p->upgrmax, 1);
   memset(p->upgrades, 0, sizeof p->upgrades[0] * countof(p->upgrades));

   for(int i = 0, j = 0; i < g_upgrmax; i++)
      if(CheckPClass())
   {
      upgrade_t *upgr = &p->upgrades[j];

      g_upgrinfo[i].Init(p, upgr);
      upgr->info = &g_upgrinfo[i];

      p->upgrademap.insert(upgr);

      if(upgr->info->cost == 0 || world.dbgUpgr)
         Lith_UpgrBuy(p, upgr, true, true);

      j++;
   }

   p->upgrinit = true;
   #undef CheckPClass
}

void Lith_PlayerDeallocUpgrades(struct player *p)
{
   upgrademap_t_dtor(&p->upgrademap);
   p->upgrmax = 0;

   for(int i = 0; i < countof(p->upgrades); i++)
      memset(&p->upgrades[i], 0, sizeof p->upgrades[i]);

   p->upgrinit = false;
}

void Lith_PlayerDeinitUpgrades(struct player *p)
{
   ForUpgrade(upgr)
      if(upgr->active)
         upgr->wasactive = true,  upgr->toggle(p);
}

void Lith_PlayerReinitUpgrades(struct player *p)
{
   ForUpgrade(upgr)
      if(upgr->wasactive)
         upgr->wasactive = false, upgr->toggle(p);
}

script
void Lith_PlayerUpdateUpgrades(struct player *p)
{
   if(Lith_IsPaused)
      return;

   ForUpgrade(upgr)
      if(upgr->active && upgr->info->Update)
         upgr->info->Update(p, upgr);
}

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

void Lith_PlayerEnterUpgrades(struct player *p)
{
   ForUpgrade(upgr)
      if(upgr->active && upgr->info->Enter)
         upgr->info->Enter(p, upgr);
}

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

// EOF
