//----------------------------------------------------------------------------
//
// Copyright (c) 2017 Project Golan
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//----------------------------------------------------------------------------
//
// LithUpgrades.h
//
// Upgrades API.
//
// Don't include this file manually, include <Lithium.h> instead.
//
//----------------------------------------------------------------------------

#include "LithUpgradeCategory.h"

#define UPGR_STATIC_MAX 80
#define UPGR_EXTRA_NUM (UPGR_STATIC_MAX - UPGR_BASE_MAX)

#define NUMAUTOGROUPS 4

#define UserData_Adrenaline data.u01
#define UserData_Implying   data.u02
#define UserData_JetBooster data.u03
#define UserData_lolsords   data.u04
#define UserData_ReactArmor data.u05
#define UserData_ReflexWetw data.u06
#define UserData_VitalScan  data.u07
#define UserData_Zoom       data.u08
#define UserData_HomingRPG  data.u09
#define UserData_Custom     data.u10

                      typedef void (*upgr_cb_t)       (struct player_s *, struct upgrade_s *);
[[__call("ScriptS")]] typedef void (*upgr_update_cb_t)(struct player_s *, struct upgrade_s *);

enum
{
   UG_None,
   UG_Pistol,
   UG_Shotgun,
   UG_Launcher,
   UG_Plasma,
   UG_BFG,
   UG_BASE_MAX
};

enum
{
   UR_AI  = 1 << 0,
   UR_WMD = 1 << 1,
   UR_WRD = 1 << 2,
   UR_RDI = 1 << 3,
   UR_RA  = 1 << 4,
};

union upgradedata_u
{
   // Private Data
   struct
   {
      int charge;
      bool readied;
   } u01;
   
   struct {int hudid;}        u02;
   struct {int charge;}       u03;
   struct {__str origweapon;} u04;
   struct {int activearmor;}  u05;
   
   struct
   {
      int charge;
      bool leaped;
   } u06;
   
   struct
   {
      int target;
      int oldtarget;
      __str tagstr;
      int health;
      int oldhealth;
      int maxhealth;
      float angle;
      float old;
      bool freak;
   } u07;
   
   struct
   {
      int zoom;
      float vzoom;
   } u08;
   
   struct {int id;} u09;
   
   // Public Data
   struct {void *userdata;} u10;
};

typedef struct upgradeinfo_s
{
   // Public Data
   [[__anonymous]] shopdef_t shopdef;
   int   category;
   int   perf;
   fixed scoreadd;
   int   group;
   unsigned requires;
   
   // Readonly Data
   int id, key;
   
   upgr_cb_t        Activate;
   upgr_cb_t        Deactivate;
   upgr_update_cb_t Update;
   upgr_cb_t        Render;
   upgr_cb_t        Enter;
} upgradeinfo_t;


typedef struct upgrade_s
{
   // Public Data
   union upgradedata_u data;
   
   // Readonly Data
   bool autogroups[NUMAUTOGROUPS];
   upgradeinfo_t const *info;
   
   bool active : 1;
   bool owned  : 1;
   
   // Private Data
   bool wasactive : 1; // for reinitializing on map load
} upgrade_t;

void Lith_LoadUpgrInfoBalance(upgradeinfo_t *uinfo, int max, char const *fname);
[[__call("ScriptS"), __extern("ACS")]]
void Lith_RegisterBasicUpgrade(int key, __str name, int cost, int category);

// EOF

