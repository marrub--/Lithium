// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_monster.h"
#include "lith_player.h"
#include "lith_world.h"
#include "lith_scorenums.h"

#include <math.h>

#define HasResistances(m) ((m)->rank >= 2)

struct dmon_stat {
   fixed x, y, z;
   fixed r, h;
   int health;
   int painwait;
};

struct monster_info {
   score_t score;
   enum mtype type;
   __str name;
   int flags;
};

enum {
   mif_fullmatch = 1 << 0
};

static struct monster_info const monsterinfo[] = {
#if 0
   // Hexen
   {Score_ShotgunGuy,  mtype_imp,         "Ettin",         mif_fullmatch},
   {Score_Imp,         mtype_lostsoul,    "FireDemon",     mif_fullmatch},
   {Score_Arachnotron, mtype_arachnotron, "CentaurLeader", mif_fullmatch},
   {Score_Demon,       mtype_demon,       "Centaur",       mif_fullmatch},
   {Score_Mancubus,    mtype_hellknight,  "IceGuy",        mif_fullmatch},
   {Score_Mancubus,    mtype_hellknight,  "SerpentLeader", mif_fullmatch},
   {Score_Arachnotron, mtype_hellknight,  "Serpent",       mif_fullmatch},
   {Score_HellKnight,  mtype_hellknight,  "Demon1",        mif_fullmatch},
   {Score_BaronOfHell, mtype_baron,       "Demon2",        mif_fullmatch},
   {Score_Cacodemon,   mtype_mancubus,    "Bishop",        mif_fullmatch},
   {Score_HellKnight,  mtype_lostsoul,    "Wraith",        mif_fullmatch},
   {Score_CyberDemon,  mtype_cyberdemon,  "Dragon",        mif_fullmatch},
   {Score_CyberDemon,  mtype_phantom,     "ClericBoss",    mif_fullmatch},
   {Score_CyberDemon,  mtype_phantom,     "FighterBoss",   mif_fullmatch},
   {Score_CyberDemon,  mtype_phantom,     "MageBoss",      mif_fullmatch},
   {Score_CyberDemon,  mtype_cyberdemon,  "Heresiarch",    mif_fullmatch},
   {Score_DSparil * 2, mtype_cyberdemon,  "Korax",         mif_fullmatch},
#endif

   // Doom 2
   {Score_ZombieMan,     mtype_zombie,        "ZombieMan"       },
   {Score_ShotgunGuy,    mtype_zombie,        "ShotgunGuy"      },
   {Score_ChaingunGuy,   mtype_zombie,        "ChaingunGuy"     },
   {Score_Imp,           mtype_imp,           "Imp"             },
   {Score_Demon,         mtype_demon,         "Demon"           },
   {Score_Demon * 1.5,   mtype_demon,         "Spectre"         },
   {Score_LostSoul,      mtype_lostsoul,      "LostSoul"        },
   {Score_Mancubus,      mtype_mancubus,      "Fatso"           },
   {Score_Mancubus,      mtype_mancubus,      "Mancubus"        },
   {Score_Arachnotron,   mtype_arachnotron,   "Arachnotron"     },
   {Score_Cacodemon,     mtype_cacodemon,     "Cacodemon"       },
   {Score_HellKnight,    mtype_hellknight,    "Knight"          },
   {Score_BaronOfHell,   mtype_baron,         "Baron"           },
   {Score_Revenant,      mtype_revenant,      "Revenant"        },
   {Score_PainElemental, mtype_painelemental, "PainElemental"   },
   {Score_Archvile,      mtype_archvile,      "Archvile"        },
   {Score_SpiderDemon,   mtype_mastermind,    "SpiderMastermind"},
   {Score_CyberDemon,    mtype_cyberdemon,    "Cyberdemon"      },

