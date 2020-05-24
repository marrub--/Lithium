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

static void SetupAttributes(struct player *p)
{
   fastmemmove(p->attr.names[at_acc], "ACC", 3);
   fastmemmove(p->attr.names[at_def], "DEF", 3);
   fastmemmove(p->attr.names[at_str], "STR", 3);
   fastmemmove(p->attr.names[at_vit], "VIT", 3);
   fastmemmove(p->attr.names[at_stm], "STM", 3);
   fastmemmove(p->attr.names[at_luk], "LUK", 3);

   switch(p->pclass) {
   case pcl_marine:    fastmemmove(p->attr.names[at_spc], "RGE", 3); break;
   case pcl_cybermage: fastmemmove(p->attr.names[at_spc], "CON", 3); break;
   case pcl_informant: fastmemmove(p->attr.names[at_spc], "ADR", 3); break;
   case pcl_wanderer:  fastmemmove(p->attr.names[at_spc], "AGI", 3); break;
   case pcl_assassin:  fastmemmove(p->attr.names[at_spc], "RSH", 3); break;
   case pcl_darklord:  fastmemmove(p->attr.names[at_spc], "REF", 3); break;
   case pcl_thoth:     fastmemmove(p->attr.names[at_spc], "???", 3); break;
   }

   if(p->pclass & pcl_robot) {
      fastmemmove(p->attr.names[at_vit], "POT", 3);
      fastmemmove(p->attr.names[at_stm], "REP", 3);
   } else if(p->pclass & pcl_nonhuman) {
      fastmemmove(p->attr.names[at_vit], "POT", 3);
      fastmemmove(p->attr.names[at_stm], "REG", 3);
   }

   p->attr.expprev = 0;
   p->attr.expnext = 500;
   p->attr.level = 1;
}

static void SetPClass(struct player *p)
{
   __with(str cl = p->pcstr = ACS_GetActorClass(0);) {
           if(cl == so_MarinePlayer   ) p->pclass = pcl_marine;
      else if(cl == so_CyberMagePlayer) p->pclass = pcl_cybermage;
      else if(cl == so_InformantPlayer) p->pclass = pcl_informant;
      else if(cl == so_WandererPlayer ) p->pclass = pcl_wanderer;
      else if(cl == so_AssassinPlayer ) p->pclass = pcl_assassin;
      else if(cl == so_DarkLordPlayer ) p->pclass = pcl_darklord;
      else if(cl == so_ThothPlayer    ) p->pclass = pcl_thoth;
      else for(;;)
      {
         Log("Invalid player class detected, everything is going to explode!");
         ACS_Delay(1);
      }
   }

   p->discrim = P_Discrim(p->pclass);
   p->color   = P_Color(p->pclass);
}

/* Extern Functions -------------------------------------------------------- */

bool P_ButtonPressed(struct player *p, i32 bt)
{
   return p->buttons & bt && !(p->old.buttons & bt);
}

bool P_SetVel(struct player *p, k32 velx, k32 vely, k32 velz, bool add)
{
   if(add) p->velx += velx, p->vely += vely, p->velz += velz;
   else    p->velx  = velx, p->vely  = vely, p->velz  = velz;

   return ACS_SetActorVelocity(p->tid, velx, vely, velz, add, true);
}

void P_ValidateTID(struct player *p)
{
   if(ACS_ActivatorTID() == 0) {
      ACS_Thing_ChangeTID(0, p->tid = ACS_UniqueTID());
      Dbg_Log(log_dev, "set ptid from 0 to %i", p->tid);
   } else if(p->tid != ACS_ActivatorTID()) {
      Dbg_Log(log_dev, "set ptid from %i to %i", p->tid, ACS_ActivatorTID());
      p->tid = ACS_ActivatorTID();
   }
}

