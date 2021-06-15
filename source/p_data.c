/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Player data tracking.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "m_version.h"
#include "w_world.h"
#include "w_monster.h"

/* Static Functions -------------------------------------------------------- */

static void SetupAttributes()
{
   fastmemcpy(pl.attr.names[at_acc], "ACC", 3);
   fastmemcpy(pl.attr.names[at_def], "DEF", 3);
   fastmemcpy(pl.attr.names[at_str], "STR", 3);
   fastmemcpy(pl.attr.names[at_vit], "VIT", 3);
   fastmemcpy(pl.attr.names[at_stm], "STM", 3);
   fastmemcpy(pl.attr.names[at_luk], "LUK", 3);

   switch(pl.pclass) {
   case pcl_marine:    fastmemcpy(pl.attr.names[at_spc], "RGE", 3); break;
   case pcl_cybermage: fastmemcpy(pl.attr.names[at_spc], "CON", 3); break;
   case pcl_informant: fastmemcpy(pl.attr.names[at_spc], "ADR", 3); break;
   case pcl_wanderer:  fastmemcpy(pl.attr.names[at_spc], "AGI", 3); break;
   case pcl_assassin:  fastmemcpy(pl.attr.names[at_spc], "RSH", 3); break;
   case pcl_darklord:  fastmemcpy(pl.attr.names[at_spc], "REF", 3); break;
   case pcl_thoth:     fastmemcpy(pl.attr.names[at_spc], "MNA", 3); break;
   }

   if(pl.pclass & pcl_robot) {
      fastmemcpy(pl.attr.names[at_vit], "POT", 3);
      fastmemcpy(pl.attr.names[at_stm], "REP", 3);
   } else if(pl.pclass & pcl_nonhuman) {
      fastmemcpy(pl.attr.names[at_vit], "POT", 3);
      fastmemcpy(pl.attr.names[at_stm], "REG", 3);
   }

   pl.attr.expprev = 0;
   pl.attr.expnext = 500;
   pl.attr.level = 1;
}

static void SetPClass()
{
   pl.pcstr = ACS_GetActorClass(0);

   /**/ if(pl.pcstr == so_MarinePlayer   ) pl.pclass = pcl_marine;
   else if(pl.pcstr == so_CyberMagePlayer) pl.pclass = pcl_cybermage;
   else if(pl.pcstr == so_InformantPlayer) pl.pclass = pcl_informant;
   else if(pl.pcstr == so_WandererPlayer ) pl.pclass = pcl_wanderer;
   else if(pl.pcstr == so_AssassinPlayer ) pl.pclass = pcl_assassin;
   else if(pl.pcstr == so_DarkLordPlayer ) pl.pclass = pcl_darklord;
   else if(pl.pcstr == so_ThothPlayer    ) pl.pclass = pcl_thoth;
   #ifndef NDEBUG
   else {
      Log("Invalid player class detected (%S (%p)), "
          "everything is going to explode!", pl.pcstr, pl.pcstr);
   }
   #endif

   pl.discrim = P_Discrim(pl.pclass);
   pl.color   = P_Color(pl.pclass);
}

/* Extern Functions -------------------------------------------------------- */

bool P_ButtonPressed(i32 bt)
{
   return pl.buttons & bt && !(pl.old.buttons & bt);
}

bool P_SetVel(k32 velx, k32 vely, k32 velz, bool add)
{
   if(add) pl.velx += velx, pl.vely += vely, pl.velz += velz;
   else    pl.velx  = velx, pl.vely  = vely, pl.velz  = velz;

   return ACS_SetActorVelocity(pl.tid, velx, vely, velz, add, true);
}

void P_ValidateTID()
{
   if(ACS_ActivatorTID() == 0) {
      ACS_Thing_ChangeTID(0, pl.tid = ACS_UniqueTID());
      Dbg_Log(log_dev, "set ptid from 0 to %i", pl.tid);
   } else if(pl.tid != ACS_ActivatorTID()) {
      Dbg_Log(log_dev, "set ptid from %i to %i", pl.tid, ACS_ActivatorTID());
      pl.tid = ACS_ActivatorTID();
   }
}

