// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Monster entry points.                                                    │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "w_monster.h"
#include "p_player.h"
#include "w_world.h"
#include "m_file.h"
#include "m_tokbuf.h"
#include "m_trie.h"

#define HasResistances(m) ((m)->rank >= 2)

noinit static struct monster_preset monsterpreset[1024];
noinit static struct monster_info monsterinfo[2048];
noinit static mem_size_t monsterpresetnum, monsterinfonum;

stkoff static void GetInfo(dmon_t *m) {
   m->x = GetX(0);
   m->y = GetY(0);
   m->z = GetZ(0);
   m->r = GetRadius(0);
   m->h = GetHeight(0);
   m->health = GetHealth(0);
}

script static void ApplyLevels(dmon_t *m, i32 prev) {
   GetInfo(m);
   Dbg_Log(log_dmon,
           _l("monster "), _p(m->id), _l(" leveled up ("), _p(prev),
           _l(" -> "), _p(m->level), _c(')'));
   for(i32 i = prev + 1; i <= m->level; i++) {
      if(i % 10 == 0) {
         /* if we have resistances, randomly pick a resistance we
          * already have */
         if(HasResistances(m)) {
            i32 r;
            do {
               r = ACS_Random(0, dmgtype_max-1);
            } while(m->resist[r] == 0);
            m->resist[r] += 2;
         }
      }
   }
   if(m->level >= 5) {
      i32 level_delta = m->level - prev;
      k64 health_mul  = m->spawnhealth / 100.0lk;
      i32 new_health  = level_delta * health_mul * m->rank / 5.0lk;
      SetHealth(0, m->health + new_health);
      m->maxhealth += new_health;
      k32 new_damage = level_delta / ACS_RandomFixed(100.0, 200.0);
      m->damagemul += new_damage;
      SetDamageMultiplier(0, m->damagemul);
      Dbg_Log(log_dmon,
              _l("monster "), _p(m->id), _l(" HP+"), _p(new_health ),
              _l(" DMG+"), _p(new_damage));
   }
   for(i32 i = 0; i < dmgtype_max; i++) {
      ifauto(i32, resist, m->resist[i] / 15.0) {
         InvGive(strp(_l(OBJ "M_"),
                      _p(sa_dmgtype_names[i]),
                      _p(mini(resist, _max_rank))),
                 1);
         Dbg_Log(log_dmon,
                 _l("monster "), _p(m->id), _c(' '), _p(sa_dmgtype_names[i]),
                 _l(" = "), _p(resist));
      }
   }
}

script static void ShowBarrier(dmon_t const *m, k32 alpha) {
   if(Paused) {
      return;
   }
   bool anyplayer = false;
   /* Optimization: Check for players nearby first. */
   if(aabb_point(m->x - 192, m->y - 192, 384, 384, pl.x, pl.y)) {
      anyplayer = true;
   }
   if(!anyplayer) {
      return;
   }
   BeginAngles(m->x, m->y);
   ServCallV(sm_MonsterBarrierLook);
   for(i32 i = 0; i < wl.a_cur; i++) {
      struct polar *a = &wl.a_angles[i];
      k32 dst = m->r / 2 + a->dst / 4;
      k32 x   = m->x + ACS_Cos(a->ang) * dst;
      k32 y   = m->y + ACS_Sin(a->ang) * dst;
      i32 tid = ACS_UniqueTID();
      k32 alp = (1 - a->dst / (256 * (m->rank - 1))) * alpha;
      str bar;
      switch(m->rank) {
      case 8:  bar = so_MonsterFavlosaura; break;
      case 7:  bar = so_MonsterFosaura;    break;
      case 6:  bar = so_MonsterSkotosaura; break;
      case 5:  bar = so_MonsterHeptaura;   break;
      default: bar = so_MonsterBarrier;    break;
      }
      ACS_SpawnForced(bar, x, y, m->z + m->h / 2, tid);
      SetAlpha(tid, clampk(alp, 0.0, 1.0));
   }
}

