// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_monster.h"
#include "lith_player.h"
#include "lith_world.h"
#include "lith_scorenums.h"

#define HasResistances(m) ((m)->rank >= 2)

#define GetInfo(m) \
   do { \
      (m)->ms->x = ACS_GetActorX(0); \
      (m)->ms->y = ACS_GetActorY(0); \
      (m)->ms->z = ACS_GetActorZ(0); \
      \
      (m)->ms->r = ACS_GetActorPropertyFixed(0, APROP_Radius); \
      (m)->ms->h = ACS_GetActorPropertyFixed(0, APROP_Height); \
      \
      (m)->ms->health = ACS_GetActorProperty(0, APROP_Health); \
   } while(0)

// Types ---------------------------------------------------------------------|

struct dmon_stat {
   fixed x, y, z;
   fixed r, h;
   int   health;
   int   painwait;
   bool  finalized;
};

struct monster_info {
   u64        exp;
   i96        score;
   enum mtype type;
   __str      name;
   int        flags;
};

enum {
   mif_full = 1 << 0
};

// Static Objects ------------------------------------------------------------|

#define M(name) Exp_##name, Score_##name
static struct monster_info const monsterinfo[] = {
#if 0
   // Hexen
   {9999, Score_ShotgunGuy,  mtype_imp,         "Ettin",         mif_full},
   {9999, Score_Imp,         mtype_lostsoul,    "FireDemon",     mif_full},
   {9999, Score_Arachnotron, mtype_arachnotron, "CentaurLeader", mif_full},
   {9999, Score_Demon,       mtype_demon,       "Centaur",       mif_full},
   {9999, Score_Mancubus,    mtype_hellknight,  "IceGuy",        mif_full},
   {9999, Score_Mancubus,    mtype_hellknight,  "SerpentLeader", mif_full},
   {9999, Score_Arachnotron, mtype_hellknight,  "Serpent",       mif_full},
   {9999, Score_HellKnight,  mtype_hellknight,  "Demon1",        mif_full},
   {9999, Score_BaronOfHell, mtype_baron,       "Demon2",        mif_full},
   {9999, Score_Cacodemon,   mtype_mancubus,    "Bishop",        mif_full},
   {9999, Score_HellKnight,  mtype_lostsoul,    "Wraith",        mif_full},
   {9999, Score_CyberDemon,  mtype_cyberdemon,  "Dragon",        mif_full},
   {9999, Score_CyberDemon,  mtype_phantom,     "ClericBoss",    mif_full},
   {9999, Score_CyberDemon,  mtype_phantom,     "FighterBoss",   mif_full},
   {9999, Score_CyberDemon,  mtype_phantom,     "MageBoss",      mif_full},
   {9999, Score_CyberDemon,  mtype_cyberdemon,  "Heresiarch",    mif_full},
   {9999, Score_DSparil * 2, mtype_cyberdemon,  "Korax",         mif_full},
#endif

   // Doom 2
   {M(ZombieMan),     mtype_zombie,        "ZombieMan"       },
   {M(ShotgunGuy),    mtype_zombie,        "ShotgunGuy"      },
   {M(ChaingunGuy),   mtype_zombie,        "ChaingunGuy"     },
   {M(Imp),           mtype_imp,           "Imp"             },
   {M(Demon),         mtype_demon,         "Demon"           },
   {M(Spectre),       mtype_demon,         "Spectre"         },
   {M(LostSoul),      mtype_lostsoul,      "LostSoul"        },
   {M(Mancubus),      mtype_mancubus,      "Fatso"           },
   {M(Mancubus),      mtype_mancubus,      "Mancubus"        },
   {M(Arachnotron),   mtype_arachnotron,   "Arachnotron"     },
   {M(Cacodemon),     mtype_cacodemon,     "Cacodemon"       },
   {M(HellKnight),    mtype_hellknight,    "Knight"          },
   {M(BaronOfHell),   mtype_baron,         "Baron"           },
   {M(Revenant),      mtype_revenant,      "Revenant"        },
   {M(PainElemental), mtype_painelemental, "PainElemental"   },
   {M(Archvile),      mtype_archvile,      "Archvile"        },
   {M(SpiderDemon),   mtype_mastermind,    "SpiderMastermind"},
   {M(CyberDemon),    mtype_cyberdemon,    "Cyberdemon"      },

   // Heretic
   {M(Imp),         mtype_imp,         "Gargoyle"  },
   {M(Demon),       mtype_demon,       "Golem"     },
   {M(Nitrogolem),  mtype_demon,       "Nitrogolem"},
   {M(Spectre),     mtype_demon,       "Sabreclaw" },
   {M(Cacodemon),   mtype_mancubus,    "Disciple"  },
   {M(Ophidian),    mtype_arachnotron, "Ophidian"  },
   {M(HellKnight),  mtype_hellknight,  "Warrior"   },
   {M(BaronOfHell), mtype_cacodemon,   "IronLich"  },
   {M(Maulotaur),   mtype_mastermind,  "Maulotaur" },
   {M(DSparil),     mtype_cyberdemon,  "DSparil"   },

