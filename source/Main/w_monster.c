// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_monster.h"
#include "lith_player.h"
#include "lith_world.h"
#include "lith_scorenums.h"

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

// Types ---------------------------------------------------------------------|

struct dmon_stat {
   fixed x, y, z;
   fixed r, h;
   int   health;
   int   painwait;
   bool  finalized;
};

// Static Objects ------------------------------------------------------------|

#include "lith_monsterinfo.h"

static __str const dmgtype_names[dmgtype_max] = {
   "Bullets",
   "Energy",
   "Fire",
   "Magic",
   "Melee",
   "Shrapnel"
};

// Static Functions ----------------------------------------------------------|

static void ApplyLevels(dmon_t *m, int prev)
{
   GetInfo(m);

   for(int i = prev + 1; i <= m->level; i++)
   {
      if(i % 10 == 0)
      {
         // if we have resistances, randomly pick a resistance we already have
         if(HasResistances(m))
         {
            int r;
            do {r = ACS_Random(0, dmgtype_max-1);} while(m->resist[r] == 0);
            m->resist[r] += 2;
         }
      }
   }

   if(m->level >= 5)
   {
      fixed rn = m->rank / 10.0;
      i64 hp10 = m->maxhealth / 10;
      i64 newh = ((m->level - prev) * hp10 * (i64)(ACS_RandomFixed(rn - 0.1, rn + 0.1) * 0xfff)) / 0xfff;
      LogDebug(log_dmonV, "monster %i: newh %li", m->id, newh);
      SetPropI(0, APROP_Health, m->ms->health + newh);
      m->maxhealth += newh;
   }

   for(int i = 0; i < dmgtype_max; i++) {
      ifauto(int, resist, m->resist[i] / 15.0) {
         InvGive(StrParam("Lith_M_%S%i", dmgtype_names[i],
            min(resist, MAXRANK)), 1);
      }
   }
}

stkcall
static void ShowBarrier(dmon_t const *m, fixed alpha)
{
   bool anyplayer = false;

   // Optimization: Check for players nearby first.
   int const xw1 = m->ms->x - 192, xw2 = m->ms->x + 192;
   int const yw1 = m->ms->y - 192, yw2 = m->ms->y + 192;

   Lith_ForPlayer() if(aabb(xw1, yw1, xw2, yw2, p->x, p->y))
      {anyplayer = true; break;}

   if(!anyplayer)
      return;

   world.begAngles(m->ms->x, m->ms->y);
   ServCallI("MonsterBarrierLook");

   for(int i = 0; i < world.a_cur; i++)
   {
      struct polar *a = &world.a_angles[i];

      fixed dst = m->ms->r / 2 + a->dst / 4;
      fixed x   = m->ms->x + ACS_Cos(a->ang) * dst;
      fixed y   = m->ms->y + ACS_Sin(a->ang) * dst;
      int   tid = ACS_UniqueTID();
      __str bar = m->rank >= 5 ? "Lith_MonsterHeptaura" : "Lith_MonsterBarrier";

      ACS_SpawnForced(bar, x, y, m->ms->z + m->ms->h / 2, tid);
      SetPropK(tid, APROP_Alpha, (1 - a->dst / (256 * (m->rank - 1))) * alpha);
   }
}

static void BaseMonsterLevel(dmon_t *m)
{
   fixed rn1 = ACS_RandomFixed(1, MAXRANK);
   fixed rn2 = ACS_RandomFixed(1, MAXLEVEL);
   fixed bias;

   if(world.fun & lfun_ragnarok)
   {
      m->rank  = MAXRANK;
      m->level = MAXLEVEL;
   }
   else
   {
      switch(world.game) {
      case Game_Episodic: bias = world.mapscleared / 10.0; break;
      default:            bias = world.mapscleared / 40.0; break;
      }

      Lith_ForPlayer() {
         rn2 += p->attr.level / 2.0;
         break;
      }

      bias *= bias;
      bias += (ACS_GameSkill() / (fixed)skill_nightmare) * 0.1;
      bias += world.difficulty / 100.0;
      bias *= ACS_RandomFixed(1, 1.5);

      m->rank  = minmax(rn1 * bias * 2, 1, MAXRANK);
      m->level = minmax(rn2 * bias * 1, 1, MAXLEVEL);
   }

   if(HasResistances(m)) {
      for(int i = 0; i < m->rank; i++)
         m->resist[ACS_Random(1, dmgtype_max)-1] += 5;
   }

   ApplyLevels(m, 0);
}

// Spawn a Monster Soul and temporarily set the species of it until the
// actor is no longer solid, so it won't explode immediately.
script
static void SoulCleave(dmon_t *m, struct player *p)
{
   int tid = ACS_UniqueTID();
   ACS_SpawnForced("Lith_MonsterSoul", m->ms->x, m->ms->y, m->ms->z + 16, tid);
   SetPropI(tid, APROP_Damage, 7 * m->rank * ACS_Random(1, 8));

   Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
   SetPropS(tid, APROP_Species, GetPropS(0, APROP_Species));

   for(int i = 0; ACS_CheckFlag(0, "SOLID") && i < 15; i++) ACS_Delay(1);

   SetPropS(tid, APROP_Species, "Lith_Player");
}