script static void BaseMonsterLevel(dmon_t *m) {
   k32 rlv = ACS_RandomFixed(1, _max_level);
   k32 bias;
   bias = wl.hubscleared / 40.0;
   bias *= bias;
   switch(ACS_GameSkill()) {
   case _skill_tourist:   bias += 0.1; break;
   default:
   case _skill_normal:    bias += 0.2; break;
   case _skill_hard:      bias += 0.3; break;
   case _skill_nightmare: bias += 0.5; break;
   }
   if(cv.sv_extrahard) {
      bias += 0.1;
   }
   bias += cv.sv_difficulty / 100.0;
   bias *= ACS_RandomFixed(1, 1.5);
   if(m->mi->type == mtype_darkone) {
      m->rank  = 8;
      m->level = _max_level + rlv + rlv * bias;
   } else if(get_bit(m->mi->flags, _mif_angelic)) {
      m->rank  = 7;
      m->level = 7 + rlv * bias;
   } else if(get_bit(m->mi->flags, _mif_dark)) {
      m->rank  = 6;
      m->level = 6 + rlv * bias;
   } else if(GetFun() & lfun_ragnarok) {
      m->rank  = _max_rank;
      m->level = _max_level + rlv * bias;
   } else {
      k32 rrn = ACS_RandomFixed(1, _max_rank);
      rlv += pl.attr.level / 2.0;
      m->rank  = clampi(rrn * bias * 2, 1, _max_rank);
      m->level = clampi(rlv * bias    , 1, _max_level);
   }
   switch(m->rank) {
   case 5: ServCallV(sm_SetTeleFogTo, so_TeleFog5); break;
   case 6: ServCallV(sm_SetTeleFogTo, so_TeleFog6); break;
   case 7: ServCallV(sm_SetTeleFogTo, so_TeleFog7); break;
   }
   if(HasResistances(m)) {
      for(i32 i = 0; i < m->rank; i++) {
         m->resist[ACS_Random(1, dmgtype_max) - 1] += 5;
      }
   }
   ApplyLevels(m, 0);
   Dbg_Log(log_dmon,
           _l("monster "), _p(m->id), _l(" \Cdr"), _p(m->rank), _l(" \Cgl"),
           _p(m->level), _l(" \C-running on "), _p(ACS_GetActorClass(0)),
           _l(" filter "), _p((cstr)m->mi->name));
}

/* Spawn a Monster Soul and temporarily set the species of it until the
 * actor is no longer solid, so it won't explode immediately.
 */
alloc_aut(0) script static void SoulCleave(dmon_t *m) {
   i32 tid = ACS_UniqueTID();
   ACS_SpawnForced(so_MonsterSoul, m->x, m->y, m->z + 16, tid);
   SetDamage(tid, m->level / 8 * 7);
   PtrSet(tid, AAPTR_DEFAULT, AAPTR_TARGET, pl.tid);
   SetSpecies(tid, GetSpecies(0));
   for(i32 i = 0; ACS_CheckFlag(0, sm_solid) && i < 15; i++) {
      ACS_Delay(1);
   }
   SetSpecies(tid, so_Player);
}

static void SpawnManaPickup(dmon_t *m) {
   i32 i = 0;
   do {
      i32 tid = ACS_UniqueTID();
      i32 x   = m->x + ACS_Random(-16, 16);
      i32 y   = m->y + ACS_Random(-16, 16);
      ACS_Spawn(so_ManaPickup, x, y, m->z + 4, tid);
      PtrSet(tid, AAPTR_DEFAULT, AAPTR_TRACER, pl.tid);
      PtrSet(tid, AAPTR_DEFAULT, AAPTR_TARGET, pl.tid);
      i += 150;
   } while(i < m->maxhealth);
}

static void OnFinalize(dmon_t *m) {
   if(PtrPlayerNumber(0, AAPTR_TARGET) >= 0) {
      if(pl.sgacquired) {
         bool high_level_imp =
            m->mi->type == mtype_imp && m->level >= 70 && m->rank >= 4;
         if(high_level_imp && ACS_Random(0, 100) < 4) {
            ACS_SpawnForced(so_ClawOfImp, m->x, m->y, m->z);
         }
      }
      if(!m->finalized) {
         if((get_bit(pl.upgrades[UPGR_Magic].flags, _ug_active) ||
             get_bit(pl.upgrades[UPGR_MagicW].flags, _ug_active)) &&
            pl.mana != pl.manamax &&
            (m->mi->type != mtype_zombie || ACS_Random(0, 50) < 10)) {
            SpawnManaPickup(m);
         }
         if(get_bit(pl.upgrades[UPGR_SoulCleaver].flags, _ug_active)) {
            SoulCleave(m);
         }
      }
      enum {
         _max_level_top4th = _max_level - _max_level / 4,
         _max_level_tophlf = _max_level / 2,
      };
      i32 expadd = 0;
      /**/ if(pl.health <  5) expadd += 50;
      else if(pl.health < 15) expadd += 25;
      else if(pl.health < 25) expadd += 10;
      /**/ if(m->level > _max_level_top4th) expadd += m->mi->exp / 10;
      else if(m->level > _max_level_tophlf) expadd += m->mi->exp / 60;
      if(m->rank >= 5) expadd += m->mi->exp / 60;
      attr_giveexp(expadd + m->mi->exp);
   }
   m->finalized = true;
}

