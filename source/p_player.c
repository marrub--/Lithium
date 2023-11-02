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

script static void P_bossWarnings(void);
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
   P_Dat_PTickPre();
   P_Log_Entry();
   P_Upg_Enter();
   P_Data_Load();
   P_doIntro();
   P_bossWarnings();
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
      olddelta.del     = pl.cur;
      olddelta.health  = pl.health;
      olddelta.mana    = pl.mana;
      olddelta.shield  = pl.shield;
      olddelta.buttons = pl.buttons;
      /* Tick all systems */
      if(!pl.dead) {
         if(!Paused) {
            if(pl.x == pl.old.x && pl.y == pl.old.y && pl.z == pl.old.z) {
               ++idletics;
            } else {
               idletics = 0;
            }
            if(idletics > 38 && pl.missionstatshow < 35) {
               pl.missionstatshow = 45;
            }
         }
         P_Wep_PTickPre();
         P_Scr_PTickPre();
         P_Spe_pTick();
         P_Upg_PTick();
         P_Inv_PTick();
         P_Ren_PTick();
         P_CBI_PTick();
         P_Aug_pTick();
         P_Atr_pTick();
         P_Wep_PTick();
         P_Log_PTick();
         P_Dat_PTick();
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
   }
}

alloc_aut(0) script static void revenge(void) {
   if(CVarGetI(sc_sv_revenge)) {
      StartSound(ss_player_death1, lch_body3, 0, 1.0, ATTN_NONE);
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
      if(CheckFade(fid_deathmsg)) {
         PrintText_str(deathmsg, sf_smallfnt, CR_WHITE, 0,1, 240-8,2, _u_fade, fid_deathmsg, 320);
      }
      if(pl.obit) {
         BeginPrintStr(pl.obit);
         PrintText(sf_smallfnt, CR_WHITE, 0,1, 240,2, 0, 0, 320);
      }
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

stkoff i32 P_Color(i32 pclass) {
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

stkoff i32 P_Char(i32 pclass) {
   switch(pclass) {
   case pcl_marine:    return 'm';
   case pcl_cybermage: return 'c';
   case pcl_informant: return 'i';
   case pcl_wanderer:  return 'w';
   case pcl_assassin:  return 'a';
   case pcl_darklord:  return 'd';
   case pcl_thoth:     return 't';
   }
   return '\0';
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
      score *= (k64)ACS_RandomFixed(1, attr_lukbuff());
   }

   /* Get a multiplier for the score accumulator */
   k32 mul = clampk(clampscr(score, 0, 15000) / 15000.0k, 0.1k, 1.0k);

   /* Play a sound when we pick up score */
   if(cv.player_scoresound) {
      k32 vol = 0.7k * mul;
      if(vol > 0.001k) {
         AmbientSound(ss_player_score, vol);
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

alloc_aut(0) script static void P_bossWarnings(void) {
   if(get_msk(ml.flag, _mflg_boss) == _mphantom_spawned) {
      FreezeTime();
      ACS_Delay(4);
      P_CenterNotification(sl_bosswarn, 105, CR_ORANGE, 0xFC8A2E);
      ACS_Delay(105);
      UnfreezeTime();
   }
   switch(ml.boss) {
   case boss_iconofsin:
      if(EDataI(_edt_rampancy)) {
         return;
      }
      break;
   default:
      return;
   }
   bool division = get_bit(ml.flag, _mflg_corrupted);
   if(division) {
      for(i32 i = 0; i < 105; i++) {
         k32 a = 1 - maxi(i - 70, 0) / 35.0k;
         SetSize(640, 480);
         PrintFill(0x070707 | (i32)(a * 255) << 24);
         PrintText_str(sl_verse_corrupted, sf_bigupper, CR_WHITE, 320,4, 240,0, _u_alpha, a);
         ACS_Delay(1);
      }
   }
   P_CenterNotification(sl_bosshear1, 100, CR_RED, 0xFF0000);
   ACS_Delay(135);
   P_CenterNotification(sl_bosshear2, 100, CR_RED, 0xFF0000);
   if(division) {
      ACS_Delay(135);
      P_CenterNotification(sl_bosshear3, 100, CR_DARKRED, 0x7F0000);
   }
}

stkoff static i32 P_playerColor(void) {
   switch(pl.pclass) {
   case pcl_marine:    return 0x00FF00;
   case pcl_cybermage: return 0xBF0F4A;
   case pcl_informant: return 0x8C3BF8;
   case pcl_wanderer:  return 0xFFD023;
   case pcl_assassin:  return 0xE873AF;
   case pcl_darklord:  return 0x3B47F8;
   case pcl_thoth:     return 0xBFBFBF;
   }
   return 0xFF000000;
}

alloc_aut(0) script static void P_doDepthMeter(void) {
   enum {
      scale      = 8,
      y          = 110,
      y_scaled   = (y + 1) * scale,
      notch_sx   = 12      * scale,
      notch_sy   = 1       * scale,
      notch_dist = 4       * scale,
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
   i32 scr_w      = pl.hudrpos * scale;
   i32 prev_level = (wl.hubscleared     & 31) * notch_dist;
   i32 next_level = (wl.hubscleared + 1 & 31) * notch_dist;
   i32 player_clr = P_playerColor();
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
      PrintText_str(st_depth, sf_smallfnt, CR_WHITE, pl.hudrpos,2, y,2, _u_no_unicode|_u_alpha, alpha_k);
      SetSize(320*scale, 240*scale);
      for(i32 j = 0; j < 16; ++j) {
         i32 notch_w = j % 3 == 0 ? notch_sx : notch_sx / 3;
         PrintRect(scr_w - notch_w, y_scaled + notch_dist * j, notch_w, notch_sy, cr);
      }
      PrintRect(scr_w - notch_sx, y_scaled + curr_level, notch_sx, notch_sy, player_clr | alpha_m);
      ACS_Delay(1);
   }
   if(wl.hubscleared) {
      ACS_Delay(2);
      P_DoAutoSave();
   }
}

alloc_aut(0) script static void P_doIntro(void) {
   pl.missionstatshow = 35*3;
   if(wl.hubscleared != 0 || get_bit(pl.done_intro, pl.pclass)) {
      P_doDepthMeter();
      return;
   }
   enum {
      _nlines   = 26,
      _out_tics = 35 * 2,
      _tut_tics = 297,
   };
   noinit static char text[8192], *lines[_nlines];
   noinit static i32 linec[_nlines], linen[_nlines];
   pl.modal = _gui_intro;
   ACS_SetMusic(sp_NoMusic);
   FreezeTime(false);
   ACS_FadeTo(0, 0, 0, 1.0, 0.0);
   register i32 which = 1;
   register i32 last  = 0;
   static struct gui_fil fil = {70};
   text[0] = '\0';
   for(;;) {
      SetSize(320, 240);
      if(which != last) {
         last = which;
         str next_text = lang(strp(_l(LANG "BEGINNING_"), _p((cstr)pl.discrim), _c('_'), _p(which)));
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
      _c(' ');
      _p(sl_skip_intro_1);
      _c('\n');
      ACS_PrintBind(sc_attack);
      _c(' ');
      _p(sl_skip_intro_2);
      PrintText(sf_smallfnt, CR_WHITE, 275,6, 220,0);
      if(G_Filler(280, 220, &fil, P_ButtonHeld(BT_USE | BT_ATTACK))) {
         if(P_ButtonHeld(BT_ATTACK)) {
            which++;
            continue;
         } else {
            break;
         }
      }
      for(i32 i = 0; i < _nlines; i++) {
         if(!lines[i]) {
            continue;
         }
         if(linen[i] < linec[i]) {
            linen[i]++;
         }
         if(lines[i][0] != '~') {
            BeginPrintStrN(lines[i], linen[i]);
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
         if(!lines[i]) {
            continue;
         }
         if(lines[i][0] != '~') {
            BeginPrintStrN(lines[i], linen[i]);
            PrintText(sf_smallfnt, pl.color, 0,1, 8 * i,1, _u_alpha, alpha);
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
   set_bit(pl.done_intro, pl.pclass);
   P_Data_Save();
   pl.modal = _gui_none;
   /* alert the player to open the menu */
   static struct fmt_arg fmt_args[] = {{_fmt_key}};
   fmt_args[0].val.s = sc_k_opencbi;
   str tut_txt = strp(printfmt(tmpstr(sl_open_menu), 1, fmt_args));
   static bool sync;
   sync = false;
   P_CenterNotification(tut_txt, _tut_tics, -1, -1, 0, 0, &sync);
   sync(sync);
   ACS_Delay(2);
   P_DoAutoSave();
}

alloc_aut(0) script void P_CenterNotification(str txt, i32 tics, i32 cr, i32 linecr, k32 bgfade, k32 fgfade, bool *sync) {
   static struct i32v4 src_rect, dst_rect;
   TextSize((void *)&src_rect, txt, sf_bigupper);
   src_rect.z = src_rect.x + 8;
   src_rect.w = src_rect.y + 8;
   src_rect.x = 240 - src_rect.x / 2 - 4;
   src_rect.y = 180 - src_rect.y / 2 - 4;
   if(bgfade < 1) {
      ACS_FadeTo(0, 0, 0, 0.2 * (1 - bgfade), 0.2);
   }
   ACS_Delay(3);
   AmbientSound(ss_player_cbi_centernotif, 0.6k);
   linecr = linecr == -1 ? P_playerColor() : linecr;
   cr     = cr     == -1 ? pl.color        : cr;
   for(i32 j = 0; j < tics; j++) {
      k32 a = 1 - maxi(j - (tics - 35), 0) / 35.0k;
      k32 t = 1 - mini(j, 24) / 24.0k;
      i32 line_cr = linecr + ((i32)(a * 255) << 24);
      SetSize(480, 360);
      dst_rect.x = src_rect.x - 16 * t;
      dst_rect.y = src_rect.y - 24 * t;
      dst_rect.z = src_rect.z + 32 * t;
      dst_rect.w = src_rect.w + 48 * t;
      if(fgfade < 1) {
         PrintRect(dst_rect.x, dst_rect.y, dst_rect.z, dst_rect.w, (i32)(a * (1 - fgfade) * 207) << 24);
      }
      for(i32 i = 0; i < 4 - (t == 0 ? 3 : 0); ++i) {
         i32 xn = i * 24 * t / 2;
         i32 yn = i * 16 * t / 2;
         i32 x1 = dst_rect.x - xn;
         i32 y1 = dst_rect.y - yn;
         i32 x2 = dst_rect.x + dst_rect.z + xn;
         i32 y2 = dst_rect.y + dst_rect.w + yn;
         PrintLine(x1, y1, x2, y1, line_cr);
         PrintLine(x2, y1, x2, y2, line_cr);
         PrintLine(x2, y2, x1, y2, line_cr);
         PrintLine(x1, y2, x1, y1, line_cr);
      }
      PrintText_str(txt, sf_bigupper, cr, 240,4, 180,0, _u_alpha, a);
      ACS_Delay(1);
   }
   ACS_FadeTo(0, 0, 0, 0.0, 0.2);
   ACS_Delay(7);
   if(sync) *sync = true;
}

static k32 damage_mul;
static i32 max_health;

static void P_attrRGE(void) {
   if(pl.health < pl.old.health) {
      pl.rage += attr_rgebuff() * (pl.old.health - pl.health);
   }
   pl.rage *= 0.98k;
   damage_mul += pl.rage;
}

static void P_attrCON(void) {
   if(pl.mana > pl.old.mana) {
      pl.rage += attr_conbuff() * (pl.mana - pl.old.mana);
   }
   pl.rage *= 0.97k;
   damage_mul += pl.rage;
}

static void P_attrREF(void) {
   if(pl.health < pl.old.health) {
      pl.rage += attr_refbuff() * (pl.old.health - pl.health);
   }
   pl.rage *= 0.99k;
   pl.speedmul += (i32)pl.rage;
}

static void P_Spe_pTick(void) {
   if(Paused || !get_bit(wl.cbiupgr, cupg_d_shield)) {
      return;
   }

   if(pl.shield == 0 && pl.old.shield != 0) {
      AmbientSound(ss_player_ari_shield_break, 1.0k, lch_shield);
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
         AmbientSound(ss_player_ari_shield_regenw, 0.4k);
      }
   } else if(pl.shield < pl.shieldmax) {
      if(pl.shield == pl.old.shield) {
         AmbientSound(ss_player_ari_shield_regenl, 1.0k, lch_shield);
      }
      if(ACS_Timer() % 3 == 0) {
         pl.setShield(pl.shield + 1);
      }
      if(pl.shield == pl.shieldmax) {
         AmbientSound(ss_player_ari_shield_regend, 0.8k, lch_shield);
         SetFade(fid_shielddone, 3, 8);
      }
   }
}

static void P_Atr_pTick(void) {
   if(Paused) return;

   max_health = pl.spawnhealth + attr_strbuff();
   damage_mul = attr_accbuff();

   switch(pl.pclass) {
   case pcl_marine:    P_attrRGE(); break;
   case pcl_cybermage: P_attrCON(); break;
   case pcl_darklord:  P_attrREF(); break;
   }

   SetDamageMultiplier(0, damage_mul);
   SetMembI(0, sm_DmgFac, attr_defbuff());

   pl.maxhealth = max_health;
   SetSpawnHealth(0, pl.maxhealth);

   if(pl.health < attr_stmbuff() + 10 && ACS_Timer() % attr_stmtime() == 0) {
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
         P_LogH(1, tmpstr(lang(strp(_l(LANG "LOG_AutoBuy"), _p(i + 1)))), total, total != 1 ? "s" : "");
      }
   }
}

alloc_aut(0) script_str ext("ACS") addr(OBJ "Markiplier") void Z_MapMarker(void) {
   if(get_msk(ml.flag, _mflg_func) != _mfunc_normal) {
      return;
   }
   enum {
      t           = 35 * 5,
      t_pos       = 50,
      t_alpha     = 35 * 2,
      t_alpha_beg = t_alpha,
      t_alpha_end = t - t_alpha,
   };
   static struct i32v2 s;
   str text = GetMembS(0, sm_CurMarkStr);
   TextSize(&s, text, sf_areaname);
   ACS_Delay(5);
   for(i32 i = 0; i < t; i++) {
      k32 alpha;
      /**/ if(i < t_alpha_beg) alpha = i / (k32)t_alpha;
      else if(i > t_alpha_end) alpha = (t - i) / (k32)t_alpha;
      else                     alpha = 1.0;
      i32 x = 40 - (s.x - ease_in_out_sine(mink(i / (k32)t_pos, 1.0k)) * s.x);
      SetSize(640, 480);
      PrintText_str(text, sf_areaname, CR_WHITE, x,1, 80,0, _u_no_unicode|_u_alpha, alpha);
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

   P_LogH(1, tmpstr(lang(strp(_l(LANG "LOG_GroupBuy"), _p(fmt)))), cr, success, total, success != 1 ? "s" : "");
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
   P_LogH(1, tmpstr(lang(strp(_l(LANG "LOG_GroupToggle"), _p(fmt)))));
}

alloc_aut(0) script_str type("net") ext("ACS") addr(OBJ "KeyGlare") void Z_KeyGlare(void) {
   ACS_FadeTo(255, 255, 255, 1.0, 0.0);

   AmbientSound(ss_player_glare, 1.0);
   ACS_LineAttack(0, pl.yaw, pl.pitch, 1, so_BasicPuff, st_none,
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
      PrintText_str(sl_bad_timeline, sf_bigupper, CR_WHITE, 160,4, 120,0);
      pl.setHealth(-1);
      ACS_Delay(1);
   }
}

alloc_aut(0) script_str ext("ACS") addr(OBJ "SetLane") void Z_SetLane(void) {
   SetFun(GetFun() | lfun_lane);
}

/* EOF */
