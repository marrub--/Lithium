// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_version.h"
#include "lith_world.h"
#include "lith_monster.h"

// Static Functions ----------------------------------------------------------|

static void SetupAttributes(struct player *p)
{
   p->attr.names[at_acc] = c"ACC";
   p->attr.names[at_def] = c"DEF";
   p->attr.names[at_str] = c"STR";
   p->attr.names[at_vit] = c"VIT";
   p->attr.names[at_stm] = c"STM";
   p->attr.names[at_luk] = c"LUK";

   switch(p->pclass) {
   case pcl_marine:    p->attr.names[at_spc] = c"RGE"; break;
   case pcl_cybermage: p->attr.names[at_spc] = c"CON"; break;
   case pcl_informant: p->attr.names[at_spc] = c"ADR"; break;
   case pcl_wanderer:  p->attr.names[at_spc] = c"AGI"; break;
   case pcl_assassin:  p->attr.names[at_spc] = c"RSH"; break;
   case pcl_darklord:  p->attr.names[at_spc] = c"REF"; break;
   case pcl_thoth:     p->attr.names[at_spc] = c"???"; break;
   }

   if(p->pclass & pcl_robot) {
      p->attr.names[at_vit] = c"POT";
      p->attr.names[at_stm] = c"REP";
   } else if(p->pclass & pcl_nonhuman) {
      p->attr.names[at_vit] = c"POT";
      p->attr.names[at_stm] = c"REG";
   }

   p->attr.expnext = 500;
   p->attr.level = 1;
}

static void SetPClass(struct player *p)
{
   __with(__str cl = p->pcstr = ACS_GetActorClass(0);) {
           if(cl == "Lith_MarinePlayer"   ) p->pclass = pcl_marine;
      else if(cl == "Lith_CyberMagePlayer") p->pclass = pcl_cybermage;
      else if(cl == "Lith_InformantPlayer") p->pclass = pcl_informant;
      else if(cl == "Lith_WandererPlayer" ) p->pclass = pcl_wanderer;
      else if(cl == "Lith_AssassinPlayer" ) p->pclass = pcl_assassin;
      else if(cl == "Lith_DarkLordPlayer" ) p->pclass = pcl_darklord;
      else if(cl == "Lith_ThothPlayer"    ) p->pclass = pcl_thoth;
      else for(;;)
      {
         Log("Invalid player class detected, everything is going to explode!");
         ACS_Delay(1);
      }
   }
}

// Extern Functions ----------------------------------------------------------|

stkcall
bool Lith_ButtonPressed(struct player *p, int bt)
{
   return p->buttons & bt && !(p->old.buttons & bt);
}

stkcall
bool Lith_SetPlayerVelocity(struct player *p, fixed velx, fixed vely, fixed velz, bool add)
{
   if(add)
      p->velx += velx, p->vely += vely, p->velz += velz;
   else
      p->velx = velx, p->vely = vely, p->velz = velz;

   return ACS_SetActorVelocity(p->tid, velx, vely, velz, add, true);
}

void Lith_ValidatePlayerTID(struct player *p)
{
   if(ACS_ActivatorTID() == 0) {
      ACS_Thing_ChangeTID(0, p->tid = ACS_UniqueTID());
      LogDebug(log_dev, "set ptid from 0 to %i", p->tid);
   } else if(p->tid != ACS_ActivatorTID()) {
      LogDebug(log_dev, "set ptid from %i to %i", p->tid, ACS_ActivatorTID());
      p->tid = ACS_ActivatorTID();
   }
}

