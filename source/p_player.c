/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Player entry points.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "m_list.h"
#include "p_hudid.h"
#include "w_world.h"
#include "d_vm.h"

#include <limits.h>

/* Extern Objects ---------------------------------------------------------- */

noinit struct player players[_max_players];

/* Static Functions -------------------------------------------------------- */

script static void P_bossWarning(struct player *p);
script static void P_bossText(struct player *p, i32 boss);
script static void P_doIntro(struct player *p);

/* Scripts ----------------------------------------------------------------- */

script type("enter")
static void Sc_PlayerEntry(void) {
   script extern void P_Wep_PTickPre(struct player *p);
          extern void P_Dat_PTickPre(struct player *p);
          static void P_Scr_PTickPre(struct player *p);
   script extern void P_CBI_PTick   (struct player *p);
   script extern void P_Inv_PTick   (struct player *p);
   script extern void P_Log_PTick   (struct player *p);
   script extern void P_Upg_PTick   (struct player *p);
   script extern void P_Wep_PTick   (struct player *p);
          static void P_Atr_pTick   (struct player *p);
   script extern void P_Upg_PTickPst(struct player *p);
          extern void P_Ren_PTickPst(struct player *p);
          static void P_Aug_PTick   (struct player *p);

   if(ACS_GameType() == GAME_TITLE_MAP) return;

   struct player *p = LocalPlayer;

reinit:
   while(!player_init) ACS_Delay(1);

   P_Init(p);
   P_Log_Entry(p);
   P_Upg_Enter(p);
   P_Data_Load(p);

   P_doIntro(p);

   P_bossWarning(p);

   P_bossText(p, ServCallI(sm_GetBossLevel));

   if(p->teleportedout) P_TeleportInAsync(p);

   while(p->active)
   {
      if(p->reinit)
         goto reinit;

      P_Dat_PTickPre(p);

      /* Check for resurrect. */
      if(p->health > 0 && p->dead)
         p->reinit = true;

      /* These can be changed any time, so save them here. */
      struct player_delta olddelta = p->cur;
      i32 oldhealth = p->health;
      i32 oldmana   = p->mana;

      /* Tick all systems. */
      P_Wep_PTickPre(p); /* Update weapon info */
      P_Scr_PTickPre(p); /* Update score */

      if(!p->dead) P_Upg_PTick(p);
      P_Upg_PTickPst(p);

      if(!p->dead) {
         P_Inv_PTick(p);

         if(p->cbion) P_CBI_PTick(p);

         P_Aug_PTick(p);

         P_Atr_pTick(p);
         P_Wep_PTick(p);
         P_Log_PTick(p);

         P_Dat_PTickPst(p); /* Update engine info */
      }

      P_Ren_PTickPst(p);

      /* Update view (extra precision is required here to ensure accuracy) */
      ACS_SetActorPitch(0, ACS_GetActorPitch(0) - (f32)p->addpitch);
      ACS_SetActorAngle(0, ACS_GetActorAngle(0) - (f32)p->addyaw);
      ACS_SetActorRoll (0, ACS_GetActorRoll (0) - (f32)p->addroll);

      /* Tic passes */
      ACS_Delay(1);

      if(p->dlg.num) {
         Dlg_Run(p, p->dlg.num);
         p->dlg.num = 0;
      }

      /* Update previous-tic values */
      p->old       = olddelta;
      p->oldhealth = oldhealth;
      p->oldmana   = oldmana;

      /* Reset view for next tic */
      ACS_SetActorPitch(0, ACS_GetActorPitch(0) + (f32)p->addpitch);
      ACS_SetActorAngle(0, ACS_GetActorAngle(0) + (f32)p->addyaw);
      ACS_SetActorRoll (0, ACS_GetActorRoll (0) + (f32)p->addroll);

      /* If the map changes this we need to make sure it's still correct. */
      P_ValidateTID(p);

      p->ticks++;
   }
}