/* Update all of the player's data. */
void P_Dat_PTickPre(struct player *p)
{
   Str(c_card_b, s"ChexBlueCard");
   Str(c_card_r, s"ChexRedCard");
   Str(c_card_y, s"ChexYellowCard");
   Str(d_card_b, s"BlueCard");
   Str(d_card_r, s"RedCard");
   Str(d_card_y, s"YellowCard");
   Str(d_skul_b, s"BlueSkull");
   Str(d_skul_r, s"RedSkull");
   Str(d_skul_y, s"YellowSkull");
   Str(htic_k_b, s"KeyBlue");
   Str(htic_k_g, s"KeyGreen");
   Str(htic_k_y, s"KeyYellow");

   enum {
      _warpflags = WARPF_NOCHECKPOSITION | WARPF_MOVEPTR |
                   WARPF_WARPINTERPOLATION | WARPF_COPYINTERPOLATION |
                   WARPF_COPYPITCH
   };

   p->grabInput = false;

   ACS_Warp(p->cameratid,  4, 0, ACS_GetActorViewHeight(0), 0, _warpflags);
   ACS_Warp(p->weathertid, 4, 0, ACS_GetActorViewHeight(0), 0, _warpflags);

   p->x = GetX(0);
   p->y = GetY(0);
   p->z = GetZ(0);

   p->velx = ACS_GetActorVelX(0);
   p->vely = ACS_GetActorVelY(0);
   p->velz = ACS_GetActorVelZ(0);

   p->pitch = ACS_GetActorPitch(0) - p->addpitch;
   p->yaw   = ACS_GetActorAngle(0) - p->addyaw;
   p->roll  = ACS_GetActorRoll (0) - p->addroll;

   p->pitchf = (-p->pitch + 0.25) * 2 * pi;
   p->yawf   = p->yaw * tau - pi;

   p->pitchv = ACS_GetPlayerInputFixed(-1, INPUT_PITCH);
   p->yawv   = ACS_GetPlayerInputFixed(-1, INPUT_YAW);

   p->forwardv = ACS_GetPlayerInputFixed(-1, INPUT_FORWARDMOVE);
   p->sidev    = ACS_GetPlayerInputFixed(-1, INPUT_SIDEMOVE);
   p->upv      = ACS_GetPlayerInputFixed(-1, INPUT_UPMOVE);

   p->buttons = ACS_GetPlayerInput(-1, INPUT_BUTTONS);

   p->name        = (ACS_BeginPrint(), ACS_PrintName(p->num), ACS_EndStrParam());
   p->weaponclass = ACS_GetWeapon();

   p->scopetoken = InvNum(so_WeaponScopedToken);

   p->krc = InvNum(d_card_r) || InvNum(c_card_r) || InvNum(htic_k_g);
   p->kyc = InvNum(d_card_y) || InvNum(c_card_y) || InvNum(htic_k_y);
   p->kbc = InvNum(d_card_b) || InvNum(c_card_b) || InvNum(htic_k_b);
   p->krs = InvNum(d_skul_r);
   p->kys = InvNum(d_skul_y);
   p->kbs = InvNum(d_skul_b);

   if(ACS_Timer() > 4) {
      /**/ if(p->health < p->oldhealth) p->healthused += p->oldhealth - p->health;
      else if(p->health > p->oldhealth) p->healthsum  += p->health    - p->oldhealth;

      if(p->x != p->old.x) p->unitstravelled += fastabs(p->x - p->old.x);
      if(p->y != p->old.y) p->unitstravelled += fastabs(p->y - p->old.y);
      if(p->z != p->old.z) p->unitstravelled += fastabs(p->z - p->old.z);
   }
}

script
static void LevelUp(struct player *p, u32 attr[at_max])
{
   u32 level = p->attr.level;

   for(i32 i = 0; i < at_max; i++) p->attr.attrs[i] += attr[i];

   for(i32 i = 0; i < 35 * 5; i++)
   {
      if(level != p->attr.level) return; /* a new levelup started, so exit */

      for(i32 j = 0; j < at_max; j++)
         if(i > 35*2 / (k32)at_max * j)
      {
         char *sp = p->attr.lvupstr;

         sp += sprintf(sp, "LEVEL %u", p->attr.level);
         if(p->attr.points) sprintf(sp, " (%u points)", p->attr.points);
         *sp++ = '\n';

         for(i32 k = 0, l = 0; k <= j; k++, l++)
         {
            /* skip over any +0 attributes */
            while(l < at_max && !attr[l]) l++;
            if(l >= at_max) break;

            sp += sprintf(sp, "%.3s +%u (%u)\n", p->attr.names[l], attr[l], p->attr.attrs[l]);
         }
      }

      ACS_Delay(1);
   }

   p->attr.lvupstr[0] = '\0';
}

void P_Lv_GiveEXP(struct player *p, u64 amt)
{
   struct player_attributes *a = &p->attr;

   u32 attr[at_max] = {};
   i32 levelup = 0;

   while(a->exp + amt >= a->expnext)
   {
      a->level++;
      a->expprev = a->expnext;
      a->expnext = 500 + (a->level * powlk(1.385, a->level * 0.2) * 340);

      __with(i32 pts = 7;) switch(p->getCVarI(sc_player_lvsys))
      {
      case atsys_manual: a->points += 7; break;
      case atsys_hybrid:
         a->points += 2;
         pts       -= 2;
      case atsys_auto:
         for(i32 i = 0; i < pts; i++) attr[ACS_Random(0, 100) % at_max]++;
         levelup++;
         break;
      }
   }

   if(levelup) LevelUp(p, attr);

   a->exp += amt;
}