// Update all of the player's data.
script
void Lith_PlayerUpdateData(struct player *p)
{
   static int const warpflags = WARPF_NOCHECKPOSITION | WARPF_MOVEPTR |
      WARPF_WARPINTERPOLATION | WARPF_COPYINTERPOLATION | WARPF_COPYPITCH;

   p->grabInput = false;

   ACS_Warp(p->cameratid,  4, 0, ACS_GetActorViewHeight(0), 0, warpflags);
   ACS_Warp(p->weathertid, 4, 0, ACS_GetActorViewHeight(0), 0, warpflags);

   p->x = GetX(0);
   p->y = GetY(0);
   p->z = GetZ(0);

   p->velx = ACS_GetActorVelX(0);
   p->vely = ACS_GetActorVelY(0);
   p->velz = ACS_GetActorVelZ(0);

   p->pitch = ACS_GetActorPitch(0) - p->addpitch;
   p->yaw   = ACS_GetActorAngle(0) - p->addyaw;
   p->roll  = ACS_GetActorRoll (0) - p->addroll;

   p->pitchf = ((-p->pitch + 0.25) * 2) * pi;
   p->yawf   = p->yaw * tau - pi;

   p->pitchv = ACS_GetPlayerInputFixed(-1, INPUT_PITCH);
   p->yawv   = ACS_GetPlayerInputFixed(-1, INPUT_YAW);

   p->forwardv = ACS_GetPlayerInputFixed(-1, INPUT_FORWARDMOVE);
   p->sidev    = ACS_GetPlayerInputFixed(-1, INPUT_SIDEMOVE);
   p->upv      = ACS_GetPlayerInputFixed(-1, INPUT_UPMOVE);

   p->buttons = ACS_GetPlayerInput(-1, INPUT_BUTTONS);

   p->name        = StrParam("%tS", p->num);
   p->weaponclass = ACS_GetWeapon();

   p->scopetoken = InvNum("Lith_WeaponScopedToken");

   p->krc = InvNum("RedCard")     ||
            InvNum("ChexRedCard") ||
            InvNum("KeyGreen");
   p->kyc = InvNum("YellowCard")     ||
            InvNum("ChexYellowCard") ||
            InvNum("KeyYellow");
   p->kbc = InvNum("BlueCard")     ||
            InvNum("ChexBlueCard") ||
            InvNum("KeyBlue");
   p->krs = InvNum("RedSkull");
   p->kys = InvNum("YellowSkull");
   p->kbs = InvNum("BlueSkull");
}

script ext("ACS")
void Lith_KeyDown(int pnum, int ch)
{
   withplayer(&players[pnum])
      if(p->tbptr + 1 < countof(p->txtbuf))
         p->txtbuf[p->tbptr++] = ch;
}

script
static void LevelUp(struct player *p, u32 attr[at_max])
{
   u32 level = p->attr.level;

   for(int i = 0; i < at_max; i++) p->attr.attrs[i] += attr[i];

   for(int i = 0; i < 35 * 5; i++)
   {
      if(level != p->attr.level) return; // a new levelup started, so exit

      for(int j = 0; j < at_max; j++)
         if(i > 35*2 / (fixed)at_max * j)
      {
         char *sp = p->attr.lvupstr;

         sp += sprintf(sp, c"LEVEL %u", p->attr.level);
         if(p->attr.points) sprintf(sp, c" (%u points)", p->attr.points);
         *sp++ = '\n';

         for(int k = 0, l = 0; k <= j; k++, l++)
         {
            // skip over any +0 attributes
            while(l < at_max && !attr[l]) l++;
            if(l >= at_max) break;

            sp += sprintf(sp, c"%s +%u (%u)\n", p->attr.names[l], attr[l], p->attr.attrs[l]);
         }
      }

      ACS_Delay(1);
   }

   p->attr.lvupstr[0] = '\0';
}

stkcall
void Lith_GiveEXP(struct player *p, u64 amt)
{
   struct player_attributes *a = &p->attr;

   u32 attr[at_max] = {};
   int levelup = 0;

   while(a->exp + amt >= a->expnext)
   {
      a->level++;
      a->expnext = 500 + (a->level * powlk(1.385, a->level * 0.2) * 340);

      __with(int pts = 7;) switch(p->getCVarI("lith_player_lvsys"))
      {
      case atsys_manual: a->points += 7; break;
      case atsys_hybrid:
         a->points += 2;
         pts       -= 2;
      case atsys_auto:
         for(int i = 0; i < pts; i++) attr[ACS_Random(0, 100) % at_max]++;
         levelup++;
         break;
      }
   }

   if(levelup) LevelUp(p, attr);

   a->exp += amt;
}