script static void OnDeath(dmon_t *m) {
   Dbg_Log(log_dmon, _l("monster "), _p(m->id), _l(" is ded"));
   m->wasdead = true;
   OnFinalize(m);
   P_Scr_Give(m->x, m->y, m->z, m->mi->score + m->rank * 500, false);
}

script static void OnResurrect(dmon_t *m) {
   Dbg_Log(log_dmon, _l("monster "), _p(m->id), _l(" resurrected"));
   m->resurrect = false;
}

_Noreturn alloc_aut(0) script static void MonsterMain(dmon_t *m) {
   InvGive(so_MonsterID, m->id + 1);
   GetInfo(m);
   m->spawnhealth = m->maxhealth = m->health;
   m->damagemul = 1.0;
   BaseMonsterLevel(m);
   for(i32 tic = 0;; tic++) {
      GetInfo(m);
      if(m->health <= 0) {
         OnDeath(m);
         do {
            ACS_Delay(15);
         } while(!m->resurrect);
         OnResurrect(m);
      }
      if(m->exp > _monster_level_exp) {
         i32 prev = m->level;
         i32div d = __div(m->exp, _monster_level_exp);
         m->level += d.quot;
         m->exp    = d.rem;
         ACS_SpawnForced(so_MonsterLevelUp, m->x, m->y, m->z);
         ApplyLevels(m, prev);
      }
      if(HasResistances(m) && m->level >= 20) {
         ShowBarrier(m, m->level / 100.0);
      }
      if(InvNum(so_Ionized) && tic % 5 == 0) {
         ServCallV(sm_IonizeFX);
      }
      ACS_Delay(2);
   }
}

#ifndef NDEBUG
void PrintMonsterInfo(dmon_t *m) {
   ACS_BeginPrint();
   __nprintf(
      "m=%p mi=%p active=%u id=%.3u\n"
      "wasdead=%u finalized=%u resurrect=%u\n"
      "level=%.3i rank=%i exp=%i\n"
      "health=%i/%i\n"
      "x=%k y=%k z=%k\n"
      "r=%k h=%k\n"
      "damagemul=%k painwait=%i\n"
      "mi->prename=`%s' mi->name=`%s'\n"
      "mi->exp=%u mi->score=%" FMT_SCR "\n"
      "mi->flags=%i mi->type=%i mi->mass=%i\n",
      m, m->mi, m->active, m->id,
      m->wasdead, m->finalized, m->resurrect,
      m->level, m->rank, m->exp,
      m->health, m->maxhealth,
      m->x, m->y, m->z,
      m->r, m->h,
      m->damagemul, m->painwait,
      m->mi->prename, m->mi->name,
      m->mi->exp, m->mi->score,
      m->mi->flags, m->mi->type, m->mi->mass);
   for(i32 i = 0; i < countof(m->resist); i++) {
      __nprintf("resist[%S]=%i ", sa_dmgtype_names[i], m->resist[i]);
   }
   EndLogEx(_pri_bold|_pri_nonotify);
}
#endif

script void MonInfo_Preset(struct tokbuf *tb, struct err *res) {
   noinit static mon_name_t k, v;
   struct monster_preset *pre = &monsterpreset[monsterpresetnum++];
   while(tb_kv(tb, res, k, v)) {
      unwrap(res);
      switch(MonInfo_Preset_Name(k)) {
      case _moninfo_preset_name: faststrcpy(pre->prename, v);  break;
      case _moninfo_preset_exp:  pre->exp   = faststrtoi32(v); break;
      case _moninfo_preset_scr:  pre->score = faststrtoscr(v); break;
      default:
         tb_err(tb, res, "invalid key %s; expected "
                "name, "
                "exp, "
                "or scr",
                nil, _f, k);
         unwrap_retn();
      }
   }
   unwrap(res);
   Dbg_Log(log_gsinfo,
           _l("preset "), _p((cstr)pre->prename), _l(" added: exp = "),
           _p(pre->exp), _l(", score = "), _p(pre->score));
}