/* Reset some things on the player when they spawn. */
script void P_Init(struct player *p) {
   if(!p->wasinit) {
      fastmemset(p, 0, sizeof *p);

      p->active = true;
      p->num    = ACS_PlayerNumber();

      SetPClass(p);
      SetupAttributes(p);

      /* i cri tears of pain for APROP_SpawnHealth */
      p->viewheight  = ACS_GetActorViewHeight(0);
      p->jumpheight  = GetPropK(0, APROP_JumpZ);
      p->spawnhealth = GetMembI(0, sm_Health);
      p->maxhealth   = p->spawnhealth;
      p->discount    = 1.0;
      p->stepnoise   = StrParam("player/%S/step", p->classname);

      switch(ACS_GetPlayerInfo(p->num, PLAYERINFO_GENDER)) {
         case 0: p->pronoun = pro_male;   break;
         case 1: p->pronoun = pro_female; break;
         case 2: p->pronoun = pro_nb;     break;
         case 3: p->pronoun = pro_object; break;
      }
   }

   fastmemset(&p->old, 0, sizeof p->old);

   p->reinit = p->dead = false;

   /* If the map sets the TID early on, it could already be set here. */
   p->tid = 0;
   P_ValidateTID(p);

   if(p->cameratid)  ACS_Thing_Remove(p->cameratid);
   if(p->weathertid) ACS_Thing_Remove(p->weathertid);
   ACS_SpawnForced(so_CameraHax, 0, 0, 0, p->cameratid  = ACS_UniqueTID());
   ACS_SpawnForced(so_CameraHax, 0, 0, 0, p->weathertid = ACS_UniqueTID());

   if(get_bit(dbgflag, dbgf_score)) p->score = 0xFFFFFFFFFFFFFFFFll;

   /* Any linked lists on the player need to be initialized here. */
   ListDtor(&p->hudstrlist, true);

   /* pls not exit map with murder thingies out */
   /* is bad practice */
   ACS_SetPlayerProperty(0, false, PROP_INSTANTWEAPONSWITCH);
   SetPropK(0, APROP_ViewHeight, p->viewheight);
   InvTake(so_WeaponScopedToken, INT32_MAX);

   P_CBI_PMinit(p);

   p->frozen     = 0;
   p->semifrozen = 0;

   p->addpitch = 0;
   p->addyaw   = 0;
   p->addroll  = 0;

   p->bobpitch = 0;
   p->bobyaw   = 0;

   p->extrpitch = 0;
   p->extryaw   = 0;

   p->scoreaccum     = 0;
   p->scoreaccumtime = 0;
   p->scoremul       = 1.3;

   ServCallI(sm_PlayerInit);

   p->alpha = 1;

   p->advice = snil;

   p->attr.lvupstr[0] = '\0';

   if(!p->bip.init) P_BIP_PInit(p);

   if(!p->upgrinit) P_Upg_PInit(p);
   else             P_Upg_PMInit(p);

   if(!p->invinit) P_Inv_PInit(p);

   if(!p->wasinit) {
      p->logB(1, Version " :: Compiled %s", __DATE__);

      if(dbglevel) {
         p->logH(1, "player is %u bytes long!", sizeof *p * 4);
         p->logH(1, "snil is \"%S\"", snil);
         PrintDmonAllocSize(p);
      } else {
         p->logH(1, LC(LANG "LOG_StartGame"), sc_k_opencbi);
      }

      if(GetFun() & lfun_division) {
         Str(divsigil, sOBJ "DivisionSigil");
         k32 a = ACS_GetActorAngle(0);
         k32 x = GetX(0) + ACS_Cos(a) * 128.0;
         k32 y = GetY(0) + ACS_Sin(a) * 128.0;
         k32 z = GetZ(0);
         ACS_SpawnForced(divsigil, x, y, z);
      }

      p->wasinit = true;
   }

   i32 minhealth = ACS_GetCVar(sc_sv_minhealth);
   if(p->health < minhealth) {
      p->health = minhealth;
   }

   if(get_bit(dbgflag, dbgf_items)) {
      for(i32 i = weapon_min; i < weapon_max; i++) {
         struct weaponinfo const *info = &weaponinfo[i];
         if(info->classname != snil && info->pclass & p->pclass && !get_bit(info->flags, wf_magic))
            InvGive(info->classname, 1);
      }
   }
}

void P_Dat_PTickPst(struct player *p)
{
   k32 boost = 1 + p->jumpboost;

   if(p->frozen != p->old.frozen)
      ACS_SetPlayerProperty(0, p->frozen > 0, PROP_TOTALLYFROZEN);

   if(p->speedmul != p->old.speedmul)
      SetPropK(0, APROP_Speed, 0.7 + p->speedmul);

   if(p->jumpboost != p->old.jumpboost)
      SetPropK(0, APROP_JumpZ, p->jumpheight * boost);
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "KeyDown")
void Sc_KeyDown(i32 pnum, i32 ch)
{
   with_player(&players[pnum])
      if(p->tbptr + 1 < countof(p->txtbuf))
         p->txtbuf[p->tbptr++] = ch;
}

/* EOF */