// Reset some things on the player when they spawn.
script
void Lith_ResetPlayer(struct player *p)
{
   // Zero-init
   if(!p->wasinit) {
      memset(p, 0, sizeof *p);
      p->wasinit = true;
   }

   // Constant data
   p->active = true;
   p->reinit = p->dead = false;
   p->num    = ACS_PlayerNumber();
   p->bipptr = &p->bip;

   // Static data (pre-init)
   if(!p->staticinit)
   {
      SetPClass(p);
      SetupAttributes(p);

      // i cri tears of pain for APROP_SpawnHealth
      p->viewheight   = ACS_GetActorViewHeight(0);
      p->jumpheight   = GetPropK(0, APROP_JumpZ);
      p->spawnhealth  = GetPropI(0, APROP_Health);
      p->maxhealth    = p->spawnhealth;
      p->discount     = 1.0;
      p->stepnoise    = StrParam("player/%S/step", p->classname);

      switch(ACS_GetPlayerInfo(p->num, PLAYERINFO_GENDER))
      {
      case 0: p->pronoun = pro_male;   break;
      case 1: p->pronoun = pro_female; break;
      case 2: p->pronoun = pro_nb;     break;
      case 3: p->pronoun = pro_object; break;
      }

      for(int i = 0; i < countof(p->notes); i++)
         p->notes[i] = "";
   }

   // Map-static data
   memset(&p->old, 0, sizeof p->old);

   // If the map sets the TID on the first tic, it could already be set here.
   p->tid = 0;
   p->validateTID();

   // This keeps spawning more camera actors when you die, but that should be
   // OK as long as you don't die 2 billion times.
   ACS_SpawnForced("Lith_CameraHax", 0, 0, 0, p->cameratid  = ACS_UniqueTID());
   ACS_SpawnForced("Lith_CameraHax", 0, 0, 0, p->weathertid = ACS_UniqueTID());

   if(world.dbgScore)
      p->score = 0xFFFFFFFFFFFFFFFFll;

   // Reset data

   // Any linked lists on the player need to be initialized here.
   p->hudstrlist.free(true);

   // pls not exit map with murder thingies out
   // is bad practice
   ACS_SetPlayerProperty(0, false, PROP_INSTANTWEAPONSWITCH);
   SetPropK(0, APROP_ViewHeight, p->viewheight);
   InvTake("Lith_WeaponScopedToken", 999);

   Lith_PlayerResetCBIGUI(p);

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

   p->alpha = 1;

   p->attr.lvupstr[0] = '\0';

   // Map-static data
   if(!p->bip.init) Lith_PlayerInitBIP(p);

   if(!p->upgrinit) Lith_PlayerInitUpgrades(p);
   else             Lith_PlayerReinitUpgrades(p);

   if(!p->invinit) Lith_PlayerInitInventory(p);

   // Static data
   if(!p->staticinit)
   {
      p->logB(1, "Lithium " Lith_Version " :: Compiled %S", __DATE__);

      if(world.dbgLevel) {
         p->logH(1, "player is %u bytes long!", sizeof(struct player) * 4);
         p->logH(1, "strnull is \"%S\"", strnull);
         PrintDmonAllocSize(p);
      } else {
         p->logH(1, L("LITH_LOG_StartGame"), "lith_k_opencbi");
      }

      p->deliverMail("Intro");

      p->staticinit = true;
   }

   if(world.dbgItems)
   {
      for(int i = weapon_min; i < weapon_max; i++) {
         weaponinfo_t const *info = &weaponinfo[i];
         if(info->classname != null && info->pclass & p->pclass && !(info->flags & wf_magic))
            InvGive(info->classname, 1);
      }
   }
}

stkcall
void Lith_PlayerUpdateStats(struct player *p)
{
   fixed boost = 1 + p->jumpboost;

   if(p->frozen != p->old.frozen)
      ACS_SetPlayerProperty(0, p->frozen > 0, PROP_TOTALLYFROZEN);

   if(p->speedmul != p->old.speedmul)
      SetPropK(0, APROP_Speed, 0.7 + p->speedmul);

   if(p->jumpboost != p->old.jumpboost)
      SetPropK(0, APROP_JumpZ, p->jumpheight * boost);
}

// EOF
