/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Monster entry points.
 *
 * ---------------------------------------------------------------------------|
 */

#if LITHIUM
#include "w_monster.h"
#include "p_player.h"
#include "w_world.h"
#include "w_scorenums.h"

#define HasResistances(m) ((m)->rank >= 2)

#define GetInfo(m) \
   do { \
      (m)->ms->x = GetX(0); \
      (m)->ms->y = GetY(0); \
      (m)->ms->z = GetZ(0); \
      \
      (m)->ms->r = GetPropK(0, APROP_Radius); \
      (m)->ms->h = GetPropK(0, APROP_Height); \
      \
      (m)->ms->health = GetPropI(0, APROP_Health); \
   } while(0)

/* Types ------------------------------------------------------------------- */

struct dmon_stat {
   k32  x, y, z;
   k32  r, h;
   i32  health;
   i32  painwait;
   bool finalized;
   bool resurrect;
};

/* Static Objects ---------------------------------------------------------- */

#include "w_moninfo.h"

static str const dmgtype_names[dmgtype_max] = {
   s"Bullets",
   s"Energy",
   s"Fire",
   s"Magic",
   s"Melee",
   s"Shrapnel"
};

/* Static Functions -------------------------------------------------------- */

static void ApplyLevels(dmon_t *m, i32 prev)
{
   GetInfo(m);

   for(i32 i = prev + 1; i <= m->level; i++)
   {
      if(i % 10 == 0)
      {
         /* if we have resistances, randomly pick a resistance we already have */
         if(HasResistances(m))
         {
            i32 r;
            do {r = ACS_Random(0, dmgtype_max-1);} while(m->resist[r] == 0);
            m->resist[r] += 2;
         }
      }
   }

   if(m->level >= 5)
   {
      k32 rn = m->rank / 10.0;
      i96 delt = m->level - prev;
      i96 hp10 = m->spawnhealth / 10;
      i32 newh = delt * hp10 * (i96)(ACS_RandomFixed(rn - 0.1, rn + 0.1) * 0xfff) / 0xfff;
      Dbg_Log(log_dmonV, "monster %i: newh %i", m->id, newh);
      SetPropI(0, APROP_Health, m->ms->health + newh);
      m->maxhealth += newh;
   }

   for(i32 i = 0; i < dmgtype_max; i++) {
      ifauto(i32, resist, m->resist[i] / 15.0) {
         InvGive(StrParam(OBJ "M_%S%i", dmgtype_names[i], min(resist, MAXRANK)), 1);
      }
   }
}

stkcall
static void ShowBarrier(dmon_t const *m, k32 alpha)
{
   bool anyplayer = false;

   /* Optimization: Check for players nearby first. */
   i32 const xw1 = m->ms->x - 192, xw2 = m->ms->x + 192;
   i32 const yw1 = m->ms->y - 192, yw2 = m->ms->y + 192;

   for_player() if(aabb(xw1, yw1, xw2, yw2, p->x, p->y))
      {anyplayer = true; break;}

   if(!anyplayer)
      return;

   BeginAngles(m->ms->x, m->ms->y);
   ServCallI(sm_MonsterBarrierLook);

   for(i32 i = 0; i < world.a_cur; i++)
   {
      struct polar *a = &world.a_angles[i];

      k32 dst = m->ms->r / 2 + a->dst / 4;
      k32 x   = m->ms->x + ACS_Cos(a->ang) * dst;
      k32 y   = m->ms->y + ACS_Sin(a->ang) * dst;
      i32   tid = ACS_UniqueTID();
      str bar = m->rank >= 5 ? so_MonsterHeptaura : so_MonsterBarrier;

      ACS_SpawnForced(bar, x, y, m->ms->z + m->ms->h / 2, tid);
      SetPropK(tid, APROP_Alpha, (1 - a->dst / (256 * (m->rank - 1))) * alpha);
   }
}