struct monster_info *MonInfo_BeginDef(void) {
   return &monsterinfo[monsterinfonum];
}

void MonInfo_FinishDef(struct monster_info *mi) {
   ++monsterinfonum;
   Dbg_Log(log_gsinfo,
           _l("monster "), _p((cstr)mi->name), _l(" added: type = "),
           _p(mi->type), _l(", flags = "), _p(mi->flags));
}

void MonInfo_ColorfulHellHack(struct monster_info *mi) {
   static cstr const colors[] = {
      "Common", "Green", "Blue", "Cyan", "Purple", "Yellow", "FireBlu",
      "Red", "Gray", "Abyss", "Black", "White", "Special"
   };
   struct monster_info original = *mi;
   for(mem_size_t i = 0; i < countof(colors); ++i) {
      *mi = original;
      faststrcpy2(mi->name, colors[i], original.name);
      MonInfo_FinishDef(mi);
      mi = MonInfo_BeginDef();
   }
}

script static void MonInfo_Monster(struct tokbuf *tb, struct err *res, i32 flags) {
   noinit static mon_name_t k, v;
   struct monster_preset *pre = &monsterpreset[monsterpresetnum++];
   struct monster_info *mi = MonInfo_BeginDef();
   bool finished = false;
   mi->flags = flags;
   while(tb_kv(tb, res, k, v)) {
      unwrap(res);
      switch(MonInfo_Monster_Name(k)) {
      case _moninfo_monster_filter: faststrcpy(mi->name, v);     break;
      case _moninfo_monster_exp:    mi->exp   = faststrtoi32(v); break;
      case _moninfo_monster_scr:    mi->score = faststrtoscr(v); break;
      case _moninfo_monster_mass:   mi->mass  = faststrtoi32(v); break;
      case _moninfo_monster_pre:
         for(struct monster_preset *pre = monsterpreset;; ++pre) {
            if(faststrchk(pre->prename, v)) {
               mi->pre = *pre;
               break;
            }
         }
         break;
      case _moninfo_monster_type: {
         i32 mtyp = MonInfo_Monster_TypeName(v);
         if(mtyp != -1) {
            mi->type = mtyp;
         }
         break;
      }
      case _moninfo_monster_hacks: {
         i32 hacks = tb_rflag(tb, res, v, MonInfo_Monster_HackName);
         unwrap(res);
         if(get_bit(hacks, _moninfo_hack_rh)) {
            set_bit(mi->flags, _mif_replacement_heuristics);
         }
         if(get_bit(hacks, _moninfo_hack_ch)) {
            if(get_bit(wl.compat, _comp_ch)) {
               MonInfo_ColorfulHellHack(mi);
            }
            finished = true;
         }
         break;
      }
      default:
         tb_err(tb, res, "invalid key %s; expected "
                "filter, "
                "exp, "
                "scr, "
                "pre, "
                "type, "
                "or hacks",
                nil, _f, k);
         unwrap_retn();
      }
   }
   unwrap(res);
   if(!finished) {
      MonInfo_FinishDef(mi);
   }
}

static i32 MonInfo_Flags(struct tokbuf *tb, struct err *res) {
   gosub_enable();
   char         *flag, *next, c;
   i32           flags, flgn;
   struct token *tok;
   flags = 0;
   tok   = tb_reget(tb);
   /* this supports two syntaxes since we originally implemented this
    * with a more complex syntax but realized it was pointless later,
    * however the previous syntax will still be supported since there
    * are a few mods that likely won't change with this, and maybe
    * someone just likes how it looks (it's trivial to maintain, w/e)
    */
   if(tok->type == tok_string) {
      for(c    = '"',
          next = nil,
          flag = faststrtok(tok->textV, &next, ' ');
          flag;
          flag = faststrtok(nil, &next, ' '))
      {
         gosub(sflag, flag);
      }
   } else {
      for(c = ')';;) {
         tok = tb_expc(tb, res, tb_get(tb), tok_identi, tok_parenc, 0);
         unwrap(res);

         if(tok->type == tok_parenc) {
            break;
         } else {
            gosub(sflag, flag = tok->textV);
         }
      }
   }
   return flags;

sflag:
   flgn = MonInfo_Flag_Name(flag);
   if(flgn != -1) {
      set_bit(flags, flgn);
      gosub_ret();
   } else {
      tb_err(tb, res, "invalid flag %s; expected "
             #define monster_flag_x(name) #name ", "
             #include "w_monster.h"
             "or `%c'",
             tok, _f, flag, c);
      unwrap_retn();
   }
}

