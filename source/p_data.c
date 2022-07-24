// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Player data tracking.                                                    │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "m_version.h"
#include "w_world.h"
#include "w_monster.h"

static
void SetupAttributes(void) {
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

static
void SetPClass(void) {
   pl.pcstr = ACS_GetActorClass(0);

   #define set_pcl(name) pl.pclass = pcl_##name, pl.pclass_b = pcl_##name##_b
   /**/ if(pl.pcstr == so_MarinePlayer   ) set_pcl(marine);
   else if(pl.pcstr == so_CyberMagePlayer) set_pcl(cybermage);
   else if(pl.pcstr == so_InformantPlayer) set_pcl(informant);
   else if(pl.pcstr == so_WandererPlayer ) set_pcl(wanderer);
   else if(pl.pcstr == so_AssassinPlayer ) set_pcl(assassin);
   else if(pl.pcstr == so_DarkLordPlayer ) set_pcl(darklord);
   else if(pl.pcstr == so_ThothPlayer    ) set_pcl(thoth);
   else {
      Dbg_Err(_l("Invalid player class "), _p(pl.pcstr),
              _l(", everything is going to explode!"));
   }

   pl.discrim = P_Discrim(pl.pclass);
   pl.color   = P_Color(pl.pclass);
}

bool P_ButtonPressed(i32 bt) {
   return pl.buttons & bt && !(pl.old.buttons & bt);
}

bool P_SetVel(k32 velx, k32 vely, k32 velz, bool add) {
   if(add) pl.velx += velx, pl.vely += vely, pl.velz += velz;
   else    pl.velx  = velx, pl.vely  = vely, pl.velz  = velz;

   return ACS_SetActorVelocity(pl.tid, velx, vely, velz, add, true);
}

void P_ValidateTID(void) {
   if(ACS_ActivatorTID() == 0) {
      ACS_Thing_ChangeTID(0, pl.tid = ACS_UniqueTID());
      Dbg_Log(log_dev, _l("set ptid from 0 to "), _p(pl.tid));
   } else if(pl.tid != ACS_ActivatorTID()) {
      Dbg_Log(log_dev,
              _l("set ptid from "), _p(pl.tid), _l(" to "),
              _p(ACS_ActivatorTID()));
      pl.tid = ACS_ActivatorTID();
   }
}

/* Update all of the player's data. */
void P_Dat_PTickPre(void) {
   enum {
      _warpflags = WARPF_NOCHECKPOSITION | WARPF_MOVEPTR |
                   WARPF_WARPINTERPOLATION | WARPF_COPYINTERPOLATION |
                   WARPF_COPYPITCH
   };

   pl.x = GetX(0);
   pl.y = GetY(0);
   pl.z = GetZ(0);

   pl.velx = ACS_GetActorVelX(0);
   pl.vely = ACS_GetActorVelY(0);
   pl.velz = ACS_GetActorVelZ(0);

   pl.pitch = ACS_GetActorPitch(0);
   pl.yaw   = ACS_GetActorAngle(0);
   pl.roll  = ACS_GetActorRoll (0);

   pl.pitchf = (-pl.pitch + 0.25) * 2 * pi;
   pl.yawf   = pl.yaw * tau - pi;

   pl.pitchv = ACS_GetPlayerInputFixed(-1, INPUT_PITCH);
   pl.yawv   = ACS_GetPlayerInputFixed(-1, INPUT_YAW);

   pl.forwardv = ACS_GetPlayerInputFixed(-1, INPUT_FORWARDMOVE);
   pl.sidev    = ACS_GetPlayerInputFixed(-1, INPUT_SIDEMOVE);
   pl.upv      = ACS_GetPlayerInputFixed(-1, INPUT_UPMOVE);

   pl.buttons = ACS_GetPlayerInput(-1, INPUT_BUTTONS);

   pl.name        = strp(ACS_PrintName(0));
   pl.weaponclass = ACS_GetWeapon();

   pl.scopetoken = InvNum(so_WeaponScopedToken);

   if(ACS_Timer() > 4) {
      /**/ if(pl.health < pl.old.health) pl.healthused += pl.old.health - pl.health;
      else if(pl.health > pl.old.health) pl.healthsum  += pl.health     - pl.old.health;

      if(pl.x != pl.old.x) pl.unitstravelled += fastabs(pl.x - pl.old.x);
      if(pl.y != pl.old.y) pl.unitstravelled += fastabs(pl.y - pl.old.y);
      if(pl.z != pl.old.z) pl.unitstravelled += fastabs(pl.z - pl.old.z);
   }

   switch(pl.pclass) {
   default:            pl.speedmul = 100; pl.jumpboost = 100; break;
   case pcl_marine:    pl.speedmul = 70;  pl.jumpboost = 100; break;
   case pcl_cybermage: pl.speedmul = 70;  pl.jumpboost = 125; break;
   case pcl_informant: pl.speedmul = 100; pl.jumpboost = 175; break;
   case pcl_assassin:  pl.speedmul = 100; pl.jumpboost = 150; break;
   case pcl_darklord:  pl.speedmul = 50;  pl.jumpboost = 100; break;
   }
}

alloc_aut(0) script static
void LevelUp(i32 *attr, char **attrptrs) {
   i32 level = pl.attr.level;

   for(i32 i = 0; i < 35 * 5; ++i) {
      if(level != pl.attr.level) {
         /* a new levelup started, so exit */
         return;
      }

      char **ptr = &attrptrs[i / (35 * 5 / at_max)];
      while(!*ptr) ptr++;
      pl.attr.lvupstrn = *ptr - pl.attr.lvupstr;

      ACS_Delay(1);
   }

   pl.attr.lvupstr[0] = '\0';
}

void P_Lv_GiveEXP(i32 amt) {
   struct player_attributes *a = &pl.attr;

   noinit static
   i32 attr[at_max];
   fastmemset(attr, 0, sizeof attr);

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
         for(i32 i = 0; i < pts; ++i) attr[ACS_Random(0, 100) % at_max]++;
         levelup++;
         break;
      }
   }

   if(levelup) {
      char *sp = a->lvupstr;

      noinit static
      char *attrptrs[at_max];
      fastmemset(attrptrs, 0, sizeof attrptrs);

      sp += sprintf(sp, "LEVEL %u", a->level);

      if(a->points) {
         sp += sprintf(sp, " (%u points)", a->points);
      }

      *sp++ = '\n';

      for(i32 i = 0; i < at_max; ++i) {
         a->attrs[i] += attr[i];
         if(attr[i]) {
            sp += sprintf(sp, "%.3s +%u (%u)\n",
                          a->names[i], attr[i], a->attrs[i]);
            attrptrs[i] = sp;
         }
      }

      LevelUp(attr, attrptrs);
   }

   a->exp += amt;
}


