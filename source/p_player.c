// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Player entry points.                                                     │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "m_list.h"
#include "p_hudid.h"
#include "w_world.h"
#include "d_vm.h"

#include <limits.h>

noinit struct player pl;

script static void P_bossWarning(void);
script static void P_bossText(void);
script static void P_doIntro(void);
static void P_Scr_PTickPre(void);
static void P_Spe_pTick(void);
static void P_Atr_pTick(void);
static void P_Aug_pTick(void);
static void P_initCbi(void);

dynam_aut script void P_Player(void) {
   Dbg_Log(log_dev, _l(_f));

   pl.setActivator();

   if(get_msk(ml.flag, _mflg_func) != _mfunc_normal) {
      ACS_SetPlayerProperty(true, true, PROP_TOTALLYFROZEN);
      return;
   }

reinit:
   P_Init();
   P_Log_Entry();
   P_Upg_Enter();
   P_Data_Load();
   P_doIntro();
   P_bossWarning();
   P_bossText();
   if(pl.teleportedout) P_TeleportIn();
   P_initCbi();

   struct old_player_delta olddelta;

   i32 idletics = 0;
   for(;;) {
      if(pl.reinit) {
         goto reinit;
      }

      /* Check for resurrect */
      if(pl.health > 0 && pl.dead) {
         pl.reinit = true;
      }

      /* Update data */
      P_Dat_PTickPre();

      olddelta.del    = pl.cur;
      olddelta.health = pl.health;
      olddelta.mana   = pl.mana;
      olddelta.shield = pl.shield;

      /* Tick all systems */
      if(!pl.dead) {
         if(!Paused) {
            if(pl.x == pl.old.x && pl.y == pl.old.y && pl.z == pl.old.z) {
               ++idletics;
            } else {
               idletics = 0;
            }
         }
         if(idletics > 38) {
            pl.missionstatshow = ACS_Timer() + 35;
         }
         P_Wep_PTickPre();
         P_Scr_PTickPre();
         P_Spe_pTick();
         P_Upg_PTick();
         P_Inv_PTick();
         P_CBI_PTick();
         P_Aug_pTick();
         P_Atr_pTick();
         P_Wep_PTick();
         P_Log_PTick();
         P_Dat_PTickPst();
         P_Ren_PTickPst();
      }

      /* Tic passes */
      ACS_Delay(1);

      if(pl.dlg.num) {
         Dlg_Run(pl.dlg.num);
         pl.dlg.num = 0;
      }

      /* Update post-tic values */
      pl.old = olddelta;
      P_ValidateTID();
      pl.ticks++;
   }
}

alloc_aut(0) script static void revenge(void) {
   if(CVarGetI(sc_sv_revenge)) {
      AmbientSound(ss_player_death1, 1.0);
      ACS_Delay(35);
      ServCallV(sm_PlayerDeath);
      ACS_Delay(25);
      ServCallV(sm_PlayerDeathNuke);
      ACS_Delay(25);
   }
}

dynam_aut script type("death") static void Z_PlayerDeath(void) {
   pl.dead = true;

   P_Upg_PDeinit();
   pl.scoreaccum = pl.scoreaccumtime = 0;
   revenge();

   str deathmsg = snil;
   for(i32 time = 0; pl.dead; ++time) {
      if(time != 0 && time % (35 * 30) == 0) {
         deathmsg = ns(lang_fmt(LANG "DEATHMSG_%.2i", ACS_Random(1, 20)));
         SetFade(fid_deathmsg, 35 * 4, 24);
      }

      SetSize(320, 240);
      SetClipW(0, 0, 320, 240, 320);

      if(CheckFade(fid_deathmsg)) {
         PrintTextF_str(deathmsg, sf_smallfnt, CR_WHITE, 0,1, 240-8,2,
                        fid_deathmsg);
      }

      if(pl.obit) {
         PrintTextChS(pl.obit);
         PrintText(sf_smallfnt, CR_WHITE, 0,1, 240,2);
      }

      ClearClip();

      ACS_Delay(1);
   }

   pl.obit = nil;
}

script type("respawn") static void Z_PlayerRespawn(void) {
   pl.reinit = true;
}

script type("return") static void Z_PlayerReturn(void) {
   pl.reinit = true;
}