   // Lithium
   {1000, 0, mtype_phantom,    "James"   },
   {2000, 0, mtype_phantom,    "Makarov" },
   {3000, 0, mtype_phantom,    "Isaac"   },
   {0,    0, mtype_cyberdemon, "Steggles"},

   // Cheogsh.wad
   {M(Imp),         mtype_imp,        "Howler",         mif_full},
   {M(Imp),         mtype_imp,        "SoulHarvester",  mif_full},
   {M(Demon),       mtype_demon,      "Satyr",          mif_full},
   {M(BaronOfHell), mtype_baron,      "HellWarrior",    mif_full},
   {M(CyberDemon),  mtype_cyberdemon, "Cheogsh",        mif_full},
   {M(Spectre),     mtype_demon,      "MaulerDemon",    mif_full},
   {M(Spectre),     mtype_demon,      "KDiZDNightmare", mif_full},

   // ed4_rfo1.wad
   {M(CyberDemon),    mtype_cacodemon,     "RealAbstractPain", mif_full},
   {M(Arachnotron),   mtype_arachnotron,   "RailSpider",       mif_full},
   {M(PainElemental), mtype_painelemental, "EDPE",             mif_full},
   {M(Arachnotron),   mtype_arachnotron,   "Arachnophyte",     mif_full},
   {M(Imp),           mtype_imp,           "DragonFamiliar",   mif_full},
   {M(Imp),           mtype_imp,           "BatFamiliar",      mif_full},
   {M(Imp),           mtype_imp,           "SnakeImp",         mif_full},
   {M(DRLACaptain),   mtype_zombie,        "BazookaZombie",    mif_full},
   {M(Imp),           mtype_imp,           "DarkImp",          mif_full},
   {M(Imp),           mtype_imp,           "ImpWarlord",       mif_full},

   // dop.pk3
   {M(LostSoul), mtype_lostsoul, "LostGhoul", mif_full},

   // DoomRL Arsenal Monsters
   {M(ZombieMan),   mtype_zombie, "FormerHuman"   },
   {M(ShotgunGuy),  mtype_zombie, "FormerSergeant"},
   {M(ChaingunGuy), mtype_zombie, "FormerCommando"},
   {M(DRLACaptain), mtype_zombie, "Former"}, // hue

   // Colorful Hell
   {M(ZombieMan),     mtype_zombie,        "Zombie"},
   {M(ShotgunGuy),    mtype_zombie,        "SG"    },
   {M(ChaingunGuy),   mtype_zombie,        "CGuy"  },
   {M(LostSoul),      mtype_lostsoul,      "LSoul" },
   {M(HellKnight),    mtype_hellknight,    "HK"    },
   {M(Arachnotron),   mtype_arachnotron,   "SP1"   },
   {M(Cacodemon),     mtype_cacodemon,     "Caco"  },
   {M(Archvile),      mtype_archvile,      "Arch"  },
   {M(PainElemental), mtype_painelemental, "PE"    },
   {M(SpiderDemon),   mtype_mastermind,    "Mind"  },
   {M(CyberDemon),    mtype_cyberdemon,    "Cybie" },

   // Shut Up and Lithium
   {M(ZombieMan),     mtype_imp,         "Roach",        mif_full},
   {M(ZombieMan),     mtype_imp,         "Remnant",      mif_full},
   {M(Imp),           mtype_imp,         "Turmoil",      mif_full},
   {M(ShotgunGuy),    mtype_imp,         "Pitkis",       mif_full},
   {M(ChaingunGuy),   mtype_imp,         "Helliate",     mif_full},
   {M(Demon),         mtype_demon,       "Satyr",        mif_full},
   {M(Spectre),       mtype_demon,       "Mush",         mif_full},
   {M(LostSoul),      mtype_lostsoul,    "Gsoul",        mif_full},
   {M(Cacodemon),     mtype_cacodemon,   "Accuser",      mif_full},
   {M(Revenant),      mtype_revenant,    "Famine",       mif_full},
   {M(PainElemental), mtype_arachnotron, "Writhe",       mif_full},
   {M(HellKnight),    mtype_hellknight,  "Sonnelion",    mif_full},
   {M(LostSoul),      mtype_lostsoul,    "Eotu",         mif_full},
   {M(Arachnotron),   mtype_arachnotron, "Lolth",        mif_full},
   {M(Mancubus),      mtype_mancubus,    "Malbouge",     mif_full},
   {M(BaronOfHell),   mtype_baron,       "Abaddon",      mif_full},
   {M(Archvile),      mtype_archvile,    "Pestilence",   mif_full},
   {M(SpiderDemon),   mtype_mastermind,  "Granfalloon",  mif_full},
   {M(CyberDemon),    mtype_cyberdemon,  "SoF",          mif_full},
   {M(LostSoul),      mtype_lostsoul,    "Karkass"},
   {M(HellKnight),    mtype_imp,         "Doorman",      mif_full},
   {M(LostSoul),      mtype_lostsoul,    "UnbodiedFury", mif_full},
   {M(ShotgunGuy),    mtype_imp,         "Bloodfiend",   mif_full},
   {M(Demon),         mtype_demon,       "Golem",        mif_full},
   {M(Demon),         mtype_demon,       "Phantasm",     mif_full},
   {M(Demon),         mtype_demon,       "Mortuus",      mif_full},
   {M(ChaingunGuy),   mtype_imp,         "Malum",        mif_full},
   {M(ShotgunGuy),    mtype_imp,         "Phasma",       mif_full},
   {M(Spectre),       mtype_demon,       "Licho",        mif_full},
   {M(Cacodemon),     mtype_cacodemon,   "Mandingo",     mif_full},
   {M(Cacodemon),     mtype_cacodemon,   "Hierophant",   mif_full},
   {M(CyberDemon),    mtype_cyberdemon,  "Burrower",     mif_full},
   {M(Archvile),      mtype_arachnotron, "Aranearum",    mif_full},
   {M(Cacodemon),     mtype_cacodemon,   "Lividus",      mif_full},
   {M(HellKnight),    mtype_hellknight,  "Deepone",      mif_full},
   {M(BaronOfHell),   mtype_baron,       "Deeptwo",      mif_full},
};
#undef M