script static
bool chtf_give_ammo(cheat_params_t const params) {
   for(i32 i = 0; i < countof(sa_ammo_types); ++i) {
      InvGive(sa_ammo_types[i], 0x7FFFFFFF);
   }
   return true;
}

struct cheat cht_give_ammo = cheat_s("pgfa", 0, chtf_give_ammo, "Fully ammunized");

/* Reset some things on the player when they spawn. */
script
void P_Init(void) {
   if(!pl.wasinit) {
      fastmemset(&pl, 0, sizeof pl);

      pl.active = true;

      SetPClass();
      SetupAttributes();

      pl.viewheight   = GetViewHeight();
      pl.attackheight = GetAttackHeight();
      pl.jumpheight   = GetMembK(0, sm_JumpZ);
      pl.spawnhealth  = GetHealth(0);
      pl.maxhealth    = pl.spawnhealth;
      pl.stepnoise    = fast_strdup3("player/", pl.discrim, "/step");
   }

   fastmemset(&pl.old, 0, sizeof pl.old);

   pl.reinit = pl.dead = false;

   /* If the map sets the TID early on, it could already be set here. */
   pl.tid = 0;
   P_ValidateTID();

   #ifndef NDEBUG
   if(dbgflags(dbgf_score)) pl.score = SCR_MAX / 4;
   #endif

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
   pl.bobroll  = 0;

   pl.extrpitch = 0;
   pl.extryaw   = 0;

   pl.scoreaccum     = 0;
   pl.scoreaccumtime = 0;
   pl.scoremul       = 110;

   ServCallV(sm_PlayerInit);

   pl.alpha = 1;

   pl.attr.lvupstr[0] = '\0';

   if(!bip.init) P_BIP_PInit();

   if(!pl.upgrinit) P_Upg_PInit();
   else             P_Upg_PMInit();

   if(!pl.invinit) P_Inv_PInit();

   if(!pl.wasinit) {
      pl.logH(1, tmpstr(lang(sl_log_version)), vernam, __DATE__);

      #ifndef NDEBUG
      if(dbglevel_any()) {
         pl.logH(1, "player is %u bytes long!", sizeof pl * 4);
         pl.logH(1, "snil is \"%S\"", snil);
         PrintDmonAllocSize();
      } else {
      #endif
         pl.logH(1, tmpstr(lang(sl_log_startgame)), sc_k_opencbi);
      #ifndef NDEBUG
      }
      #endif

      if(GetFun() & lfun_division) {
         k32 a = ACS_GetActorAngle(0);
         k32 x = GetX(0) + ACS_Cos(a) * 128.0;
         k32 y = GetY(0) + ACS_Sin(a) * 128.0;
         k32 z = GetZ(0);
         ACS_SpawnForced(so_DivisionSigil, x, y, z);
      }

      pl.wasinit = true;
   }

   if(pl.health < cv.sv_minhealth) {
      pl.setHealth(cv.sv_minhealth);
   }

   #ifndef NDEBUG
   if(dbgflags(dbgf_ammo)) {
      chtf_give_ammo(nil);
   }

   if(dbgflags(dbgf_items)) {
      for(i32 i = weapon_min; i < weapon_max; ++i) {
         struct weaponinfo const *info = &weaponinfo[i];
         if(info->classname != snil && info->pclass & pl.pclass) {
            InvGive(info->classname, 1);
         }
      }
   }
   #endif
}

void P_Dat_PTickPst(void) {
   if(pl.frozen != pl.old.frozen) {
      ACS_SetPlayerProperty(0, pl.frozen > 0, PROP_TOTALLYFROZEN);
   }

   SetMembI(0, sm_Speed, pl.speedmul);
   SetMembK(0, sm_JumpZ, pl.jumpheight * (pl.jumpboost / 100.0k));
}

script_str ext("ACS") addr(OBJ "InputChar")
void Z_InputChar(i32 ch) {
   if(!P_None()) Cps_SetC(pl.tb.txtbuf, pl.tb.tbptr++, ch);
}

script_str ext("ACS") addr(OBJ "InputEnd")
void Z_InputEnd(i32 ch) {
   if(!P_None()) pl.tb.changed = true;
}

/* EOF */
