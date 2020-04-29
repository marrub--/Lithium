/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Monster entry points.
 *
 * ---------------------------------------------------------------------------|
 */

#include "w_monster.h"
#include "p_player.h"
#include "w_world.h"
#include "w_scorenums.h"

#define HasResistances(m) ((m)->rank >= 2)

/* Types ------------------------------------------------------------------- */

enum {
   _max_rank  = 5,
   _max_level = 150,
};

/* Static Objects ---------------------------------------------------------- */

#include "w_moninfo.h"

StrAry(dmgtype_names,
       s"Bullets",
       s"Energy",
       s"Fire",
       s"Magic",
       s"Melee",
       s"Shrapnel");

/* Static Functions -------------------------------------------------------- */

static void GetInfo(dmon_t *m) {
   m->x = GetX(0);
   m->y = GetY(0);
   m->z = GetZ(0);

   m->r = GetPropK(0, APROP_Radius);
   m->h = GetPropK(0, APROP_Height);

   m->health = GetMembI(0, sm_Health);
}

static void ApplyLevels(dmon_t *m, i32 prev) {
   GetInfo(m);

   for(i32 i = prev + 1; i <= m->level; i++) {
      if(i % 10 == 0) {
         /* if we have resistances, randomly pick a resistance we
          * already have */
         if(HasResistances(m)) {
            i32 r;
            do {r = ACS_Random(0, dmgtype_max-1);} while(m->resist[r] == 0);
            m->resist[r] += 2;
         }
      }
   }

   if(m->level >= 5) {
      k32 rn = m->rank / 10.0;
      i96 delt = m->level - prev;
      i96 hp10 = m->spawnhealth / 10;
      i32 newh = delt * hp10 * (i96)(ACS_RandomFixed(rn - 0.1, rn + 0.1) * 0xfff) / 0xfff;
      Dbg_Log(log_dmonV, "monster %i: newh %i", m->id, newh);
      SetPropI(0, APROP_Health, m->health + newh);
      m->maxhealth += newh;
   }

   for(i32 i = 0; i < dmgtype_max; i++) {
      ifauto(i32, resist, m->resist[i] / 15.0) {
         InvGive(StrParam(OBJ "M_%S%i", dmgtype_names[i], min(resist, _max_rank)), 1);
      }
   }
}

static void ShowBarrier(dmon_t const *m, k32 alpha)
{
   bool anyplayer = false;

   /* Optimization: Check for players nearby first. */
   i32 const xw1 = m->x - 192, xw2 = m->x + 192;
   i32 const yw1 = m->y - 192, yw2 = m->y + 192;

   for_player() if(aabb(xw1, yw1, xw2, yw2, p->x, p->y))
      {anyplayer = true; break;}

   if(!anyplayer)
      return;

   BeginAngles(m->x, m->y);
   ServCallI(sm_MonsterBarrierLook);

   for(i32 i = 0; i < a_cur; i++)
   {
      struct polar *a = &a_angles[i];

      k32 dst = m->r / 2 + a->dst / 4;
      k32 x   = m->x + ACS_Cos(a->ang) * dst;
      k32 y   = m->y + ACS_Sin(a->ang) * dst;
      i32   tid = ACS_UniqueTID();
      str bar = m->rank >= 5 ? so_MonsterHeptaura : so_MonsterBarrier;

      ACS_SpawnForced(bar, x, y, m->z + m->h / 2, tid);
      SetPropK(tid, APROP_Alpha, (1 - a->dst / (256 * (m->rank - 1))) * alpha);
   }
}