static void BaseMonsterLevel(dmon_t *m)
{
   k32 rn1 = ACS_RandomFixed(1, MAXRANK);
   k32 rn2 = ACS_RandomFixed(1, MAXLEVEL);
   k32 bias;

   switch(world.game) {
   case Game_Episodic: bias = world.mapscleared / 10.0; break;
   default:            bias = world.mapscleared / 40.0; break;
   }

   for_player() {rn2 += p->attr.level / 2.0; break;}

   bias *= bias;
   bias += ACS_GameSkill() / (k32)skill_nightmare * 0.1;
   bias += ACS_GetCVar(sc_sv_difficulty) / 100.0;
   bias *= ACS_RandomFixed(1, 1.5);

   if(m->mi->flags & mif_angelic)
   {
      m->rank  = 7;
      m->level = 77;
   }
   else if(m->mi->flags & mif_dark)
   {
      m->rank  = 6;
      m->level = 66;
   }
   else if(world.fun & lfun_ragnarok)
   {
      m->rank  = MAXRANK;
      m->level = MAXLEVEL + rn2 * bias;
   }
   else
   {
      m->rank  = minmax(rn1 * bias * 2, 1, MAXRANK);
      m->level = minmax(rn2 * bias    , 1, MAXLEVEL);
   }

   if(HasResistances(m)) for(i32 i = 0; i < m->rank; i++)
      m->resist[ACS_Random(1, dmgtype_max) - 1] += 5;

   ApplyLevels(m, 0);
}