static __str const dmgtype_names[dmgtype_max] = {
   "Bullets",
   "Energy",
   "Fire",
   "Magic",
   "Melee",
   "Shrapnel"
};

// Static Functions ----------------------------------------------------------|

//
// ApplyLevels
//
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
      int hp10 = m->maxhealth / 10;
      int newh = (m->level - prev) * hp10 * (fixed64)ACS_RandomFixed(rn - 0.1, rn + 0.1);
      ACS_SetActorProperty(0, APROP_Health, m->ms->health + newh);
      m->maxhealth += newh;
   }

   for(int i = 0; i < dmgtype_max; i++) {
      ifauto(int, resist, m->resist[i] / 15.0) {
         InvGive(StrParam("Lith_M_%S%i", dmgtype_names[i],
            min(resist, MAXRANK)), 1);
      }
   }
}

//
// ShowBarrier
//
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
   HERMES("MonsterBarrierLook");

   for(int i = 0; i < world.a_cur; i++)
   {
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

//
// SoulCleave
//
// Spawn a Monster Soul and temporarily set the species of it until the
// actor is no longer solid, so it won't explode immediately.
//
script
static void SoulCleave(dmon_t *m, struct player *p)
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
static void SpawnManaPickup(dmon_t *m, struct player *p)
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
// OnFinalize
//
static void OnFinalize(dmon_t *m)
{
   withplayer(Lith_GetPlayer(0, AAPTR_TARGET))
   {
      if(p->sgacquired)
      {
         if(p->weapon.cur->info->type == weapon_c_starshot && rand() == 1)
            ACS_Teleport_EndGame();

         if(m->type == mtype_imp && m->level >= 50 && m->rank >= 4)
            ACS_SpawnForced("Lith_ClawOfImp", m->ms->x, m->ms->y, m->ms->z);
      }

      if(!m->ms->finalized)
      {
         if(p->getUpgrActive(UPGR_Magic) && p->mana != p->manamax &&
            (m->type != mtype_zombie || ACS_Random(0, 50) < 10))
         {
            SpawnManaPickup(m, p);
         }

         if(p->getUpgrActive(UPGR_SoulCleaver))
            SoulCleave(m, p);
      }

           if(p->health <  5) p->giveEXP(50);
      else if(p->health < 15) p->giveEXP(25);
      else if(p->health < 25) p->giveEXP(10);

      Lith_GiveAllEXP(m->exp + m->level + (m->rank - 1) * 10);
   }

   m->ms->finalized = true;
}

//
// OnDeath
//
static void OnDeath(dmon_t *m)
{
   LogDebug(log_dmon, "monster %i is ded", m->id);
   m->wasdead = true;

   OnFinalize(m);

   // If enemies emit score on death we only need to give extra rank score.
   Lith_GiveAllScore((world.enemycompat ? 0 : m->score) + m->rank * 500, false);
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_MonsterMain
//
script stksize(0x7f)
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

      if(HasResistances(m) && m->level >= 20)
         ShowBarrier(m, m->level / 100.);

      if(InvNum("Lith_Ionized") && tic % 5 == 0)
         HERMES("Lith_IonizeFX");

      ACS_Delay(2);
   }
}

//
// Lith_MonsterInfo
//
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
            m->type  = mi->type;
            m->score = mi->score;
            m->exp   = mi->exp;
            Lith_MonsterMain(m);
         }
         return;
      }
   }

   LogDebug(log_dmon, "no monster %S", cname);

   // If the monster failed all checks, give them this so we don't need to recheck every tick.
   InvGive("Lith_MonsterInvalid", 1);
}

//
// Lith_MonsterFinalized
//
script ext("ACS")
void Lith_MonsterFinalized()
{
   ifauto(dmon_t *, m, DmonSelf())
      OnFinalize(m);
}

// EOF