static void SpawnManaPickup(dmon_t *m, struct player *p)
{
   int i = 0;
   do {
      int tid = ACS_UniqueTID();
      int x   = m->ms->x + ACS_Random(-16, 16);
      int y   = m->ms->y + ACS_Random(-16, 16);
      ACS_Spawn("Lith_ManaPickup", x, y, m->ms->z + 4, tid);
      Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TRACER, p->tid);
      Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
      i += 150;
   } while(i < m->maxhealth);
}

static void OnFinalize(dmon_t *m)
{
   withplayer(Lith_GetPlayer(0, AAPTR_TARGET))
   {
      if(p->sgacquired)
      {
         if(p->weapon.cur->info->type == weapon_c_starshot && rand() == 1)
            ACS_Teleport_EndGame();

         if(m->mi->type == mtype_imp && m->level >= 50 && m->rank >= 4)
            ACS_SpawnForced("Lith_ClawOfImp", m->ms->x, m->ms->y, m->ms->z);
      }

      if(!m->ms->finalized)
      {
         if(p->getUpgrActive(UPGR_Magic) && p->mana != p->manamax &&
            (m->mi->type != mtype_zombie || ACS_Random(0, 50) < 10))
         {
            SpawnManaPickup(m, p);
         }

         if(m->mi->type == mtype_zombie && ACS_GetCVar("lith_sv_wepdrop") && !p->weapon.slot[3])
         {
            int tid = ACS_UniqueTID();
            ACS_SpawnForced("Shotgun", m->ms->x, m->ms->y, m->ms->z, tid);
            ACS_SetActorFlag(tid, "DROPPED", false);
         }

         if(p->getUpgrActive(UPGR_SoulCleaver))
            SoulCleave(m, p);
      }

           if(p->health <  5) p->giveEXP(50);
      else if(p->health < 15) p->giveEXP(25);
      else if(p->health < 25) p->giveEXP(10);

      Lith_GiveAllEXP(m->mi->exp + m->level + (m->rank - 1) * 10);
   }

   m->ms->finalized = true;
}

static void OnDeath(dmon_t *m)
{
   LogDebug(log_dmon, "monster %i is ded", m->id);
   m->wasdead = true;

   OnFinalize(m);

   // If enemies emit score on death we only need to give extra rank score.
   Lith_GiveAllScore((world.enemycompat ? 0 : m->mi->score) + m->rank * 500, false);
}

// Extern Functions ----------------------------------------------------------|

script ext("ACS")
void Lith_GiveEXPToMonster(int amt)
{
   ifauto(dmon_t *, m, DmonPtr(0, AAPTR_PLAYER_GETTARGET)) m->exp += amt;
}

script ext("ACS")
void Lith_PrintMonsterInfo(void)
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
      for(int i = 0; i < countof(m->resist); i++)
         Log("resist %S: %i", dmgtype_names[i], m->resist[i]);
   }
   else
      Log("no active monster");
}

script ext("ACS")
void Lith_GiveMonsterEXP(int amt)
{
   ifauto(dmon_t *, m, DmonSelf()) m->exp += amt;
}

script
void Lith_MonsterMain(dmon_t *m)
{
   struct dmon_stat ms = {};

   InvGive("Lith_MonsterID", m->id + 1);

   m->ms = &ms;
   GetInfo(m);
   m->maxhealth = m->ms->health;

   BaseMonsterLevel(m);

   LogDebug(log_dmonV, "monster %-4i \Cdr%i \Cgl%-3i \C-running on %S",
      m->id, m->rank, m->level, ACS_GetActorClass(0));

   for(int tic = 0;; tic++)
   {
      GetInfo(m);

      if(m->ms->health <= 0)
      {
         OnDeath(m);

         do {ACS_Delay(3); GetInfo(m);} while(m->ms->health <= 0);

         LogDebug(log_dmon, "monster %i resurrected", m->id);
      }

      if(m->exp > 500)
      {
         int prev = m->level;

         div_t d = div(m->exp, 500);
         m->level += d.quot;
         m->exp    = d.rem;

         ACS_SpawnForced("Lith_MonsterLevelUp", m->ms->x, m->ms->y, m->ms->z);
         ApplyLevels(m, prev);

         LogDebug(log_dmon, "monster %i leveled up (%i -> %i)", m->id, prev, m->level);
      }

      if(HasResistances(m) && m->level >= 20)
         ShowBarrier(m, m->level / 100.);

      if(InvNum("Lith_Ionized") && tic % 5 == 0)
         ServCallI("Lith_IonizeFX");

      ACS_Delay(2);
   }
}

script ext("ACS")
void Lith_MonsterInfo()
{
   while(!world.gsinit) ACS_Delay(1);

   __str cname = ACS_GetActorClass(0);

   for(int i = 0; i < countof(monsterinfo); i++)
   {
      struct monster_info const *mi = &monsterinfo[i];
      bool init;

      if(mi->flags & mif_full) init = cname == mi->name;
      else                     init = strstr_str(cname, mi->name);

      if(init)
      {
         ifauto(dmon_t *, m, AllocDmon()) {
            m->mi = mi;
            Lith_MonsterMain(m);
         }
         return;
      }
   }

   LogDebug(log_dmon, "no monster %S", cname);

   // If the monster failed all checks, give them this so we don't need to recheck every tick.
   InvGive("Lith_MonsterInvalid", 1);
}

script ext("ACS")
void Lith_MonsterFinalized()
{
   ifauto(dmon_t *, m, DmonSelf())
      OnFinalize(m);
}

// EOF
