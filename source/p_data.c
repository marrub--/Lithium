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

#include "m_engine.h"
#include "p_player.h"
#include "m_version.h"
#include "w_world.h"
#include "w_monster.h"

stkoff bool P_ButtonPressed(i32 bt) {
   return pl.buttons & bt && !(pl.old.buttons & bt);
}

stkoff bool P_ButtonHeld(i32 bt) {
   return pl.buttons & bt;
}

stkoff bool P_ButtonReleased(i32 bt) {
   return !(pl.buttons & bt) && pl.old.buttons & bt;
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

   pl.pitchv = ACS_GetPlayerInputFixed(-1, INPUT_PITCH);
   pl.yawv   = ACS_GetPlayerInputFixed(-1, INPUT_YAW);

   pl.forwardv = ACS_GetPlayerInputFixed(-1, INPUT_FORWARDMOVE);
   pl.sidev    = ACS_GetPlayerInputFixed(-1, INPUT_SIDEMOVE);
   pl.upv      = ACS_GetPlayerInputFixed(-1, INPUT_UPMOVE);

   pl.name = strp(ACS_PrintName(0));

   pl.scopetoken = ServCallI(sm_WeaponScoped);

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
   case pcl_darklord:  pl.speedmul = 45;  pl.jumpboost = 100; break;
   }

   i32 hudaspect = !CVarGetI(sc_hud_43aspect) ? ((i32)(((k32)ACS_GetScreenWidth() / (k32)ACS_GetScreenHeight()) * 240) - 320) / 2 : 0;
   pl.hudlpos = -hudaspect;
   pl.hudrpos = 320 + hudaspect;
   pl.hudtype = CVarGetI(sc_hud_type);
   if(pl.hudtype <= 0 || pl.hudtype > _hud_max) {
      pl.hudtype = pl.pclass;
   } else {
      --pl.hudtype;
   }
   pl.hudcolor = pl.hudtype != _hud_old ? P_Color(pl.hudtype) : CR_RED;
   pl.hudhppos = -hudaspect;
   switch(pl.hudtype) {
   default:
   case _hud_marine:    pl.hudhppos += 89;  break;
   case _hud_cybermage: pl.hudhppos += 65;  break;
   case _hud_informant: pl.hudhppos += 65;  break;
   case _hud_assassin:  pl.hudhppos += 85;  break;
   case _hud_darklord:  pl.hudhppos += 65;  break;
   case _hud_old:       pl.hudhppos += 155; break;
   }
   pl.hudtop = 200;
   switch(pl.hudtype) {
   case _hud_cybermage: pl.hudtop -= 5;  break;
   case _hud_assassin:  pl.hudtop += 5;  break;
   case _hud_old:       pl.hudtop += 30; break;
   }
}

script ext("ACS") addr(lsc_giveammo) bool chtf_give_ammo(cheat_params_t const params) {
   for(i32 i = 0; i < countof(sa_ammo_types); ++i) {
      InvGive(sa_ammo_types[i], INT_MAX);
   }
   return true;
}

struct cheat cht_give_ammo = cheat_s("pgfa", 0, chtf_give_ammo, "Fully ammunized");

script void P_EarlyInit(void) {
   pl.setActivator();
   if(!pl.wasinit) {
      fastmemset(&pl, 0, sizeof pl);
      pl.pclass = GetMembI(0, s"m_PClass");
      faststrcpy_str(pl.discrim, GetMembS(0, s"m_Discrim"));
   }
}

script void P_Init(void) {
   if(!pl.wasinit) {
      pl.color        = P_Color(pl.pclass);
      pl.attr.expprev = 0;
      pl.attr.expnext = _base_exp;
      pl.attr.level   = 1;
      pl.viewheight   = EDataI(_edt_viewheight);
      pl.attackheight = EDataI(_edt_attackheight);
      pl.jumpheight   = GetMembK(0, sm_JumpZ);
      pl.spawnhealth  = GetHealth(0);
      pl.maxhealth    = pl.spawnhealth;
   }
   fastmemset(&pl.old, 0, sizeof pl.old);
   pl.reinit = pl.dead = false;
   /* If the map sets the TID early on, it could already be set here. */
   pl.tid = 0;
   P_ValidateTID();
   #ifndef NDEBUG
   if(dbgflags(dbgf_score)) pl.score = SCR_MAX / 4;
   #endif
   SetViewHeight(0, pl.viewheight);
   P_CBI_PMinit();
   P_Upg_PMInit();
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
   pl.alpha = 1;
   pl.dlg.num = -1;
   if(!pl.wasinit) {
      P_BIP_PInit();
      P_Upg_PInit();
      P_Inv_PInit();
      P_LogH(1, tmpstr(sl_log_version), vernam, __DATE__);
      #ifndef NDEBUG
      if(dbglevel_any()) {
         P_LogH(1, "player is %u bytes long!", sizeof pl * 4);
         P_LogH(1, "snil is \"%S\"", snil);
         PrintDmonAllocSize();
      }
      #endif
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
         if(get_bit(info->pclass, pl.pclass)) {
            InvGive(info->classname, 1);
         }
      }
   }
   #endif
}

void P_Dat_PTick(void) {
   if(pl.frozen != pl.old.frozen) {
      ACS_SetPlayerProperty(0, pl.frozen > 0, PROP_TOTALLYFROZEN);
   }

   SetMembI(0, sm_Speed, pl.speedmul);
   SetMembK(0, sm_JumpZ, pl.jumpheight * (pl.jumpboost / 100.0k));
}

script_str ext("ACS") addr(OBJ "InputChar")
void Z_InputChar(i32 ch) {
   Cps_SetC(pl.tb.txtbuf, pl.tb.tbptr++, ch);
}

script_str ext("ACS") addr(OBJ "InputEnd")
void Z_InputEnd(i32 ch) {
   pl.tb.changed = true;
}

/* EOF */