script type("death")
static void Sc_PlayerDeath(void) {
   i32 fun = GetFun();

   if(fun & lfun_final) SetFun(fun & ~lfun_final);

   struct player *p = LocalPlayer;

   p->dead = true;

   P_Upg_PDeinit(p);

   /* unfortunately, we can't keep anything even when we want to */
   P_Inv_PQuit(p);

   Str(sv_cooploseinventory, s"sv_cooploseinventory");
   if(singleplayer || ACS_GetCVar(sv_cooploseinventory)) {
      P_Upg_PQuit(p);
      P_BIP_PQuit(p);
      p->score = p->scoreaccum = p->scoreaccumtime = 0;
   }

   if(singleplayer) {
      if(ACS_GetCVar(sc_sv_revenge)) {
         ACS_LocalAmbientSound(ss_player_death1, 127);
         ACS_Delay(35);
         ServCallI(sm_PlayerDeath);
         ACS_Delay(25);
         ServCallI(sm_PlayerDeathNuke);
         ACS_Delay(25);
      }

      while(p->dead) {
         ACS_Delay(35 * 5);
         ACS_BeginPrint();
         ACS_PrintString(Language(LANG "DEATHMSG_%.2i", ACS_Random(1, 20)));
         ACS_EndLog();
      }
   }
}

script type("respawn")
static void Sc_PlayerRespawn(void) {
   LocalPlayer->reinit = true;
}

script type("return")
static void Sc_PlayerReturn(void) {
   LocalPlayer->reinit = true;
}

script type("disconnect")
static void Sc_PlayerDisconnect(void) {
   struct player *p = LocalPlayer;

   P_BIP_PQuit(p);

   ListDtor(&p->hudstrlist, true);

   fastmemset(p, 0, sizeof *p);
}

/* Extern Functions -------------------------------------------------------- */

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
   return "Mod";
}

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

struct player *P_PtrFind(i32 tid, i32 ptr) {
   i32 pnum = PtrPlayerNumber(tid, ptr);
   if(pnum >= 0) return &players[pnum];
   else          return nil;
}

void P_GUI_Close(struct player *p) {
   Str(guisnd_close, s"player/cbi/close");

   if(p->cbion) {
      if(singleplayer) UnfreezeTime();
      ACS_LocalAmbientSound(guisnd_close, 127);
      p->cbion = false;
   }
}

void P_GUI_Use(struct player *p) {
   Str(guisnd_open,  s"player/cbi/open");
   Str(guisnd_win95, s"player/cbi/win95");

   if(p->dead) return;

   if(!p->cbion) {
      if(singleplayer) FreezeTime();
      if(ACS_Random(0, 10000) == 777) {
         ACS_LocalAmbientSound(guisnd_win95, 127);
      } else {
         ACS_LocalAmbientSound(guisnd_open, 127);
      }
      p->cbion = true;
   } else {
      P_GUI_Close(p);
   }
}

i96 P_Scr_Give(struct player *p, i96 score, bool nomul) {
   /* Could cause division by zero */
   if(score == 0)
      return 0;

   if(!nomul) {
      score *= p->scoremul;
      score *= 1 + (k64)ACS_RandomFixed(0, p->attr.attrs[at_luk] / 77.7);
      score *= scoremul;
   }

   /* Get a multiplier for the score accumulator and sound volume */
   k64 mul = minmax(score, 0, 15000) / 15000.0lk;
       mul = minmax(mul, 0.1lk, 1.0lk);
   k32 vol = 0.7lk * mul;

   /* Play a sound when we pick up score */
   if(vol > 0.001k && p->getCVarI(sc_player_scoresound))
      StartSound(ss_player_score, lch_item2, 0, vol, ATTN_STATIC);

   /* hue */
   if(get_bit(p->upgrades[UPGR_CyberLegs].flags, _ug_active) && ACS_Random(0, 10000) == 0) {
      p->brouzouf += score;
      p->logB(1, "You gained brouzouf.");
   }

   if(get_bit(p->upgrades[UPGR_TorgueMode].flags, _ug_active) && ACS_Random(0, 10) == 0) {
      p->spuriousexplosions++;
      ACS_SpawnForced(so_EXPLOOOSION, p->x, p->y, p->z);
   }

   /* Add score and set score accumulator */
   p->score          += score;
   p->scoresum       += score;
   p->scoreaccum     += score;
   p->scoreaccumtime += 20 * (mul * 2.0lk);

   /* Log score */
   if(p->getCVarI(sc_player_scorelog))
      p->logH(1, "+\Cj%lli\Cnscr", score);

   return score;
}

void P_Scr_Take(struct player *p, i96 score) {
   if(p->score - score >= 0) {
      p->scoreused += score;
      p->score     -= score;
   } else {
      p->scoreused += p->score;
      p->score      = 0;
   }

   p->scoreaccum     = 0;
   p->scoreaccumtime = 0;
}

script void P_GiveAllScore(i96 score, bool nomul) {
   for_player() {
      p->setActivator();
      P_Scr_Give(p, score, nomul);
   }
}

script void P_GiveAllEXP(u64 amt) {
   for_player() {
      p->setActivator();
      P_Lv_GiveEXP(p, amt);
   }
}

