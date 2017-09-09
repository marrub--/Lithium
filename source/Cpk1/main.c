// Copyright © 2017 Graham Sanderson, all rights reserved.
// vim: columns=120
#include "lith_player.h"
#include "lith_world.h"
#include "lith_version.h"
#include "cpk1_upgrades.h"

// Static Objects ------------------------------------------------------------|

static upgradeinfo_t UpgrInfo[] = {
   {{"HeadsUpDis3", "HeadsUpDisp", 0}, pcl_informant, UC_Body, 1, -0.05, UG_HUD},
};

// Static Functions ----------------------------------------------------------|

static void SetDataPtr(player_t *p, upgrade_t *upgr)
{
   static struct cpk1_upgr_data upgrdata[MAX_PLAYERS];
   upgr->dataptr = &upgrdata[p->num];
}

static bool UpgrReinit(upgradeinfo_t *ui)
{
   ui->Init = SetDataPtr;

   switch(ui->key)
   {
   #define Ret(n) return true;
   #define Fn_F(n, cb) ui->cb = Upgr_##n##_##cb;
   #define Fn_S(n, cb) Fn_F(n, cb)
   #include "cpk1_upgradefuncs.h"
      return true;
   }

   return false;
}

static void GInit()
{
   extern void Cpk1_PlayerUpdate(player_t *p);

   LogDebug(log_dev, "CPK1 GINIT RUNNING");

   Lith_CbReg_UpgrReinit(UpgrReinit);
   Lith_CbReg_PlayerUpdate(Cpk1_PlayerUpdate);
}

static void GSInit()
{
   LogDebug(log_dev, "CPK1 GSINIT RUNNING");

   for(int i = 0; i < countof(UpgrInfo); i++) {
      UpgrInfo[i].key = i + UPGR_CPK1_BASE;
      Lith_UpgradeRegister(&UpgrInfo[i]);
   }
}

// Scripts -------------------------------------------------------------------|

[[__call("ScriptS"), __script("Open")]]
static void Cpk1_World()
{
   Lith_CheckAPIVersion();

   LogDebug(log_dev, "CPK1 OPEN");

   if(!world.gsinit)
      Lith_CbReg_GSInit(GSInit);

   Lith_CbReg_GInit(GInit);
}

// EOF