   // Heretic
   {Score_Imp,         mtype_imp,         "Gargoyle"  },
   {Score_Demon,       mtype_demon,       "Golem"     },
   {Score_Nitrogolem,  mtype_demon,       "Nitrogolem"},
   {Score_Demon * 1.5, mtype_demon,       "Sabreclaw" },
   {Score_Cacodemon,   mtype_mancubus,    "Disciple"  },
   {Score_Arachnotron, mtype_arachnotron, "Ophidian"  },
   {Score_HellKnight,  mtype_hellknight,  "Warrior"   },
   {Score_BaronOfHell, mtype_cacodemon,   "IronLich"  },
   {Score_SpiderDemon, mtype_mastermind,  "Maulotaur" },
   {Score_DSparil,     mtype_cyberdemon,  "DSparil"   },

   // Lithium
   {0, mtype_phantom,    "James"   },
   {0, mtype_phantom,    "Makarov" },
   {0, mtype_phantom,    "Isaac"   },
   {0, mtype_cyberdemon, "Steggles"},

   // DoomRL Arsenal Monsters
   {Score_ZombieMan,   mtype_zombie, "FormerHuman"   },
   {Score_ShotgunGuy,  mtype_zombie, "FormerSergeant"},
   {Score_ChaingunGuy, mtype_zombie, "FormerCommando"},
   {Score_ChaingunGuy, mtype_zombie, "Former"}, // hue

   // Colorful Hell
   {Score_ZombieMan,     mtype_zombie,        "Zombie"},
   {Score_ShotgunGuy,    mtype_zombie,        "SG"    },
   {Score_ChaingunGuy,   mtype_zombie,        "CGuy"  },
   {Score_LostSoul,      mtype_lostsoul,      "LSoul" },
   {Score_HellKnight,    mtype_hellknight,    "HK"    },
   {Score_Arachnotron,   mtype_arachnotron,   "SP1"   },
   {Score_Cacodemon,     mtype_cacodemon,     "Caco"  },
   {Score_Archvile,      mtype_archvile,      "Arch"  },
   {Score_PainElemental, mtype_painelemental, "PE"    },
   {Score_SpiderDemon,   mtype_mastermind,    "Mind"  },
   {Score_CyberDemon,    mtype_cyberdemon,    "Cybie" },
};

static __str const dmgtype_names[dmgtype_max] = {
   "Bullets",
   "Energy",
   "Fire",
   "Magic",
   "Melee",
   "Shrapnel"
};

//
// WaitForResurrect
//
[[__call("SScriptS")]]
static void WaitForResurrect(dmon_t const *m)
{
   while(ACS_GetActorProperty(0, APROP_Health) <= 0)
      ACS_Delay(2);

   LogDebug(log_dmon, "monster %i resurrected", m->id);
}

//
// GetInfo
//
static void GetInfo(struct dmon_stat *ms)
{
   ms->x = ACS_GetActorX(0);
   ms->y = ACS_GetActorY(0);
   ms->z = ACS_GetActorZ(0);

   ms->r = ACS_GetActorPropertyFixed(0, APROP_Radius);
   ms->h = ACS_GetActorPropertyFixed(0, APROP_Height);

   ms->health = ACS_GetActorProperty(0, APROP_Health);
}

//
// ApplyLevels
//
static void ApplyLevels(dmon_t *m, int prev)
{
   GetInfo(m->ms);

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

         m->painresist++;
      }
   }

   if(m->level >= 5)
   {
      float rn = m->rank / 10.f;
      int hp10 = m->maxhealth / 10;
      int newh = (m->level - prev) * hp10 * RandomFloat(rn - .1f, rn + .1f);
      ACS_SetActorProperty(0, APROP_Health, m->ms->health + newh);
      m->maxhealth += newh;
   }

   for(int i = 0; i < dmgtype_max; i++) {
      ifauto(int, resist, m->resist[i] / 15.0) {
         ACS_GiveInventory(StrParam("Lith_M_%S%i", dmgtype_names[i],
            min(resist, MAXRANK)), 1);
      }
   }
}