/* Update all of the player's data. */
void P_Dat_PTickPre()
{
   Str(so_c_card_b, s"ChexBlueCard");
   Str(so_c_card_r, s"ChexRedCard");
   Str(so_c_card_y, s"ChexYellowCard");
   Str(so_d_card_b, s"BlueCard");
   Str(so_d_card_r, s"RedCard");
   Str(so_d_card_y, s"YellowCard");
   Str(so_d_skul_b, s"BlueSkull");
   Str(so_d_skul_r, s"RedSkull");
   Str(so_d_skul_y, s"YellowSkull");
   Str(so_htic_k_b, s"KeyBlue");
   Str(so_htic_k_g, s"KeyGreen");
   Str(so_htic_k_y, s"KeyYellow");

   enum {
      _warpflags = WARPF_NOCHECKPOSITION | WARPF_MOVEPTR |
                   WARPF_WARPINTERPOLATION | WARPF_COPYINTERPOLATION |
                   WARPF_COPYPITCH
   };

   pl.grabInput = false;

   ACS_Warp(pl.cameratid,  4, 0, ACS_GetActorViewHeight(0), 0, _warpflags);
   ACS_Warp(pl.weathertid, 4, 0, ACS_GetActorViewHeight(0), 0, _warpflags);

   pl.x = GetX(0);
   pl.y = GetY(0);
   pl.z = GetZ(0);

   pl.velx = ACS_GetActorVelX(0);
   pl.vely = ACS_GetActorVelY(0);
   pl.velz = ACS_GetActorVelZ(0);

   pl.pitch = ACS_GetActorPitch(0) - pl.addpitch;
   pl.yaw   = ACS_GetActorAngle(0) - pl.addyaw;
   pl.roll  = ACS_GetActorRoll (0) - pl.addroll;

   pl.pitchf = (-pl.pitch + 0.25) * 2 * pi;
   pl.yawf   = pl.yaw * tau - pi;

   pl.pitchv = ACS_GetPlayerInputFixed(-1, INPUT_PITCH);
   pl.yawv   = ACS_GetPlayerInputFixed(-1, INPUT_YAW);

   pl.forwardv = ACS_GetPlayerInputFixed(-1, INPUT_FORWARDMOVE);
   pl.sidev    = ACS_GetPlayerInputFixed(-1, INPUT_SIDEMOVE);
   pl.upv      = ACS_GetPlayerInputFixed(-1, INPUT_UPMOVE);

   pl.buttons = ACS_GetPlayerInput(-1, INPUT_BUTTONS);

   pl.name        = (ACS_BeginPrint(), ACS_PrintName(pl.num), ACS_EndStrParam());
   pl.weaponclass = ACS_GetWeapon();

   pl.scopetoken = InvNum(so_WeaponScopedToken);

   pl.krc = InvNum(so_d_card_r) || InvNum(so_c_card_r) || InvNum(so_htic_k_g);
   pl.kyc = InvNum(so_d_card_y) || InvNum(so_c_card_y) || InvNum(so_htic_k_y);
   pl.kbc = InvNum(so_d_card_b) || InvNum(so_c_card_b) || InvNum(so_htic_k_b);
   pl.krs = InvNum(so_d_skul_r);
   pl.kys = InvNum(so_d_skul_y);
   pl.kbs = InvNum(so_d_skul_b);

   if(ACS_Timer() > 4) {
      /**/ if(pl.health < pl.oldhealth) pl.healthused += pl.oldhealth - pl.health;
      else if(pl.health > pl.oldhealth) pl.healthsum  += pl.health    - pl.oldhealth;

      if(pl.x != pl.old.x) pl.unitstravelled += fastabs(pl.x - pl.old.x);
      if(pl.y != pl.old.y) pl.unitstravelled += fastabs(pl.y - pl.old.y);
      if(pl.z != pl.old.z) pl.unitstravelled += fastabs(pl.z - pl.old.z);
   }
}

