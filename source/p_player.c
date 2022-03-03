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

#include "common.h"
#include "p_player.h"
#include "m_list.h"
#include "p_hudid.h"
#include "w_world.h"
#include "d_vm.h"

#include <limits.h>

noinit struct player player;

script static
void P_bossWarning();

script static
void P_bossText(i32 boss);

script static
void P_doIntro();

_Noreturn dynam_aut script type("enter") static
void Sc_PlayerEntry(void) {
   static
   void P_Scr_PTickPre();

   static
   void P_Atr_pTick();

   static
   void P_Aug_PTick();

   if(ACS_GameType() == GAME_TITLE_MAP) return;

reinit:
   while(!player_init) ACS_Delay(1);

   P_Init();
   P_Log_Entry();
   P_Upg_Enter();
   P_Data_Load();

   P_doIntro();

   P_bossWarning();

   P_bossText(ServCallI(sm_GetBossLevel));

   if(pl.teleportedout) P_TeleportIn();

   while(pl.active) {
      if(pl.reinit)
         goto reinit;

      P_Dat_PTickPre();

      /* Check for resurrect. */
      if(pl.health > 0 && pl.dead)
         pl.reinit = true;

      /* These can be changed any time, so save them here. */
      struct player_delta olddelta = pl.cur;
      i32 oldhealth = pl.health;
      i32 oldmana   = pl.mana;

      /* Tick all systems. */
      P_Wep_PTickPre(); /* Update weapon info */
      P_Scr_PTickPre(); /* Update score */

      if(!pl.dead) P_Upg_PTick();
      P_Upg_PTickPst();

      if(!pl.dead) {
         P_Inv_PTick();

         if(pl.modal == _gui_cbi) P_CBI_PTick();

         P_Aug_PTick();

         P_Atr_pTick();
         P_Wep_PTick();
         P_Log_PTick();

         P_Dat_PTickPst(); /* Update engine info */
      }

      P_Ren_PTickPst();

      /* Tic passes */
      ACS_Delay(1);

      if(pl.dlg.num) {
         Dlg_Run(pl.dlg.num);
         pl.dlg.num = 0;
      }

      /* Update previous-tic values */
      pl.old       = olddelta;
      pl.oldhealth = oldhealth;
      pl.oldmana   = oldmana;

      /* If the map changes this we need to make sure it's still correct. */
      P_ValidateTID();

      pl.ticks++;
   }
}

alloc_aut(0) script static
void revenge(void) {
   if(CVarGetI(sc_sv_revenge)) {
      AmbientSound(ss_player_death1, 1.0);
      ACS_Delay(35);
      ServCallI(sm_PlayerDeath);
      ACS_Delay(25);
      ServCallI(sm_PlayerDeathNuke);
      ACS_Delay(25);
   }
}

dynam_aut script type("death") static
void Sc_PlayerDeath(void) {
   i32 fun = GetFun();

   if(fun & lfun_final) SetFun(fun & ~lfun_final);

   pl.dead = true;

   P_Upg_PDeinit();

   /* unfortunately, we can't keep anything even when we want to */
   P_Inv_PQuit();
   P_Upg_PQuit();
   P_BIP_PQuit();
   pl.score = pl.scoreaccum = pl.scoreaccumtime = 0;

   revenge();

   str deathmsg = snil;
   for(i32 time = 0; pl.dead; ++time) {
      if(time != 0 && time % (35 * 5) == 0) {
         deathmsg = ns(lang_fmt(LANG "DEATHMSG_%.2i", ACS_Random(1, 20)));
         SetFade(fid_deathmsg, 35 * 4, 24);
      }

      if(CheckFade(fid_deathmsg)) {
         PrintTextF_str(deathmsg, sf_lmidfont, CR_WHITE, 320/2,4, 0,1,
                        fid_deathmsg);
      }

      if(pl.obit) {
         SetSize(320, 240);
         SetClipW(0, 0, 320, 240, 320);
         ACS_BeginPrint();
         ACS_PrintChar('>');
         ACS_PrintChar(' ');
         PrintChrSt(pl.obit);
         ACS_PrintChar(' ');
         ACS_PrintChar('<');
         PrintText(sf_lmidfont, CR_WHITE, 320/2,4, 240,2);
         ClearClip();
      }

      ACS_Delay(1);

      EndDrawing();
   }

   pl.obit = nil;
}

script type("respawn") static
void Sc_PlayerRespawn(void) {
   pl.reinit = true;
}

script type("return") static
void Sc_PlayerReturn(void) {
   pl.reinit = true;
}