static void P_initCbi(void) {
   if(cv.sv_nobosses || cv.sv_nobossdrop || dbgflags(dbgf_items)) {
      for(i32 i = 0; i < bossreward_max; i++) {
         CBI_Install(i);
      }
   }

   CBI_InstallSpawned();
}

alloc_aut(0) stkcall cstr P_Discrim(i32 pclass) {
   switch(pclass) {
   case pcl_marine:    return "Lane";
   case pcl_cybermage: return "Jem";
   case pcl_informant: return "Fulk";
   case pcl_wanderer:  return "Luke";
   case pcl_assassin:  return "Omi";
   case pcl_darklord:  return "Ari";
   case pcl_thoth:     return "Kiri";
   }
   return "";
}

alloc_aut(0) stkcall i32 P_Color(i32 pclass) {
   switch(pclass) {
   case pcl_marine:    return Cr(green);
   case pcl_cybermage: return Cr(red);
   case pcl_informant: return Cr(purple);
   case pcl_wanderer:  return Cr(yellow);
   case pcl_assassin:  return Cr(pink);
   case pcl_darklord:  return Cr(blue);
   case pcl_thoth:     return Cr(grey);
   }
   return CR_WHITE;
}

void P_GUI_Close(void) {
   if(pl.modal == _gui_cbi) {
      UnfreezeTime();

      AmbientSound(ss_player_cbi_close, 1.0);
      pl.modal = _gui_none;
   }
}

void P_GUI_Use(void) {
   if(pl.dead) return;

   switch(pl.modal) {
   case _gui_cbi:
      P_GUI_Close();
      break;
   case _gui_none:
      FreezeTime();

      if(ACS_Random(0, 10000) == 777) {
         AmbientSound(ss_player_cbi_win95, 1.0);
      } else {
         AmbientSound(ss_player_cbi_open, 1.0);
      }

      pl.modal = _gui_cbi;
      break;
   }
}

score_t P_Scr_Give(k32 x, k32 y, k32 z, score_t score, bool nomul) {
   bool seen;
   struct i32v2 vp = project(x, y, z, &seen);
   if(!seen) {
      vp.x = 320/2 + ACS_Random(-64, 64);
      vp.y = 240/2 + ACS_Random(-48, 48);
   }
   return P_Scr_GivePos(vp.x, vp.y, score, nomul);
}

score_t P_Scr_GivePos(i32 x, i32 y, score_t score, bool nomul) {
   /* Could cause division by zero */
   if(score == 0) {
      return 0;
   }

   if(!nomul) {
      score *= pl.scoremul / 100.0lk;
      score *= 1 + (k64)ACS_RandomFixed(0, pl.attr.attrs[at_luk] / 77.7);
   }

   /* Get a multiplier for the score accumulator */
   k32 mul = clampk(clampscr(score, 0, 15000) / 15000.0k, 0.1k, 1.0k);

   /* Play a sound when we pick up score */
   if(cv.player_scoresound) {
      k32 vol = 0.7k * mul;
      if(vol > 0.001k) {
         StartSound(ss_player_score, lch_item2, 0, vol, ATTN_STATIC);
      }
   }

   /* hue */
   if(get_bit(pl.upgrades[UPGR_CyberLegs].flags, _ug_active) && ACS_Random(0, 10000) == 0) {
      pl.brouzouf += score;
      P_LogB(1, "You gained brouzouf.");
   }

   if(get_bit(pl.upgrades[UPGR_TorgueMode].flags, _ug_active) && ACS_Random(0, 10) == 0) {
      pl.spuriousexplosions++;
      ACS_SpawnForced(so_EXPLOOOSION, pl.x, pl.y, pl.z);
   }

   /* Add score and set score accumulator */
   pl.score          += score;
   pl.scoresum       += score;
   pl.scoreaccum     += score;
   pl.scoreaccumtime += 20 * mul * 2;

   /* display score */
   if(cv.player_scoredisp & _itm_disp_log) {
      P_LogH(1, "+\Cj%" FMT_SCR "\Cnscr", score);
   }
   if(cv.player_scoredisp & _itm_disp_pop) {
      DrawCallV(sm_AddScoreNum, x, y, strp(_p(score), _l("\Cnscr")));
   }

   return score;
}

void P_Scr_Take(score_t score) {
   if(pl.score - score >= 0) {
      pl.scoreused += score;
      pl.score     -= score;
   } else {
      pl.scoreused += pl.score;
      pl.score      = 0;
   }

   pl.scoreaccum     = 0;
   pl.scoreaccumtime = 0;
}