alloc_aut(0) script static
void LevelUp(u32 *attr, char **attrptrs) {
   u32 level = pl.attr.level;

   for(i32 i = 0; i < 35 * 5; i++) {
      if(level != pl.attr.level) {
         /* a new levelup started, so exit */
         goto done;
      }

      char **ptr = &attrptrs[i / (35 * 5 / at_max)];
      while(!*ptr) ptr++;
      pl.attr.lvupstrn = *ptr - pl.attr.lvupstr;

      ACS_Delay(1);
   }

   pl.attr.lvupstr[0] = '\0';
done:
   Dalloc(attr);
   Dalloc(attrptrs);
}

void P_Lv_GiveEXP(u64 amt) {
   struct player_attributes *a = &pl.attr;

   u32 *attr = Malloc(sizeof(u32) * at_max, _tag_huds);
   i32 levelup = 0;

   while(a->exp + amt >= a->expnext) {
      a->level++;
      a->expprev = a->expnext;
      a->expnext = 500 + a->level * powlk(1.385, a->level * 0.2) * 340;

      i32 pts = 7;
      switch(CVarGetI(sc_player_lvsys)) {
      case atsys_manual:
         a->points += 7;
         break;
      case atsys_hybrid:
         a->points += 2;
         pts       -= 2;
      case atsys_auto:
         for(i32 i = 0; i < pts; i++) attr[ACS_Random(0, 100) % at_max]++;
         levelup++;
         break;
      }
   }

   if(levelup) {
      char *sp = a->lvupstr;
      char **attrptrs = Malloc(sizeof(char *) * at_max, _tag_huds);

      sp += sprintf(sp, "LEVEL %u", a->level);

      if(a->points) {
         sp += sprintf(sp, " (%u points)", a->points);
      }

      *sp++ = '\n';

      for(i32 i = 0; i < at_max; i++) {
         a->attrs[i] += attr[i];
         if(attr[i]) {
            sp += sprintf(sp, "%.3s +%u (%u)\n",
                          a->names[i], attr[i], a->attrs[i]);
            attrptrs[i] = sp;
         }
      }

      LevelUp(attr, attrptrs);
   } else {
      Dalloc(attr);
   }

   a->exp += amt;
}