static void BaseMonsterLevel(dmon_t *m)
{
   k32 rlv = ACS_RandomFixed(1, _max_level);
   k32 bias;

   bias = mapscleared / 40.0;

   bias *= bias;
   bias += ACS_GameSkill() / (k32)skill_nightmare * 0.1;
   bias += ACS_GetCVar(sc_sv_difficulty) / 100.0;
   bias *= ACS_RandomFixed(1, 1.5);

   if(get_bit(m->mi->flags, mif_angelic)) {
      m->rank  = 7;
      m->level = 7 + rlv * bias;
   } else if(get_bit(m->mi->flags, mif_dark)) {
      m->rank  = 6;
      m->level = 6 + rlv * bias;
   } else if(GetFun() & lfun_ragnarok) {
      m->rank  = _max_rank;
      m->level = _max_level + rlv * bias;
   } else {
      k32 rrn = ACS_RandomFixed(1, _max_rank);

      for_player() {rlv += p->attr.level / 2.0; break;}

      m->rank  = minmax(rrn * bias * 2, 1, _max_rank);
      m->level = minmax(rlv * bias    , 1, _max_level);
   }

   switch(m->rank) {
      case 5: ServCallI(sm_SetTeleFogTo, so_TeleFog5); break;
      case 6: ServCallI(sm_SetTeleFogTo, so_TeleFog6); break;
      case 7: ServCallI(sm_SetTeleFogTo, so_TeleFog7); break;
   }

   if(HasResistances(m)) for(i32 i = 0; i < m->rank; i++)
      m->resist[ACS_Random(1, dmgtype_max) - 1] += 5;

   ApplyLevels(m, 0);
}

/* Spawn a Monster Soul and temporarily set the species of it until the
 * actor is no longer solid, so it won't explode immediately.
 */