script static void P_bossWarningDone(void) {
   if(get_msk(ml.flag, _mflg_boss) == _mphantom_spawned) {
      P_LogB(1, tmpstr(lang_discrim(sl_log_bosswarn)));
   }
}

alloc_aut(0) script static void P_bossWarning(void) {
   ACS_Delay(35 * 5);
   P_bossWarningDone();
}

dynam_aut script static
void P_bossText(void) {
   if(ml.boss == boss_iconofsin && EDataI(_edt_rampancy)) {
      return;
   }

   bool division = get_bit(ml.flag, _mflg_corrupted);

   if(division) {
      for(i32 i = 0; i < 35*2; i++) {
         SetSize(320, 240);
         PrintFill(0xFF070707);
         PrintText_str(ns(lang(sl_verse_corrupted)), sf_lmidfont, CR_WHITE, 160,4, 120,0);
         ACS_Delay(1);
      }
      P_LogB(1, tmpstr(lang(sl_log_bosshear3)));
   }
}

alloc_aut(0) stkcall static i32 P_playerColor(void) {
   switch(pl.pclass) {
   case pcl_marine:    return 0xFF00FF00;
   case pcl_cybermage: return 0xFFBF0F4A;
   case pcl_informant: return 0xFF8C3BF8;
   case pcl_wanderer:  return 0xFFFFD023;
   case pcl_assassin:  return 0xFFE873AF;
   case pcl_darklord:  return 0xFF3B47F8;
   case pcl_thoth:     return 0xFFBFBFBF;
   }
   return 0xFF000000;
}

alloc_aut(0) stkcall script static void P_doDepthMeter(void) {
   enum {
      scale      = 8,
      y          = 110,
      y_scaled   = (y + 1) * scale,
      notch_sx   = 12      * scale,
      notch_sy   = 1       * scale,
      notch_dist = 4       * scale,
      scr_w      = 320     * scale,
      scr_h      = 240     * scale,
      t_initial  = 15,
      t_move     = 70,
      t_wait     = 35,
      t_out      = 35,
      s_initial = 0, e_initial = s_initial + t_initial,
      s_move,        e_move    = s_move    + t_move,
      s_wait,        e_wait    = s_wait    + t_wait,
      s_out,         e_out     = s_out     + t_out,
   };
   i32 prev_level = (wl.hubscleared     & 31) * notch_dist;
   i32 next_level = (wl.hubscleared + 1 & 31) * notch_dist;
   i32 player_clr = P_playerColor() & 0xFFFFFF;
   ACS_Delay(8);
   for(i32 i = 0; i < e_out; ++i) {
      if(i == s_out && cv.player_resultssound) {
         AmbientSound(ss_player_depthdown, 1.0);
      }
      i32 curr_level = lerpk(prev_level, next_level, i <= e_move ? ease_out_cubic(maxi(i - e_initial, 0) / (k32)t_move) : 1.0k);
      k32 alpha_k = i >= s_out ? 1.0 - ease_out_cubic((i - e_wait) / (k32)t_out) : 1.0;
      i32 alpha_i = 0xFF * alpha_k;
      i32 alpha_m = alpha_i << 24;
      i32 cr = 0xFFFF0000 | (alpha_i << 8) | alpha_i;
      SetSize(320, 240);
      PrintTextAX_str(st_depth, sf_smallfnt, CR_WHITE, 320,2, y,2, alpha_k, _u_no_unicode);
      SetSize(320*scale, 240*scale);
      for(i32 j = 0; j < 16; ++j) {
         i32 notch_w = j % 3 == 0 ? notch_sx : notch_sx / 3;
         PrintRect(scr_w - notch_w, y_scaled + notch_dist * j, notch_w, notch_sy, cr);
      }
      PrintRect(scr_w - notch_sx, y_scaled + curr_level, notch_sx, notch_sy, player_clr | alpha_m);
      ACS_Delay(1);
   }
}

