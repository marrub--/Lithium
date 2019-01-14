// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// p_upgrades.c: Upgrade handling.

#include "u_common.h"
#include "w_world.h"

// Static Objects ------------------------------------------------------------|

static struct upgradeinfo g_upgrinfoex[UPGR_EXTRA_NUM];

static struct upgradeinfo *g_upgrinfo;
static i32 g_upgrmax = UPGR_BASE_MAX;

// Static Functions ----------------------------------------------------------|

static bool UpgrCanBuy(struct player *p, struct shopdef const *, void *upgr)
{
   return !((struct upgrade *)upgr)->owned;
}

static void UpgrShopBuy(struct player *p, struct shopdef const *, void *upgr)
{
   P_Upg_SetOwned(p, upgr);
}

static bool UpgrGive(struct player *, struct shopdef const *, void *upgr_, i32 tid)
{
   struct upgrade const *upgr = upgr_;

   SetMembI(tid, sm_UpgradeId, upgr->info->key);

   switch(upgr->info->category) {
   case UC_Body: SetMembI(tid, sm_UpgradeBody, true); break;
   case UC_Weap: SetMembI(tid, sm_UpgradeWeap, true); break;
   default:      SetMembI(tid, sm_UpgradeExtr, true); break;
   }

   return true;
}

static i32 CompUpgrInfo(void const *lhs, void const *rhs)
{
   struct upgradeinfo const *u1 = lhs, *u2 = rhs;
   i32 c1 = u1->category - u2->category;
   if(c1 != 0) return c1;
   else        return u1->key - u2->key;
}

stkcall
static void SetDataPtr(struct player *p, struct upgrade *upgr)
{
   upgr->dataptr = &p->upgrdata;
}

// Extern Functions ----------------------------------------------------------|

struct upgradeinfo *Upgr_Register(struct upgradeinfo const *upgr)
{
   struct upgradeinfo *ui = &g_upgrinfoex[g_upgrmax++ - UPGR_BASE_MAX];
   *ui = *upgr;
   return ui;
}

void Upgr_MInit(void)
{
   for(i32 i = 0; i < g_upgrmax; i++)
   {
      struct upgradeinfo *ui = &g_upgrinfo[i];

      // Set up static function pointers
      ui->Init = SetDataPtr; // this is set again by UpgrReinit

      ui->ShopBuy    = UpgrShopBuy;
      ui->ShopCanBuy = UpgrCanBuy;
      ui->ShopGive   = UpgrGive;

      // Set up individual upgrades' function pointers
      switch(ui->key)
      {
      #define Ret(n) continue;
      #define Fn_F(n, cb) ui->cb = Upgr_##n##_##cb;
      #define Fn_S(n, cb) Fn_F(n, cb)
      #include "u_func.h"
         continue;
      }
   }
}

void Upgr_GInit(void)
{
   g_upgrinfo = Calloc(g_upgrmax, sizeof *g_upgrinfo);
   memmove(g_upgrinfo, upgrinfobase, sizeof upgrinfobase);

   for(i32 i = 0; i < countof(g_upgrinfoex); i++)
      if(g_upgrinfoex[i].name)
         g_upgrinfo[UPGR_BASE_MAX + i] = g_upgrinfoex[i];

   qsort(g_upgrinfo, g_upgrmax, sizeof *g_upgrinfo, CompUpgrInfo);

   for(i32 i = 0; i < g_upgrmax; i++)
      g_upgrinfo[i].id = i;
}

void P_Upg_SetOwned(struct player *p, struct upgrade *upgr)
{
   if(upgr->owned) return;

   upgr->owned = true;
   p->upgradesowned++;

   if(upgr->info->category == UC_Body && upgr->info->cost == 0)
      P_Upg_Toggle(p, upgr);
}

script
void P_Upg_PInit(struct player *p)
{
   #define CheckPClass() (g_upgrinfo[i].pclass & p->pclass)
   for(i32 i = 0; i < g_upgrmax; i++)
      if(CheckPClass())
         p->upgrmax++;

   upgrademap_t_ctor(&p->upgrademap, p->upgrmax, 1);
   memset(p->upgrades, 0, sizeof p->upgrades[0] * countof(p->upgrades));

   for(i32 i = 0, j = 0; i < g_upgrmax; i++)
      if(CheckPClass())
   {
      struct upgrade *upgr = &p->upgrades[j];

      g_upgrinfo[i].Init(p, upgr);
      upgr->info = &g_upgrinfo[i];

      p->upgrademap.insert(upgr);

      if(upgr->info->cost == 0 || dbgflag & dbgf_upgr)
         P_Upg_Buy(p, upgr, true, true);

      j++;
   }

   p->upgrinit = true;
   #undef CheckPClass
}

void P_Upg_PQuit(struct player *p)
{
   upgrademap_t_dtor(&p->upgrademap);
   p->upgrmax = 0;

   for(i32 i = 0; i < countof(p->upgrades); i++)
      memset(&p->upgrades[i], 0, sizeof p->upgrades[i]);

   p->upgrinit = false;
}

void P_Upg_PDeinit(struct player *p)
{
   for_upgrade(upgr)
      if(upgr->active)
         upgr->wasactive = true,  P_Upg_Toggle(p, upgr);
}

void P_Upg_PMInit(struct player *p)
{
   for_upgrade(upgr)
      if(upgr->wasactive)
         upgr->wasactive = false, P_Upg_Toggle(p, upgr);
}

script
void P_Upg_PTick(struct player *p)
{
   if(Paused)
      return;

   for_upgrade(upgr)
      if(upgr->active && upgr->info->Update)
         upgr->info->Update(p, upgr);
}

script
void P_Upg_PTickPst(struct player *p)
{
   for_upgrade(upgr) if(upgr->active && upgr->info->Render)
   {
      ACS_SetHudSize(320, 240);
      ACS_SetHudClipRect(0, 0, 0, 0);
      SetSize(320, 240);
      ClearClip();
      upgr->info->Render(p, upgr);
   }
}

void P_Upg_Enter(struct player *p)
{
   for_upgrade(upgr)
      if(upgr->active && upgr->info->Enter)
         upgr->info->Enter(p, upgr);
}

bool P_Upg_CanActivate(struct player *p, struct upgrade *upgr)
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

bool P_Upg_Toggle(struct player *p, struct upgrade *upgr)
{
   if(!P_Upg_CanActivate(p, upgr)) return false;

   upgr->active = !upgr->active;

   if(upgr->active) p->cbi.pruse += upgr->info->perf;
   else             p->cbi.pruse -= upgr->info->perf;

   if(upgr->active && upgr->info->group)
      for_upgrade(other)
         if(other != upgr && other->active && other->info->group == upgr->info->group)
            P_Upg_Toggle(p, other);

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