/* Spawn a Monster Soul and temporarily set the species of it until the */
/* actor is no longer solid, so it won't explode immediately. */
script
static void SoulCleave(dmon_t *m, struct player *p)
{
   i32 tid = ACS_UniqueTID();
   ACS_SpawnForced(so_MonsterSoul, m->ms->x, m->ms->y, m->ms->z + 16, tid);
   SetPropI(tid, APROP_Damage, 7 * m->rank * ACS_Random(1, 8));

   PtrSet(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
   SetPropS(tid, APROP_Species, GetPropS(0, APROP_Species));

   for(i32 i = 0; ACS_CheckFlag(0, s_SOLID) && i < 15; i++) ACS_Delay(1);

   SetPropS(tid, APROP_Species, so_Player);
}

static void SpawnManaPickup(dmon_t *m, struct player *p)
{
   i32 i = 0;
   do {
      i32 tid = ACS_UniqueTID();
      i32 x   = m->ms->x + ACS_Random(-16, 16);
      i32 y   = m->ms->y + ACS_Random(-16, 16);
      ACS_Spawn(so_ManaPickup, x, y, m->ms->z + 4, tid);
      PtrSet(tid, AAPTR_DEFAULT, AAPTR_TRACER, p->tid);
      PtrSet(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
      i += 150;
   } while(i < m->maxhealth);
}

static void OnFinalize(dmon_t *m)
{
   with_player(P_PtrFind(0, AAPTR_TARGET))
   {
      if(p->sgacquired)
      {
         if(p->weapon.cur->info->type == weapon_c_starshot && rand() == 1)
            ACS_Teleport_EndGame();

         if(m->mi->type == mtype_imp && m->level >= 50 && m->rank >= 4)
            ACS_SpawnForced(so_ClawOfImp, m->ms->x, m->ms->y, m->ms->z);
      }

      if(!m->ms->finalized)
      {
         if(p->getUpgrActive(UPGR_Magic) && p->mana != p->manamax &&
            (m->mi->type != mtype_zombie || ACS_Random(0, 50) < 10))
         {
            SpawnManaPickup(m, p);
         }

         if(m->mi->type == mtype_zombie && ACS_GetCVar(sc_sv_wepdrop) && !p->weapon.slot[3])
         {
            i32 tid = ACS_UniqueTID();
            ACS_SpawnForced(so_Shotgun, m->ms->x, m->ms->y, m->ms->z, tid);
            ACS_SetActorFlag(tid, s_DROPPED, false);
         }

         if(p->getUpgrActive(UPGR_SoulCleaver))
            SoulCleave(m, p);
      }

           if(p->health <  5) P_Lv_GiveEXP(p, 50);
      else if(p->health < 15) P_Lv_GiveEXP(p, 25);
      else if(p->health < 25) P_Lv_GiveEXP(p, 10);

      P_GiveAllEXP(m->mi->exp + m->level + (m->rank - 1) * 10);
   }

   m->ms->finalized = true;
}

static void OnDeath(dmon_t *m)
{
   Dbg_Log(log_dmon, "monster %i is ded", m->id);
   m->wasdead = true;

   OnFinalize(m);

   /* If enemies emit score on death we only need to give extra rank score. */
   P_GiveAllScore((world.enemycompat ? 0 : m->mi->score) + m->rank * 500, false);
}

script
static void MonsterMain(dmon_t *m)
{
   struct dmon_stat ms = {};

   InvGive(so_MonsterID, m->id + 1);

   m->ms = &ms;
   GetInfo(m);
   m->spawnhealth = m->maxhealth = m->ms->health;

   BaseMonsterLevel(m);

   Dbg_Log(log_dmonV, "monster %-4i \Cdr%i \Cgl%-3i \C-running on %S",
      m->id, m->rank, m->level, ACS_GetActorClass(0));

   for(i32 tic = 0;; tic++)
   {
      GetInfo(m);

      if(m->ms->health <= 0)
      {
         OnDeath(m);

         do {ACS_Delay(15);} while(!m->ms->resurrect);

         m->ms->resurrect = false;
         Dbg_Log(log_dmon, "monster %i resurrected", m->id);
      }

      if(m->exp > 500)
      {
         i32 prev = m->level;

         div_t d = __div(m->exp, 500);
         m->level += d.quot;
         m->exp    = d.rem;

         ACS_SpawnForced(so_MonsterLevelUp, m->ms->x, m->ms->y, m->ms->z);
         ApplyLevels(m, prev);

         Dbg_Log(log_dmon, "monster %i leveled up (%i -> %i)", m->id, prev, m->level);
      }

      if(HasResistances(m) && m->level >= 20)
         ShowBarrier(m, m->level / 100.);

      if(InvNum(so_Ionized) && tic % 5 == 0)
         ServCallI(sm_IonizeFX);

      ACS_Delay(2);
   }
}

/* Extern Functions -------------------------------------------------------- */

void PrintMonsterInfo(void)
{
   ifauto(dmon_t *, m, DmonPtr(0, AAPTR_PLAYER_GETTARGET))
   {
      Log("%p (%p %p) %S active: %u id: %.3u\n"
          "wasdead: %u finalized: %u painwait: %i\n"
          "level: %.3i rank: %i exp: %i\n"
          "health: %i/%i\n"
          "x: %k y: %k z: %k\n"
          "r: %k h: %k\n"
          "mi->exp: %lu mi->score: %lli\n"
          "mi->flags: %i mi->type: %i",
          m, m->ms, m->mi, m->mi->name, m->active, m->id,
          m->wasdead, m->ms->finalized, m->ms->painwait,
          m->level, m->rank, m->exp,
          m->ms->health, m->maxhealth,
          m->ms->x, m->ms->y, m->ms->z,
          m->ms->r, m->ms->h,
          m->mi->exp, m->mi->score,
          m->mi->flags, m->mi->type);
      for(i32 i = 0; i < countof(m->resist); i++)
         Log("resist %S: %i", dmgtype_names[i], m->resist[i]);
   }
   else
      Log("no active monster");
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr("Lith_GiveMonsterEXP")
void Sc_GiveMonsterEXP(i32 amt)
{
   ifauto(dmon_t *, m, DmonSelf()) m->exp += amt;
}

script_str ext("ACS") addr("Lith_ResurrectMonster")
void Sc_ResurrectMonster(i32 amt)
{
   ifauto(dmon_t *, m, DmonSelf()) m->ms->resurrect = true;
}

script ext("ACS") addr(lsc_monsterinfo)
void Sc_MonsterInfo(void)
{
   while(ACS_Timer() < 3) ACS_Delay(1);

   str cname = ACS_GetActorClass(0);

   for(i32 i = 0; i < countof(monsterinfo); i++)
   {
      struct monster_info const *mi = &monsterinfo[i];
      bool init;

      if(mi->flags & mif_full) init = cname == mi->name;
      else                     init = strstr_str(cname, mi->name);

      if(init)
      {
         ifauto(dmon_t *, m, AllocDmon()) {
            m->mi = mi;
            MonsterMain(m);
         }
         return;
      }
   }

   Dbg_Log(log_dmon, "no monster %S", cname);

   /* If the monster failed all checks, give them this so we don't need to recheck every tick. */
   /* Edit: This isn't necessary anymore, but what the hell, keep it. */
   InvGive(so_MonsterInvalid, 1);
}

script_str ext("ACS") addr("Lith_MonsterFinalized")
void Sc_MonsterFinalized(void)
{
   ifauto(dmon_t *, m, DmonSelf())
      OnFinalize(m);
}
#endif

/* EOF */