alloc_aut(0) stkcall script static void P_doIntro(void) {
   pl.missionstatshow = ACS_Timer() + 70;

   if(wl.hubscleared != 0 || pl.done_intro & pl.pclass) {
      P_doDepthMeter();
      return;
   }

   enum {
      _nlines   = 26,
      _out_tics = 35 * 2,
   };

   noinit static char text[8192], *lines[_nlines];
   noinit static i32 linec[_nlines], linen[_nlines];

   pl.modal = _gui_intro;
   ACS_SetMusic(sp_DSEMPTY);
   FreezeTime(false);
   ACS_FadeTo(0, 0, 0, 1.0, 0.0);

   register i32 which = 1;
   register i32 last  = 0;

   noinit static struct gui_fil fil;
   fil.tic = 70;

   text[0] = '\0';

   for(;;) {
      SetSize(320, 240);
      if(which != last) {
         last = which;

         ACS_BeginPrint();
         PrintChrLi(LANG "BEGINNING_");
         PrintChrSt(pl.discrim);
         ACS_PrintChar('_');
         ACS_PrintInt(which);
         str next_text = lang(ACS_EndStrParam());
         if(!next_text) {
            break;
         }

         faststrcpy_str(text, next_text);

         AmbientSound(ss_player_showtext, 1.0);

         noinit static
         char *next, *line;
         next = nil;
         line = faststrtok(text, &next, '\n');
         for(i32 i = 0; i < _nlines; i++) {
            lines[i] = nil;
            linec[i] = 0;
            linen[i] = 0;

            if(text) {
               if(*text) {
                  lines[i] = line;
                  linec[i] = faststrlen(line);
               }

               line = faststrtok(nil, &next, '\n');
            }
         }
      }

      ACS_BeginPrint();
      ACS_PrintBind(sc_use);
      ACS_PrintString(ns(lang(sl_skip_intro_1)));
      ACS_PrintChar('\n');
      ACS_PrintBind(sc_attack);
      ACS_PrintString(ns(lang(sl_skip_intro_2)));
      PrintText(sf_smallfnt, CR_WHITE, 275,6, 220,0);

      if(G_Filler(280, 220, &fil, pl.buttons & (BT_USE | BT_ATTACK))) {
         if(pl.buttons & BT_ATTACK) {
            which++;
            continue;
         } else {
            break;
         }
      }

      for(i32 i = 0; i < _nlines; i++) {
         if(!lines[i]) continue;

         if(linen[i] < linec[i]) {
            linen[i]++;
         }

         if(lines[i][0] != '~') {
            PrintTextChr(lines[i], linen[i]);
            PrintText(sf_smallfnt, pl.color, 0,1, 8 * i,1);
         }
      }

      ACS_Delay(1);
   }

   UnfreezeTime(false);

   AmbientSound(ss_player_startgame, 1.0);
   ACS_FadeTo(0, 0, 0, 0.0, 2.0);

   for(i32 j = 0; j < _out_tics; j++) {
      k32 alpha = (_out_tics - j) / (k32)_out_tics;

      SetSize(320, 240);

      for(i32 i = 0; i < _nlines; i++) {
         if(!lines[i]) continue;

         if(lines[i][0] != '~') {
            PrintTextChr(lines[i], linen[i]);
            PrintTextA(sf_smallfnt, pl.color, 0,1, 8 * i,1, alpha);
         }

         for(i32 k = 0; k < linen[i]; k++) {
            if(ACS_Random(0, 100) < 1) {
               lines[i][k] = ACS_Random('!', '}');
            }
         }

         if(linen[i]) {
            linen[i]--;
         }
      }

      ACS_Delay(1);
   }

   ACS_SetMusic(sp_star);

   pl.done_intro |= pl.pclass;
   P_Data_Save();

   pl.modal = _gui_none;
}

static k32 damage_mul;
static i32 max_health;

static void P_attrRGE(void) {
   i32 rge = pl.attr.attrs[at_spc];

   if(pl.health < pl.old.health) {
      pl.rage += rge * (pl.old.health - pl.health) / 1000.0;
   }

   pl.rage = lerpk(pl.rage, 0, 0.02);

   damage_mul += pl.rage;
}

static void P_attrCON(void) {
   i32 con = pl.attr.attrs[at_spc];

   if(pl.mana > pl.old.mana) {
      pl.rage += con * (pl.mana - pl.old.mana) / 1100.0;
   }

   pl.rage = lerpk(pl.rage, 0, 0.03);

   damage_mul += pl.rage;
}