//
// ShowBarrier
//
static void ShowBarrier(dmon_t const *m, fixed alpha)
{
   bool anyplayer = false;

   // Optimization: Check for players nearby first.
   int const xw1 = m->ms->x - 192, xw2 = m->ms->x + 192;
   int const yw1 = m->ms->y - 192, yw2 = m->ms->y + 192;
   Lith_ForPlayer() {
      if(aabb(xw1, yw1, xw2, yw2, p->x, p->y)) {
         anyplayer = true;
         break;
      }
   }

   if(!anyplayer)
      return;

   world.begAngles(m->ms->x, m->ms->y);
   ACS_GiveInventory("Lith_MonsterBarrierLook", 1);

   for(int i = 0; i < world.a_cur; i++) {
      struct polar *a = &world.a_angles[i];

      fixed dst = m->ms->r / 2 + a->dst / 4;
      fixed x   = m->ms->x + ACS_Cos(a->ang) * dst;
      fixed y   = m->ms->y + ACS_Sin(a->ang) * dst;
      int   tid = ACS_UniqueTID();
      __str bar = m->rank >= 5 ? "Lith_MonsterHeptaura" : "Lith_MonsterBarrier";

      ACS_SpawnForced(bar, x, y, m->ms->z + m->ms->h / 2, tid);
      ACS_SetActorPropertyFixed(tid, APROP_Alpha,
         (1 - a->dst / (256 * (m->rank - 1))) * alpha);
   }
}

//
// BaseMonsterLevel
//
static void BaseMonsterLevel(dmon_t *m)
{
   fixed rn1 = ACS_RandomFixed(1, MAXRANK);
   fixed rn2 = ACS_RandomFixed(1, MAXLEVEL);
   fixed bias;

   switch(world.game) {
   case Game_Episodic: bias = world.mapscleared / 10.0; break;
   default:            bias = world.mapscleared / 40.0; break;
   }

   bias *= bias;
   bias += (ACS_GameSkill() / (fixed)skill_nightmare) * 0.1;
   bias += world.difficulty / 100.0;
   bias *= ACS_RandomFixed(1, 1.5);

   m->rank  = minmax(rn1 * bias * 2, 1, MAXRANK);
   m->level = minmax(rn2 * bias * 1, 1, MAXLEVEL);

   if(HasResistances(m)) {
      for(int i = 0; i < m->rank; i++)
         m->resist[ACS_Random(1, dmgtype_max)-1] += 5;
   }

   ApplyLevels(m, 0);
}

//
// ApplyPainResist
//
static void ApplyPainResist(dmon_t *m)
{
   if(!m->ms->painwait) {
      ACS_GiveInventory("Lith_MonsterUsePain", 1);
      m->ms->painwait = m->painresist;
   } else {
      ACS_GiveInventory("Lith_MonsterNoPain", 1);
      m->ms->painwait--;
   }
}

//
// SoulCleave
//
// Spawn a Monster Soul and temporarily set the species of it until the
// actor is no longer solid, so it won't explode immediately.
//
[[__call("ScriptS")]]
static void SoulCleave(dmon_t *m, player_t *p)
{
   int tid = ACS_UniqueTID();
   ACS_SpawnForced("Lith_MonsterSoul", m->ms->x, m->ms->y, m->ms->z + 16, tid);
   ACS_SetActorProperty(tid, APROP_Damage, 7 * m->rank * ACS_Random(1, 8));

   Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
   ACS_SetActorPropertyString(tid, APROP_Species,
      ACS_GetActorPropertyString(0, APROP_Species));

   for(int i = 0; ACS_CheckFlag(0, "SOLID") && i < 15; i++)
      ACS_Delay(1);

   ACS_SetActorPropertyString(tid, APROP_Species, "Lith_Player");
}