script
static void SoulCleave(dmon_t *m, struct player *p)
{
   Str(solid_s, s"SOLID");

   i32 tid = ACS_UniqueTID();
   ACS_SpawnForced(so_MonsterSoul, m->x, m->y, m->z + 16, tid);
   SetPropI(tid, APROP_Damage, m->level / 8 * 7);

   PtrSet(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
   SetPropS(tid, APROP_Species, GetPropS(0, APROP_Species));

   for(i32 i = 0; ACS_CheckFlag(0, solid_s) && i < 15; i++) ACS_Delay(1);

   SetPropS(tid, APROP_Species, so_Player);
}

static void SpawnManaPickup(dmon_t *m, struct player *p)
{
   i32 i = 0;
   do {
      i32 tid = ACS_UniqueTID();
      i32 x   = m->x + ACS_Random(-16, 16);
      i32 y   = m->y + ACS_Random(-16, 16);
      ACS_Spawn(so_ManaPickup, x, y, m->z + 4, tid);
      PtrSet(tid, AAPTR_DEFAULT, AAPTR_TRACER, p->tid);
      PtrSet(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
      i += 150;
   } while(i < m->maxhealth);
}

static void OnFinalize(dmon_t *m) {
   with_player(P_PtrFind(0, AAPTR_TARGET)) {
      if(p->sgacquired) {
         bool high_level_imp =
            m->mi->type == mtype_imp && m->level >= 70 && m->rank >= 4;
         if(high_level_imp && ACS_Random(0, 100) < 4)
            ACS_SpawnForced(so_ClawOfImp, m->x, m->y, m->z);
      }

      if(!m->finalized) {
         if(get_bit(p->upgrades[UPGR_Magic].flags, _ug_active) &&
            p->mana != p->manamax &&
            (m->mi->type != mtype_zombie || ACS_Random(0, 50) < 10)) {
            SpawnManaPickup(m, p);
         }

         if(ACS_GetCVar(sc_sv_wepdrop)) {
            Str(sgun, s"Shotgun");
            Str(cgun, s"Chaingun");
            str sp = snil;
            switch(m->mi->type) {
               case mtype_zombiesg: if(!p->weapon.slot[3]) sp = sgun; break;
               case mtype_zombiecg: if(!p->weapon.slot[4]) sp = cgun; break;
            }
            if(sp) {
               Str(dropped_s, s"DROPPED");
               i32 tid = ACS_UniqueTID();
               ACS_SpawnForced(sp, m->x, m->y, m->z, tid);
               ACS_SetActorFlag(tid, dropped_s, false);
            }
         }

         if(get_bit(p->upgrades[UPGR_SoulCleaver].flags, _ug_active))
            SoulCleave(m, p);
      }

           if(p->health <  5) P_Lv_GiveEXP(p, 50);
      else if(p->health < 15) P_Lv_GiveEXP(p, 25);
      else if(p->health < 25) P_Lv_GiveEXP(p, 10);

      P_GiveAllEXP(m->mi->exp + m->level + (m->rank - 1) * 10);
   }

   m->finalized = true;
}

static void OnDeath(dmon_t *m)
{
   Dbg_Log(log_dmon, "monster %i is ded", m->id);
   m->wasdead = true;

   OnFinalize(m);

   P_GiveAllScore(m->mi->score + m->rank * 500, false);
}

script
static void MonsterMain(dmon_t *m)
{
   InvGive(so_MonsterID, m->id + 1);

   GetInfo(m);
   m->spawnhealth = m->maxhealth = m->health;

   BaseMonsterLevel(m);

   Dbg_Log(log_dmonV, "monster %-4i \Cdr%i \Cgl%-3i \C-running on %S",
      m->id, m->rank, m->level, ACS_GetActorClass(0));

   for(i32 tic = 0;; tic++)
   {
      GetInfo(m);

      if(m->health <= 0)
      {
         OnDeath(m);

         do {ACS_Delay(15);} while(!m->resurrect);

         m->resurrect = false;
         Dbg_Log(log_dmon, "monster %i resurrected", m->id);
      }

      if(m->exp > _monster_level_exp)
      {
         i32 prev = m->level;

         div_t d = __div(m->exp, _monster_level_exp);
         m->level += d.quot;
         m->exp    = d.rem;

         ACS_SpawnForced(so_MonsterLevelUp, m->x, m->y, m->z);
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
      Log("%p (%p) %S active: %u id: %.3u\n"
          "wasdead: %u finalized: %u painwait: %i\n"
          "level: %.3i rank: %i exp: %i\n"
          "health: %i/%i\n"
          "x: %k y: %k z: %k\n"
          "r: %k h: %k\n"
          "mi->exp: %lu mi->score: %lli\n"
          "mi->flags: %i mi->type: %i",
          m, m->mi, m->mi->name, m->active, m->id,
          m->wasdead, m->finalized, m->painwait,
          m->level, m->rank, m->exp,
          m->health, m->maxhealth,
          m->x, m->y, m->z,
          m->r, m->h,
          m->mi->exp, m->mi->score,
          m->mi->flags, m->mi->type);
      for(i32 i = 0; i < countof(m->resist); i++)
         Log("resist %S: %i", dmgtype_names[i], m->resist[i]);
   }
   else
      Log("no active monster");
}

/* Scripts ----------------------------------------------------------------- */

script ext("ACS") addr(lsc_monstertype)
i32 Sc_GetMonsterType()
{
   ifauto(dmon_t *, m, DmonSelf()) return m->mi->type;
   else                            return mtype_unknown;
}

script_str ext("ACS") addr(OBJ "GiveMonsterEXP")
void Sc_GiveMonsterEXP(i32 amt)
{
   ifauto(dmon_t *, m, DmonSelf()) m->exp += amt;
}

script_str ext("ACS") addr(OBJ "ResurrectMonster")
void Sc_ResurrectMonster(i32 amt)
{
   ifauto(dmon_t *, m, DmonSelf()) m->resurrect = true;
}

script ext("ACS") addr(lsc_monsterinfo)
void Sc_MonsterInfo(void)
{
   Str(rladaptive, s"RLAdaptive");
   Str(rlhax,      s"RLCyberdemonMkII");

   str cname = ACS_GetActorClass(0);

   if(strstr_str(cname, rladaptive) || strstr_str(cname, rlhax))
      return;

   for(i32 i = 0; i < countof(monsterinfo); i++) {
      struct monster_info const *mi = &monsterinfo[i];
      bool init;

      if(get_bit(mi->flags, mif_full)) init = cname == mi->name;
      else                             init = strstr_str(cname, mi->name);

      if(init) {
         ACS_Delay(1);
         ifauto(dmon_t *, m, AllocDmon()) {
            m->mi = mi;
            MonsterMain(m);
         }
         return;
      }
   }

   Dbg_Log(log_dmon, "no monster %S", cname);

   /* If the monster failed all checks, give them this so we don't
      need to recheck every tick.
      Edit: This isn't necessary anymore, but what the hell, keep it. */
   InvGive(so_MonsterInvalid, 1);
}

script_str ext("ACS") addr(OBJ "MonsterFinalized")
void Sc_MonsterFinalized(void)
{
   ifauto(dmon_t *, m, DmonSelf())
      OnFinalize(m);
}

/* EOF */