/* Reset some things on the player when they spawn. */
script void P_Init() {
   if(!pl.wasinit) {
      fastmemset(&pl, 0, sizeof pl);

      pl.active = true;
      pl.num    = ACS_PlayerNumber();

      SetPClass();
      SetupAttributes();

      pl.viewheight   = GetViewHeight();
      pl.attackheight = GetAttackHeight();
      pl.jumpheight   = GetMembK(0, sm_JumpZ);
      pl.spawnhealth  = GetHealth(0);
      pl.maxhealth    = pl.spawnhealth;
      pl.discount     = 1.0;
      pl.stepnoise    = StrParam("player/%S/step", pl.classname);

      switch(ACS_GetPlayerInfo(pl.num, PLAYERINFO_GENDER)) {
         case 0: pl.pronoun = pro_male;   break;
         case 1: pl.pronoun = pro_female; break;
         case 2: pl.pronoun = pro_nb;     break;
         case 3: pl.pronoun = pro_object; break;
      }
   }

   fastmemset(&pl.old, 0, sizeof pl.old);

   pl.reinit = pl.dead = false;

   /* If the map sets the TID early on, it could already be set here. */
   pl.tid = 0;
   P_ValidateTID();

   if(pl.cameratid)  ACS_Thing_Remove(pl.cameratid);
   if(pl.weathertid) ACS_Thing_Remove(pl.weathertid);
   ACS_SpawnForced(so_CameraHax, 0, 0, 0, pl.cameratid  = ACS_UniqueTID());
   ACS_SpawnForced(so_CameraHax, 0, 0, 0, pl.weathertid = ACS_UniqueTID());

   #ifndef NDEBUG
   if(get_bit(dbgflags, dbgf_score)) pl.score = 0xFFFFFFFFFFFFFFFFll;
   #endif

   /* Any linked lists on the player need to be initialized here. */
   ListDtor(&pl.hudstrlist, true);

   /* pls not exit map with murder thingies out */
   /* is bad practice */
   ACS_SetPlayerProperty(0, false, PROP_INSTANTWEAPONSWITCH);
   SetViewHeight(0, pl.viewheight);
   InvTake(so_WeaponScopedToken, INT32_MAX);

   P_CBI_PMinit();

   pl.frozen     = 0;
   pl.semifrozen = 0;

   pl.addpitch = 0;
   pl.addyaw   = 0;
   pl.addroll  = 0;

   pl.bobpitch = 0;
   pl.bobyaw   = 0;

   pl.extrpitch = 0;
   pl.extryaw   = 0;

   pl.scoreaccum     = 0;
   pl.scoreaccumtime = 0;
   pl.scoremul       = 1.3;

   ServCallI(sm_PlayerInit);

   pl.alpha = 1;

   pl.advice = snil;

   pl.attr.lvupstr[0] = '\0';

   if(!pl.bip.init) P_BIP_PInit();

   if(!pl.upgrinit) P_Upg_PInit();
   else             P_Upg_PMInit();

   if(!pl.invinit) P_Inv_PInit();

   if(!pl.wasinit) {
      pl.logH(1, LC(LANG "LOG_Version"), VersionName, __DATE__);

      #ifndef NDEBUG
      if(dbglevel) {
         pl.logH(1, "player is %u bytes long!", sizeof pl * 4);
         pl.logH(1, "snil is \"%S\"", snil);
         PrintDmonAllocSize();
      } else {
      #endif
         pl.logH(1, LC(LANG "LOG_StartGame"), sc_k_opencbi);
      #ifndef NDEBUG
      }
      #endif

      if(GetFun() & lfun_division) {
         Str(so_divsigil, sOBJ "DivisionSigil");
         k32 a = ACS_GetActorAngle(0);
         k32 x = GetX(0) + ACS_Cos(a) * 128.0;
         k32 y = GetY(0) + ACS_Sin(a) * 128.0;
         k32 z = GetZ(0);
         ACS_SpawnForced(so_divsigil, x, y, z);
      }

      pl.wasinit = true;
   }

   i32 minhealth = CVarGetI(sc_sv_minhealth);
   if(pl.health < minhealth) {
      pl.health = minhealth;
   }

   #ifndef NDEBUG
   if(get_bit(dbgflags, dbgf_items)) {
      for(i32 i = weapon_min; i < weapon_max; i++) {
         struct weaponinfo const *info = &weaponinfo[i];
         if(info->classname != snil && info->pclass & pl.pclass && !(info->defammotype & AT_Mana))
            InvGive(info->classname, 1);
      }
   }
   #endif
}

void P_Dat_PTickPst()
{
   k32 boost = 1 + pl.jumpboost;

   if(pl.frozen != pl.old.frozen)
      ACS_SetPlayerProperty(0, pl.frozen > 0, PROP_TOTALLYFROZEN);

   if(pl.speedmul != pl.old.speedmul)
      SetMembK(0, sm_Speed, 0.7 + pl.speedmul);

   if(pl.jumpboost != pl.old.jumpboost)
      SetMembK(0, sm_JumpZ, pl.jumpheight * boost);
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "KeyDown")
void Sc_KeyDown(i32 ch)
{
   if(!P_None())
      if(pl.tbptr + 1 < countof(pl.txtbuf))
         pl.txtbuf[pl.tbptr++] = ch;
}

/* EOF */