script type("disconnect") static
void Sc_PlayerDisconnect(void) {
   P_BIP_PQuit();

   fastmemset(&pl, 0, sizeof pl);
}

alloc_aut(0) stkcall
cstr P_Discrim(i32 pclass) {
   switch(pclass) {
   case pcl_marine:    return "Stan";
   case pcl_cybermage: return "Jem";
   case pcl_informant: return "Fulk";
   case pcl_wanderer:  return "Luke";
   case pcl_assassin:  return "Omi";
   case pcl_darklord:  return "Ari";
   case pcl_thoth:     return "Kiri";
   }
   return "";
}

alloc_aut(0) stkcall
i32 P_Color(i32 pclass) {
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

void P_GUI_Close() {
   if(pl.modal == _gui_cbi) {
      UnfreezeTime();

      AmbientSound(ss_player_cbi_close, 1.0);
      pl.modal = _gui_none;
   }
}

void P_GUI_Use() {
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

i96 P_Scr_Give(k32 x, k32 y, k32 z, i96 score, bool nomul) {
   bool seen;
   struct i32v2 vp = project(x, y, z, &seen);
   if(!seen) {
      vp.x = 320/2 + ACS_Random(-64, 64);
      vp.y = 240/2 + ACS_Random(-48, 48);
   }
   return P_Scr_GivePos(vp.x, vp.y, score, nomul);
}

i96 P_Scr_GivePos(i32 x, i32 y, i96 score, bool nomul) {
   /* Could cause division by zero */
   if(score == 0)
      return 0;

   if(!nomul) {
      score *= pl.scoremul;
      score *= 1 + (k64)ACS_RandomFixed(0, pl.attr.attrs[at_luk] / 77.7);
   }

   /* Get a multiplier for the score accumulator and sound volume */
   k64 mul = clamplk(score, 0, 15000) / 15000.0lk;
       mul = clamplk(mul, 0.1lk, 1.0lk);
   k32 vol = 0.7lk * mul;

   /* Play a sound when we pick up score */
   if(vol > 0.001k && CVarGetI(sc_player_scoresound))
      StartSound(ss_player_score, lch_item2, 0, vol, ATTN_STATIC);

   /* hue */
   if(get_bit(pl.upgrades[UPGR_CyberLegs].flags, _ug_active) && ACS_Random(0, 10000) == 0) {
      pl.brouzouf += score;
      pl.logB(1, "You gained brouzouf.");
   }

   if(get_bit(pl.upgrades[UPGR_TorgueMode].flags, _ug_active) && ACS_Random(0, 10) == 0) {
      pl.spuriousexplosions++;
      ACS_SpawnForced(so_EXPLOOOSION, pl.x, pl.y, pl.z);
   }

   /* Add score and set score accumulator */
   pl.score          += score;
   pl.scoresum       += score;
   pl.scoreaccum     += score;
   pl.scoreaccumtime += 20 * (mul * 2.0lk);

   /* display score */
   i32 scoredisp = CVarGetI(sc_player_scoredisp);
   if(scoredisp & _itm_disp_log) {
      pl.logH(1, "+\Cj%lli\Cnscr", score);
   }
   if(scoredisp & _itm_disp_pop) {
      DrawCallI(sm_AddScoreNum, x, y, StrParam("%lli\Cnscr", score));
   }

   return score;
}

void P_Scr_Take(i96 score) {
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

script static
void P_bossWarningDone() {
   if(bossspawned)
      pl.logB(1, tmpstr(lang_discrim(sl_log_bosswarn)));
}

alloc_aut(0) script static
void P_bossWarning() {
   ACS_Delay(35 * 5);
   P_bossWarningDone();
}

dynam_aut script static
void P_bossText(i32 boss) {
   if(boss == boss_iconofsin && ServCallI(sm_IsRampancy)) {
      return;
   }

   bool division = boss == boss_iconofsin && GetFun() & lfun_final;

   if(division) pl.logB(1, tmpstr(lang(sl_log_bosshear3)));

   if(!CVarGetI(sc_player_bosstexts)) return;

   cstr fmt;
   switch(boss) {
   case boss_none:
   case boss_other:
      return;
   case boss_barons:      fmt = LANG "BOSS_BAR_%i_%s"; break;
   case boss_cyberdemon:  fmt = LANG "BOSS_CYB_%i_%s"; break;
   case boss_spiderdemon: fmt = LANG "BOSS_SPI_%i_%s"; break;
   case boss_iconofsin:
      if(division) fmt = LANG "BOSS_DIV_%i";
      else         fmt = LANG "BOSS_IOS_%i_%s";
      break;
   }

   pl.logB(1, tmpstr(lang(sl_log_bosshear1)));
   pl.logB(1, tmpstr(lang(sl_log_bosshear2)));

   ACS_Delay(35 * 4);

   WaitPause();

   i32 t = 35 * 5 * (division ? 8 : 10);
   str text;
   for(i32 i = 0, j = 1; i < t; i++) {
      if(i % (35 * 5) == 0) {
         k32 di = ACS_RandomFixed(0.1, 0.2);
         k32 fa = ACS_RandomFixed(0.1, 0.6);
         k32 ft = ACS_RandomFixed(0.1, 1.0);
         k32 ya = ACS_RandomFixed(0.0, 0.6);
         k32 pt = ACS_RandomFixed(0.2, 1.1);
         k32 ys = ACS_Sin(ya), yc = ACS_Cos(ya);

         pl.bobyaw   += ys * di;
         pl.bobpitch += yc * di;

         ACS_FadeTo(255, 0, 0, fa, 0.0);
         ACS_FadeTo(255, 0, 0, 0.0, ft);

         StartSound(ss_enemies_boss_talk, lch_voice2, CHANF_LOCAL, 1.0, 1.0, pt);

         SetFade(fid_bosstext, 20, 1);
         text = ns(lang_fmt_discrim(fmt, j));
         j++;
      }

      SetSize(640, 400);
      SetClipW(0, 0, 640, 400, 640);
      PrintTextF_str(text, sf_bigupper, CR_WHITE, 320,4, 100,0, fid_bosstext);
      ClearClip();

      PausableTick();
   }
}

alloc_aut(0) stkcall static
i32 P_playerColor() {
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

alloc_aut(0) stkcall script static
void P_doDepthMeter() {
   if(!mapscleared) {
      return;
   }

   StartSound(ss_player_depthdown, lch_depth, 0, 1.0, ATTN_STATIC);

   str level_name = fast_strupper((ACS_BeginPrint(), ACS_PrintName(PRINTNAME_LEVELNAME), ACS_EndStrParam()));
   i32 prev_level = (mapscleared % 32) * 8;
   i32 next_level = (mapscleared + 1 % 32) * 8;
   i32 player_clr = P_playerColor() & 0xFFFFFF;

   for(i32 i = 0; i < 175; ++i) {
      i32 curr_level = lerpk(prev_level, next_level, i > 35 ? ease_in_out_back(clampk((i - 35) / 70.0k, 0.0k, 1.0k)) : 0.0k);
      k32 alpha_k = i > 140 ? 1.0 - ease_out_cubic((i - 140) / 35.0k) : 1.0;
      i32 alpha = (i32)(0xFF * alpha_k) << 24;

      SetSize(640, 480);
      PrintTextAX_str(level_name, sf_areaname, CR_WHITE, 640,2, 98,2, alpha_k, _u_no_unicode);
      PrintRect(640 - 200, 100, 200, 2, alpha);
      PrintTextAX_str(st_depth, sf_smallfnt, CR_WHITE, 640,2, 102,1, alpha_k, _u_no_unicode);
      PrintRect(640, 110, 2, 240, alpha);
      for(i32 j = 0; j < 16; ++j) {
         i32 w = j % 3 == 0 ? 24 : 8;
         PrintRect(640 - w, 110 + j * 16, w, 2, alpha);
      }
      PrintRect(640 - 24, 110 + curr_level, 24, 2, player_clr | alpha);
      ACS_Delay(1);
   }
}

alloc_aut(0) stkcall script static
void P_doIntro() {
   if(mapscleared != 0 || pl.done_intro & pl.pclass) {
      P_doDepthMeter();
      return;
   }

   enum {
      _nlines   = 26,
      _out_tics = 35 * 2,
   };

   noinit static
   char text[8192], *lines[_nlines];

   noinit static
   u32 linec[_nlines], linen[_nlines];

   pl.modal = _gui_intro;
   ACS_SetMusic(sp_lsnd_Silence);
   FreezeTime(false);
   ACS_FadeTo(0, 0, 0, 1.0, 0.0);

   register i32 which = 1;
   register i32 last  = 0;

   noinit static
   struct gui_fil fil;
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

static
void P_attrRGE() {
   i32 rge = pl.attr.attrs[at_spc];

   if(pl.health < pl.oldhealth)
      pl.rage += rge * (pl.oldhealth - pl.health) / 1000.0;

   pl.rage = lerpk(pl.rage, 0, 0.02);
}

static
void P_attrCON() {
   i32 rge = pl.attr.attrs[at_spc];

   if(pl.mana > pl.oldmana)
      pl.rage += rge * (pl.mana - pl.oldmana) / 1100.0;

   pl.rage = lerpk(pl.rage, 0, 0.03);
}

static
void P_Atr_pTick() {
   if(Paused) return;

   k32  acc = pl.attr.attrs[at_acc] / 150.0;
   k32  def = pl.attr.attrs[at_def] / 170.0;
   i32 strn = pl.attr.attrs[at_str];
   i32  stm = pl.attr.attrs[at_stm];
   i32 stmt = 75 - stm;

   switch(pl.pclass) {
   case pcl_marine:    P_attrRGE(); break;
   case pcl_cybermage: P_attrCON(); break;
   }

   pl.maxhealth = pl.spawnhealth + strn;
   SetDamageMultiplier(0, 1.0 + acc + pl.rage);
   SetMembI(0, sm_DmgFac, clampi(100 * def, 0, 100));
   SetSpawnHealth(0, pl.maxhealth);

   if(pl.health < stm+10 && (stmt < 2 || pl.ticks % stmt == 0))
      pl.health = pl.health + 1;
}

static
void P_Scr_PTickPre() {
   if(!pl.scoreaccumtime || pl.score < pl.old.score) {
      pl.scoreaccum = 0;
      pl.scoreaccumtime = 0;
   }

   /**/ if(pl.scoreaccumtime > 0) pl.scoreaccumtime--;
   else if(pl.scoreaccumtime < 0) pl.scoreaccumtime++;
}

static
void P_Aug_PTick() {
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
         pl.logH(1, tmpstr(lang_fmt(LANG "LOG_AutoBuy%i", i + 1)), total, total != 1 ? "s" : "");
      }
   }
}

alloc_aut(0) script_str ext("ACS") addr(OBJ "Markiplier")
void Sc_MapMarker(i32 tid) {
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

alloc_aut(0) stkcall static
str GetAdviceMarker(i32 tid) {
   if(tid) {
      str text = GetNameTag(tid);

      ACS_BeginPrint();
      for(i32 i = 0, n = ACS_StrLen(text); i < n; i++) {
         if(text[i] == '{') {
            ACS_BeginPrint();
            for(i32 j = 0; j < 15 && text[++i] != '}'; j++)
               ACS_PrintChar(text[i]);
            ACS_PrintBind(ACS_EndStrParam());
         } else {
            ACS_PrintChar(text[i]);
         }
      }
      return ACS_EndStrParam();
   } else {
      return snil;
   }
}

alloc_aut(0) script_str ext("ACS") addr(OBJ "SetAdviceMarker")
void Sc_SetAdviceMarker(i32 tid) {
   str text = GetAdviceMarker(tid);

   ACS_Delay(5);

   pl.advice = text;
   SetFade(fid_advice, 35 * 5, 12);
}

script_str type("net") ext("ACS") addr(OBJ "KeyBuyAutoGroup")
void Sc_KeyBuyAutoGroup(i32 grp) {
   if(grp < 0 || grp >= 4) {
      return;
   }

   if(!P_None()) {
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

      pl.logH(1, tmpstr(lang_fmt(LANG "LOG_GroupBuy%i", fmt)), cr, success, total, success != 1 ? "s" : "");
   }
}

script_str type("net") ext("ACS") addr(OBJ "KeyToggleAutoGroup")
void Sc_KeyToggleAutoGroup(i32 grp) {
   if(grp < 0 || grp >= 4) {
      return;
   }

   if(!P_None()) {
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
      pl.logH(1, tmpstr(lang_fmt(LANG "LOG_GroupToggle%i", fmt)));
   }
}

alloc_aut(0) script_str type("net") ext("ACS") addr(OBJ "KeyGlare")
void Sc_KeyGlare(void) {
   if(!P_None()) {
      ACS_FadeTo(255, 255, 255, 1.0, 0.0);

      AmbientSound(ss_player_glare, 1.0);
      ACS_LineAttack(0, pl.yaw, pl.pitch, 1, so_Dummy, st_none,
         32767.0, FHF_NORANDOMPUFFZ | FHF_NOIMPACTDECAL);

      ACS_Delay(14);

      ACS_FadeTo(255, 255, 255, 0.0, 0.2);

      ACS_Delay(19);
   }
}

_Noreturn dynam_aut script_str ext("ACS") addr(OBJ "TimelineInconsistent")
void Sc_TimelineInconsistent(void) {
   pl.setActivator();
   for(;;) {
      ACS_FadeTo(0, 0, 0, 1.0, 0.0);
      SetSize(320, 240);
      PrintText_str(ns(lang(sl_bad_timeline)), sf_bigupper, CR_WHITE, 160,4, 120,0);
      pl.health = -1;
      ACS_Delay(1);
   }
}

/* EOF */