//
// SpawnManaPickup
//
static void SpawnManaPickup(dmon_t *m, player_t *p)
{
   int i = 0;
   do {
      int tid = ACS_UniqueTID();
      int x   = m->ms->x + ACS_Random(-8, 8);
      int y   = m->ms->y + ACS_Random(-8, 8);
      ACS_Spawn("Lith_ManaPickup", x, y, m->ms->z + 4, tid);
      Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TRACER, p->tid);
      Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
      i += 150;
   } while(i < m->maxhealth);
}

//
// OnDeath
//
static void OnDeath(dmon_t *m)
{
   LogDebug(log_dmon, "monster %i is ded", m->id);
   m->wasdead = true;

   ifauto(player_t *, p, Lith_GetPlayer(0, AAPTR_TARGET))
   {
      if(p->sigil.acquired)
      {
         if(p->weapon.cur->info->type == weapon_c_starshot && rand() == 1)
            ACS_Teleport_EndGame();

         if(m->type == mtype_imp && m->level >= 50 && m->rank >= 4)
            ACS_SpawnForced("Lith_ClawOfImp", m->ms->x, m->ms->y, m->ms->z);
      }

      if(p->getUpgrActive(UPGR_Magic) && p->mana != p->manamax &&
         (m->type != mtype_zombie || ACS_Random(0, 50) < 10))
      {
         SpawnManaPickup(m, p);
      }

      if(p->getUpgrActive(UPGR_SoulCleaver))
         SoulCleave(m, p);
   }

   // If enemies emit score on death we only need to give extra rank score.
   Lith_GiveAllScore((world.enemycompat ? 0 : m->score) + m->rank * 500, false);
}

//
// Lith_MonsterMain
//
[[__call("ScriptS")]]
void Lith_MonsterMain(dmon_t *m)
{
   struct dmon_stat ms = {};

   ACS_GiveInventory("Lith_MonsterID", m->id + 1);

   m->ms = &ms;
   GetInfo(m->ms);
   m->maxhealth = m->ms->health;

   BaseMonsterLevel(m);

   LogDebug(log_dmonV, "monster %i\t\Cdr%i \Cgl%i\C-\trunning on %S",
      m->id, m->rank, m->level, ACS_GetActorClass(0));

   for(;;)
   {
      GetInfo(m->ms);

      if(ms.health <= 0) {
         OnDeath(m);
         WaitForResurrect(m);
      }

      if(HasResistances(m) && m->level >= 20)
         ShowBarrier(m, m->level / (fixed)MAXLEVEL);

      if(ACS_CheckInventory("Lith_Ionized") && ACS_Timer() % 10 == 0)
         ACS_GiveInventory("Lith_IonizedFXSpawner", 1);

      // Strange optimization: This causes horrible lag in the ZScript VM,
      // but in ZDoom 2.8.1 it's completely fine. What the fuck.
      if(!world.grafZoneEntered && m->painresist)
         ApplyPainResist(m);

      ACS_Delay(1);
   }
}

//
// Lith_MonsterInfo
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_MonsterInfo(int tid)
{
   if(tid) ACS_SetActivator(tid);
   while(!world.gsinit) ACS_Delay(1);

   __str cname = ACS_GetActorClass(0);

   for(int i = 0; i < countof(monsterinfo); i++)
   {
      struct monster_info const *mi = &monsterinfo[i];
      bool init;

      if(mi->flags & mif_fullmatch) init = cname == mi->name;
      else                          init = strstr_str(cname, mi->name);

      if(init)
      {
         ifauto(dmon_t *, m, AllocDmon()) {
            m->type  = mi->type;
            m->score = mi->score;
            Lith_MonsterMain(m);
         }
         return;
      }
   }

   LogDebug(log_dmon, "no monster %S", cname);

   // If the monster failed all checks, give them this so we don't need to recheck every tick.
   ACS_GiveInventory("Lith_MonsterInvalid", 1);
}

// EOF