/* Static Functions -------------------------------------------------------- */

script static void P_bossWarning(struct player *p) {
   ACS_Delay(35 * 5);

   if(bossspawned)
      p->logB(1, LanguageC(LANG "LOG_BossWarn%s", p->discrim));
}

script static void P_bossText(struct player *p, i32 boss) {
   if(boss == boss_iconofsin && ServCallI(sm_IsRampancy)) {
      return;
   }

   bool division = boss == boss_iconofsin && GetFun() & lfun_final;

   if(division) p->logB(1, LC(LANG "LOG_BossHear3"));

   if(!p->getCVarI(sc_player_bosstexts)) return;

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

   p->logB(1, LC(LANG "LOG_BossHear1"));
   p->logB(1, LC(LANG "LOG_BossHear2"));

   ACS_Delay(35 * 4);

   WaitPause();

   i32 t = 35 * 5 * (division ? 8 : 10);
   char text[1024];
   for(i32 i = 0, j = 1; i < t; i++) {
      if(i % (35 * 5) == 0) {
         k32 di = ACS_RandomFixed(0.1, 0.2);
         k32 fa = ACS_RandomFixed(0.1, 0.6);
         k32 ft = ACS_RandomFixed(0.1, 1.0);
         k32 ya = ACS_RandomFixed(0.0, 0.6);
         k32 pt = ACS_RandomFixed(0.2, 1.1);
         k32 ys = ACS_Sin(ya), yc = ACS_Cos(ya);

         p->bobyaw   += ys * di;
         p->bobpitch += yc * di;

         ACS_FadeTo(255, 0, 0, fa, 0.0);
         ACS_FadeTo(255, 0, 0, 0.0, ft);

         StartSound(ss_enemies_boss_talk, lch_voice2, CHANF_LOCAL, 1.0, 1.0, pt);

         SetFade(fid_bosstext, 20, 1);
         LanguageCV(text, fmt, j, p->discrim);
         j++;
      }

      SetSize(640, 400);
      SetClipW(0, 0, 640, 400, 640);
      PrintTextChS(text);
      PrintTextF(s_bigupper, CR_WHITE, 320,4, 100,0, fid_bosstext);
      ClearClip();

      PausableTick();
   }
}