script static void MonInfo_Monsters(struct tokbuf *tb, struct err *res) {
   i32 flags = MonInfo_Flags(tb, res);
   unwrap(res);
   tb_expdr(tb, res, tok_braceo);
   unwrap(res);
   while(!tb_drop(tb, tok_bracec)) {
      MonInfo_Monster(tb, res, flags);
      unwrap(res);
   }
}

script static void MonInfo_Presets(struct tokbuf *tb, struct err *res) {
   tb_expdr(tb, res, tok_braceo);
   unwrap(res);
   while(!tb_drop(tb, tok_bracec)) {
      MonInfo_Preset(tb, res);
      unwrap(res);
   }
}

script static void MonInfo_Compile(struct tokbuf *tb, struct err *res) {
   for(;;) {
      struct token *tok = tb_expc(tb, res, tb_get(tb), tok_pareno, tok_string, tok_identi, tok_eof, 0);
      unwrap(res);
      switch(tok->type) {
      case tok_eof:
         return;
      case tok_string:
      case tok_pareno:
         MonInfo_Monsters(tb, res);
         unwrap(res);
         break;
      case tok_identi:
         switch(MonInfo_TopLevelName(tok->textV)) {
         case _moninfo_toplevel_presets:
            MonInfo_Presets(tb, res);
            unwrap(res);
            break;
         default:
            tb_err(tb, res, "invalid toplevel token", tok, _f);
            unwrap_retn();
            break;
         }
         break;
      }
   }
}

script void Mon_Init(void) {
   Dbg_Log(log_dev, _l(_f));
   monsterpresetnum = 0;
   monsterinfonum   = 0;
   i32 prev = 0;
   for(FILE *fp; (fp = W_OpenIter(sp_LITHMONS, 't', &prev));) {
      struct tokbuf tb;
      tb_ctor(&tb, fp, "LITHMONS");
      struct err res = {};
      MonInfo_Compile(&tb, &res);
      unwrap_print(&res);
      tb_dtor(&tb);
      fclose(fp);
   }
}

script ext("ACS") addr(lsc_monstertype) i32 Z_GetMonsterType(void) {
   ifauto(dmon_t *, m, DmonSelf()) {
      return m->mi->type;
   } else {
      return mtype_unknown;
   }
}

script_str ext("ACS") addr(OBJ "GiveMonsterEXP") void Z_GiveMonsterEXP(i32 amt) {
   ifauto(dmon_t *, m, DmonSelf()) {
      m->exp += amt;
   }
}

script_str ext("ACS") addr(OBJ "ResurrectMonster") void Z_ResurrectMonster(i32 amt) {
   ifauto(dmon_t *, m, DmonSelf()) {
      m->resurrect = true;
   }
}

alloc_aut(0) script ext("ACS") addr(lsc_monsterinfo) void Z_MonsterInfo(void) {
   noinit static mon_name_t cname;
   faststrcpy_str(cname, ACS_GetActorClass(0));
   i32 mass = GetMass(0);
   for(i32 i = monsterinfonum - 1; i >= 0; --i) {
      struct monster_info const *mi = &monsterinfo[i];
      bool init;
      if(get_bit(mi->flags, _mif_full)) {
         if(get_bit(mi->flags, _mif_nocase)) {
            init = faststrcasechk(cname, mi->name);
         } else {
            init = faststrchk(cname, mi->name);
         }
      } else {
         if(get_bit(mi->flags, _mif_nocase)) {
            init = faststrcasestr(cname, mi->name);
         } else {
            init = faststrstr(cname, mi->name);
         }
      }
      init = init && (!mi->mass || mass == mi->mass);
      if(!init && get_bit(mi->flags, _mif_replacement_heuristics) &&
         ServCallI(sm_CheckReplacementHeuristics, fast_strdup(mi->name)))
      {
         init = true;
      }
      if(init) {
         if(!get_bit(mi->flags, _mif_nonmonster)) {
            dmon_t *m = AllocDmon();
            m->mi = mi;
            MonsterMain(m);
         }
         return;
      }
   }
   PrintErr(_l("no monster filter for "), _p((cstr)cname));
}

script_str ext("ACS") addr(OBJ "MonsterFinalized") void Z_MonsterFinalized(void) {
   ifauto(dmon_t *, m, DmonSelf()) {
      OnFinalize(m);
   }
}

/* EOF */