static void P_attrREF(void) {
   i32 ref = pl.attr.attrs[at_spc];

   if(pl.health < pl.old.health) {
      pl.rage += ref * (pl.old.health - pl.health) / 150.0;
   }

   pl.rage = lerpk(pl.rage, 0, 0.01);

   pl.speedmul += (i32)pl.rage;
}

static void P_Spe_pTick(void) {
   if(Paused || !get_bit(wl.cbiupgr, cupg_d_shield)) {
      return;
   }

   if(pl.shield == 0 && pl.old.shield != 0) {
      StartSound(ss_player_ari_shield_break, lch_shield, CHANF_MAYBE_LOCAL|CHANF_UI, 1.0, ATTN_STATIC);
      pl.regenwaitmax = 665;
      pl.regenwait    = 700;
      ACS_FadeTo(184, 205, 255, 0.3k, 0.0k);
      ACS_FadeTo(222, 5, 92, 0.0k, 3.0k);
   } else if(pl.shield < pl.old.shield) {
      ACS_StopSound(pl.tid, lch_shield);
      i32 diff = pl.old.shield - pl.shield;
      if(pl.regenwait <= 350) {
         pl.regenwaitmax = 315;
         pl.regenwait    = 350;
      }
      k32 amt = diff / (k32)pl.shieldmax;
      ACS_FadeTo(184, 205, 255, amt, 0.0k);
      ACS_FadeTo(222, 5, 92, 0.0k, amt*2.0k + 2.0k/35.0k);
   } else if(pl.regenwait) {
      --pl.regenwait;
      if(pl.regenwait == pl.regenwaitmax) {
         StartSound(ss_player_ari_shield_regenw, lch_auto, CHANF_MAYBE_LOCAL|CHANF_UI, 0.9, ATTN_STATIC);
      }
   } else if(pl.shield < pl.shieldmax) {
      if(pl.shield == pl.old.shield) {
         StartSound(ss_player_ari_shield_regenl, lch_shield, CHANF_MAYBE_LOCAL|CHANF_UI|CHANF_LOOP, 1.0, ATTN_STATIC);
      }
      if(ACS_Timer() % 3 == 0) {
         pl.setShield(pl.shield + 1);
      }
      if(pl.shield == pl.shieldmax) {
         StartSound(ss_player_ari_shield_regend, lch_shield, CHANF_MAYBE_LOCAL|CHANF_UI, 0.8, ATTN_STATIC);
         SetFade(fid_shielddone, 3, 8);
      }
   }
}

static void P_Atr_pTick(void) {
   if(Paused) return;

   k32  acc = pl.attr.attrs[at_acc] / 150.0;
   k32  def = pl.attr.attrs[at_def] / 170.0;
   i32 strn = pl.attr.attrs[at_str];
   i32  stm = pl.attr.attrs[at_stm];
   i32 stmt = 75 - stm;

   max_health = pl.spawnhealth + strn;
   damage_mul = 1.0 + acc;

   switch(pl.pclass) {
   case pcl_marine:    P_attrRGE(); break;
   case pcl_cybermage: P_attrCON(); break;
   case pcl_darklord:  P_attrREF(); break;
   }

   SetDamageMultiplier(0, damage_mul);
   SetMembI(0, sm_DmgFac, clampi(100 * def, 0, 100));

   pl.maxhealth = max_health;
   SetSpawnHealth(0, pl.maxhealth);

   if(pl.health < stm + 10 && (stmt < 2 || pl.ticks % stmt == 0)) {
      pl.setHealth(pl.health + 1);
   }
}

static void P_Scr_PTickPre(void) {
   if(!pl.scoreaccumtime || pl.score < pl.old.score) {
      pl.scoreaccum = 0;
      pl.scoreaccumtime = 0;
   }

   /**/ if(pl.scoreaccumtime > 0) pl.scoreaccumtime--;
   else if(pl.scoreaccumtime < 0) pl.scoreaccumtime++;
}

static void P_Aug_pTick(void) {
   for(i32 i = 0; i < 4; i++) {
      i32 total = 0;

      for_upgrade(upgr) {
         if(get_bit(upgr->agroups, i) && get_bit(pl.autobuy, i)) {
            if(P_Upg_Buy(upgr, true)) {
               total++;
               P_Upg_Toggle(upgr);
            }
         }
      }

      if(total) {
         AmbientSound(ss_player_cbi_auto_buy, 1.0);
         P_LogH(1, tmpstr(lang_fmt(LANG "LOG_AutoBuy%i", i + 1)), total, total != 1 ? "s" : "");
      }
   }
}