script static
void P_doIntro(struct player *p) {
   enum {
      _nlines   = 26,
      _out_tics = 35 * 2,
   };

   Str(use,       s"+use");
   Str(attack,    s"+attack");
   Str(startgame, s"player/startgame");
   Str(showtext,  s"player/showtext");

   if(mapscleared != 0 || p->done_intro & p->pclass) return;

   p->doing_intro = true;
   ACS_SetMusic(sp_lsounds_Silence);
   FreezeTime(false);
   ACS_FadeTo(0, 0, 0, 1.0, 0.0);

   char *text = Malloc(8192);

   char *lines[_nlines];
   u32   linec[_nlines];
   u32   linen[_nlines];

   u32 which = 1;
   u32 last  = 0;
   u32 fill  = 0;

   for(;;) {
      SetSize(320, 240);
      if(which != last) {
         last = which;

         ifauto(str, texts,
                LanguageNull(LANG "BEGINNING_%s_%u", p->discrim, which)) {
            lstrcpy_str(text, texts);
         } else {
            break;
         }

         ACS_LocalAmbientSound(showtext, 127);

         char *line = strtok(text, "\n");
         for(i32 i = 0; i < _nlines; i++) {
            lines[i] = nil;
            linec[i] = 0;
            linen[i] = 0;

            if(text) {
               if(*text) {
                  lines[i] = line;
                  linec[i] = strlen(line);
               }

               line = strtok(nil, "\n");
            }
         }
      }

      PrintTextFmt(LC(LANG "SKIP_INTRO"), use, attack);
      PrintText(s_smallfnt, CR_WHITE, 275,6, 220,0);

      if(G_Filler(280, 220, &fill, 70, p->buttons & (BT_USE | BT_ATTACK))) {
         if(p->buttons & BT_ATTACK) {
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
            PrintText(s_smallfnt, p->color, 0,1, 8 * i,1);
         }
      }

      ACS_Delay(1);
   }

   UnfreezeTime(false);

   ACS_LocalAmbientSound(startgame, 127);
   ACS_FadeTo(0, 0, 0, 0.0, 2.0);

   for(i32 j = 0; j < _out_tics; j++) {
      k32 alpha = (_out_tics - j) / (k32)_out_tics;

      SetSize(320, 240);

      for(i32 i = 0; i < _nlines; i++) {
         if(!lines[i]) continue;

         if(lines[i][0] != '~') {
            PrintTextChr(lines[i], linen[i]);
            PrintTextA(s_smallfnt, p->color, 0,1, 8 * i,1, alpha);
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

   Dalloc(text);

   ACS_SetMusic(sp_star);

   p->done_intro |= p->pclass;
   P_Data_Save(p);

   p->doing_intro = false;
}

static void P_attrRGE(struct player *p) {
   i32 rge = p->attr.attrs[at_spc];

   if(p->health < p->oldhealth)
      p->rage += rge * (p->oldhealth - p->health) / 1000.0;

   p->rage = lerpk(p->rage, 0, 0.02);
}

static void P_attrCON(struct player *p) {
   i32 rge = p->attr.attrs[at_spc];

   if(p->mana > p->oldmana)
      p->rage += rge * (p->mana - p->oldmana) / 1100.0;

   p->rage = lerpk(p->rage, 0, 0.03);
}

static void P_Atr_pTick(struct player *p) {
   if(Paused) return;

   k32  acc = p->attr.attrs[at_acc] / 150.0;
   k32  def = p->attr.attrs[at_def] / 170.0;
   i32 strn = p->attr.attrs[at_str];
   i32  stm = p->attr.attrs[at_stm];
   i32 stmt = 75 - stm;

   switch(p->pclass) {
      case pcl_marine:    P_attrRGE(p); break;
      case pcl_cybermage: P_attrCON(p); break;
   }

   p->maxhealth = p->spawnhealth + strn;
   SetPropK(0, APROP_DamageMultiplier, 1.0 + acc + p->rage);
   SetMembI(0, sm_DmgFac, minmax(100 * def, 0, 100));
   SetPropI(0, APROP_SpawnHealth, p->maxhealth);

   if(p->health < stm+10 && (stmt < 2 || p->ticks % stmt == 0))
      p->health = p->health + 1;
}

static void P_Scr_PTickPre(struct player *p) {
   if(!p->scoreaccumtime || p->score < p->old.score) {
      p->scoreaccum = 0;
      p->scoreaccumtime = 0;
   }

   /**/ if(p->scoreaccumtime > 0) p->scoreaccumtime--;
   else if(p->scoreaccumtime < 0) p->scoreaccumtime++;
}

static void P_Aug_PTick(struct player *p) {
   for(i32 i = 0; i < 4; i++) {
      i32 total = 0;

      for_upgrade(upgr) {
         if(get_bit(upgr->agroups, i) && get_bit(p->autobuy, i)) {
            if(P_Upg_Buy(p, upgr, true)) {
               total++;
               P_Upg_Toggle(p, upgr);
            }
         }
      }

      if(total) {
         Str(snd, s"player/cbi/auto/buy");
         ACS_LocalAmbientSound(snd, 127);
         p->logH(1, LanguageC(LANG "LOG_AutoBuy%i", i + 1), total, total != 1 ? "s" : "");
      }
   }
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "Markiplier")
void Sc_MapMarker(i32 tid) {
   enum {ticks = 35 * 2};

   str text = GetPropS(tid, APROP_NameTag);

   ACS_Delay(5);

   with_player(LocalPlayer) {
      for(i32 i = 0; i < ticks; i++) {
         k32 alpha;

         /**/ if(i < 15)         alpha = i / 15.0;
         else if(i > ticks - 15) alpha = (ticks - i) / 15.0;
         else                    alpha = 1.0;

         i32 x = absk(ACS_Sin(i / (k32)ticks / 4.0)) / 4.0 * 800.0;

         SetSize(640, 480);
         PrintTextAX_str(text, s_areaname, CR_WHITE, x,4, 80,0, alpha, ptf_no_utf);

         ACS_Delay(1);
      }
   }
}

script_str ext("ACS") addr(OBJ "SetAdviceMarker")
void Sc_SetAdviceMarker(i32 tid) {
   str text;

   if(tid) {
      text = GetPropS(tid, APROP_NameTag);

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
      text = ACS_EndStrParam();
   } else {
      text = snil;
   }

   ACS_Delay(5);

   with_player(LocalPlayer) {
      p->advice = text;
      SetFade(fid_advice, 35 * 5, 12);
   }
}

script ext("ACS") addr(lsc_drawplayericon)
void Sc_DrawPlayerIcon(i32 num, i32 x, i32 y) {
   with_player(&players[num]) {
      k32 a = absk((x - 160) / 90.0);
           if(a < 0.2) a = 0.2;
      else if(a > 1.0) a = 1.0;

      PrintTextFmt("%S <%i>\n", p->name, p->num);
      __nprintf(p->health <= 0 ? "Dead\n" : "%iHP\n", p->health);
      if(p->pclass & pcl_magicuser) __nprintf("%iMP\n", p->mana);
      PrintTextA(s_smallfnt, CR_WHITE, x-9,1, y-2,1, a);
   }
}

script ext("ACS") addr(lsc_drawdmgnum)
void Sc_DrawDmgNum(i32 which_alpha, i32 damage, i32 x, i32 y) {
   i32 which =  which_alpha & 0x003;
   i32 alpha = (which_alpha & 0x3FC) >> 2;

   i32 cr;
   str font;

   k32 a = alpha / 255.0k;

   switch(which) {
   case _dnum_smol: font = s_smallfnt; cr = CR_GREY;  break;
   case _dnum_norm: font = s_lmidfont; cr = CR_WHITE; break;
   case _dnum_crit: font = s_bigupper; cr = CR_GOLD;  break;
   }

   ACS_BeginPrint();
   ACS_PrintInt(damage);
   PrintTextA(font, cr, x,0, y,0, a);
}

script_str type("net") ext("ACS") addr(OBJ "KeyBuyAutoGroup")
void Sc_KeyBuyAutoGroup(i32 grp) {
   Str(snd_bought,  s"player/cbi/auto/buy");
   Str(snd_invalid, s"player/cbi/auto/invalid");

   if(grp < 0 || grp >= 4) {
      return;
   }

   with_player(LocalPlayer) {
      i32 total = 0, success = 0;

      for_upgrade(upgr) {
         if(!get_bit(upgr->flags, _ug_owned) && get_bit(upgr->agroups, grp)) {
            total++;

            if(P_Upg_Buy(p, upgr, true)) {
               success++;
               P_Upg_Toggle(p, upgr);
            }
         }
      }

      char cr;
      str  snd;

      /**/ if(success ==     0) {cr = 'g'; snd = snd_invalid;}
      else if(success != total) {cr = 'j'; snd = snd_bought;}
      else                      {cr = 'q'; snd = snd_bought;}

      ACS_LocalAmbientSound(snd, 127);

      i32 fmt = total ? grp + 1 : grp + 5;

      p->logH(1, LanguageC(LANG "LOG_GroupBuy%i", fmt), cr, success, total, success != 1 ? "s" : "");
   }
}

script_str type("net") ext("ACS") addr(OBJ "KeyToggleAutoGroup")
void Sc_KeyToggleAutoGroup(i32 grp) {
   Str(snd, s"player/cbi/auto/toggle");

   if(grp < 0 || grp >= 4) {
      return;
   }

   with_player(LocalPlayer) {
      i32 total = 0;

      for_upgrade(upgr) {
         if(get_bit(upgr->flags, _ug_owned) && get_bit(upgr->agroups, grp)) {
            total++;
            P_Upg_Toggle(p, upgr);
         }
      }

      if(total) ACS_LocalAmbientSound(snd, 127);

      i32 fmt = total ? grp + 1 : grp + 5;
      p->logH(1, LanguageC(LANG "LOG_GroupToggle%i", fmt));
   }
}

script_str type("net") ext("ACS") addr(OBJ "KeyGlare")
void Sc_KeyGlare(void) {
   with_player(LocalPlayer) {
      Str(snd, s"player/glare");

      ACS_FadeTo(255, 255, 255, 1.0, 0.0);

      ACS_LocalAmbientSound(snd, 127);
      ACS_LineAttack(0, p->yaw, p->pitch, 1, so_Dummy, s_None,
         32767.0, FHF_NORANDOMPUFFZ | FHF_NOIMPACTDECAL);

      ACS_Delay(14);

      ACS_FadeTo(255, 255, 255, 0.0, 0.2);

      ACS_Delay(19);
   }
}

script_str ext("ACS") addr(OBJ "TimelineInconsistent")
void Sc_TimelineInconsistent(void) {
   for(;;) {
      for_player() {
         Str(bad_timeline, sLANG "BADTIMELINE");
         p->setActivator();
         ACS_FadeTo(0, 0, 0, 1.0, 0.0);
         SetSize(320, 240);
         PrintText_str(L(bad_timeline), s_bigupper, CR_WHITE, 160,4, 120,0);
         p->health = -1;
      }
      ACS_Delay(1);
   }
}

/* EOF */