alloc_aut(0) script_str ext("ACS") addr(OBJ "Markiplier") void Z_MapMarker(i32 tid) {
   if(get_msk(ml.flag, _mflg_func) != _mfunc_normal) return;

   enum {ticks = 35 * 2};

   str text = GetNameTag(tid);

   ACS_Delay(5);

   for(i32 i = 0; i < ticks; i++) {
      k32 alpha;

      /**/ if(i < 15)         alpha = i / 15.0;
      else if(i > ticks - 15) alpha = (ticks - i) / 15.0;
      else                    alpha = 1.0;

      i32 x = fastabsk(ACS_Sin(i / (k32)ticks / 4.0)) / 4.0 * 800.0;

      SetSize(640, 480);
      PrintTextAX_str(text, sf_areaname, CR_WHITE, x,4, 80,0, alpha, _u_no_unicode);
      ACS_Delay(1);
   }
}

script_str type("net") ext("ACS") addr(OBJ "KeyBuyAutoGroup") void Z_KeyBuyAutoGroup(i32 grp) {
   if(grp < 0 || grp >= 4) {
      return;
   }

   i32 total = 0, success = 0;

   for_upgrade(upgr) {
      if(!get_bit(upgr->flags, _ug_owned) && get_bit(upgr->agroups, grp)) {
         total++;

         if(P_Upg_Buy(upgr, true)) {
            success++;
            P_Upg_Toggle(upgr);
         }
      }
   }

   char cr;
   str  snd;

   /**/ if(success ==     0) {cr = 'g'; snd = ss_player_cbi_auto_invalid;}
   else if(success != total) {cr = 'j'; snd = ss_player_cbi_auto_buy;}
   else                      {cr = 'q'; snd = ss_player_cbi_auto_buy;}

   AmbientSound(snd, 1.0);

   i32 fmt = total ? grp + 1 : grp + 5;

   P_LogH(1, tmpstr(lang_fmt(LANG "LOG_GroupBuy%i", fmt)), cr, success, total, success != 1 ? "s" : "");
}

script_str type("net") ext("ACS") addr(OBJ "KeyToggleAutoGroup") void Z_KeyToggleAutoGroup(i32 grp) {
   if(grp < 0 || grp >= 4) {
      return;
   }

   i32 total = 0;

   for_upgrade(upgr) {
      if(get_bit(upgr->flags, _ug_owned) && get_bit(upgr->agroups, grp)) {
         total++;
         P_Upg_Toggle(upgr);
      }
   }

   if(total) AmbientSound(ss_player_cbi_auto_toggle,  1.0);
   else      AmbientSound(ss_player_cbi_auto_invalid, 1.0);

   i32 fmt = total ? grp + 1 : grp + 5;
   P_LogH(1, tmpstr(lang_fmt(LANG "LOG_GroupToggle%i", fmt)));
}

alloc_aut(0) script_str type("net") ext("ACS") addr(OBJ "KeyGlare") void Z_KeyGlare(void) {
   ACS_FadeTo(255, 255, 255, 1.0, 0.0);

   AmbientSound(ss_player_glare, 1.0);
   ACS_LineAttack(0, pl.yaw, pl.pitch, 1, so_Dummy, st_none,
                  32767.0, FHF_NORANDOMPUFFZ | FHF_NOIMPACTDECAL);

   ACS_Delay(14);

   ACS_FadeTo(255, 255, 255, 0.0, 0.2);

   ACS_Delay(19);
}

_Noreturn dynam_aut script_str ext("ACS") addr(OBJ "TimelineInconsistent") void Z_TimelineInconsistent(void) {
   pl.setActivator();
   for(;;) {
      ACS_FadeTo(0, 0, 0, 1.0, 0.0);
      SetSize(320, 240);
      PrintText_str(ns(lang(sl_bad_timeline)), sf_bigupper, CR_WHITE, 160,4, 120,0);
      pl.setHealth(-1);
      ACS_Delay(1);
   }
}

alloc_aut(0) script_str ext("ACS") addr(OBJ "SetLane") void Z_SetLane(void) {
   SetFun(GetFun() | lfun_lane);
}

/* EOF */
