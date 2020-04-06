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

/* Static Objects ---------------------------------------------------------- */

static struct {str on, off;} guisnd[gui_max - 1] = {
   {s"player/cbi/open", s"player/cbi/close"},
};

/* Static Functions -------------------------------------------------------- */

script static void P_BossWarning(struct player *p);
script static void P_BossText(struct player *p, i32 boss);

/* Scripts ----------------------------------------------------------------- */

script type("enter")
static void Sc_PlayerEntry(void)
{
   script  extern void P_Wep_PTickPre(struct player *p);
   stkcall extern void P_Dat_PTickPre(struct player *p);
   stkcall static void P_Scr_PTickPre(struct player *p);
   script  extern void P_CBI_PTick   (struct player *p);
   script  extern void P_Inv_PTick   (struct player *p);
   script  extern void P_Log_PTick   (struct player *p);
   script  extern void P_Upg_PTick   (struct player *p);
   script  extern void P_Wep_PTick   (struct player *p);
   stkcall static void P_Atr_PTick   (struct player *p);
   script  extern void P_Upg_PTickPst(struct player *p);
   stkcall extern void P_Ren_PTickPst(struct player *p);

   if(ACS_GameType() == GAME_TITLE_MAP) return;

   struct player *p = LocalPlayer;

reinit:
   while(!player_init) ACS_Delay(1);

   P_Init(p);
   P_Log_Entry(p);
   P_Upg_Enter(p);
   P_Data_Load(p);

   P_BossWarning(p);

   P_BossText(p, ServCallI(sm_GetBossLevel));

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

      if(!p->dead)
      {
         P_Inv_PTick(p);

         switch(p->activegui)
         case gui_cbi: P_CBI_PTick(p);

         P_Atr_PTick(p);
         P_Wep_PTick(p);
         P_Log_PTick(p);

         P_Dat_PTickPst(p); /* Update engine info */

         if(pauseinmenus) ServCallI(sm_PauseTick, p->num);
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
static void Sc_PlayerDeath(void)
{
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
         Log("%S", Language(LANG "DEATHMSG_%.2i", ACS_Random(1, 20)));
      }
   }
}

script type("respawn")
static void Sc_PlayerRespawn(void)
{
   LocalPlayer->reinit = true;
}

script type("return")
static void Sc_PlayerReturn(void)
{
   LocalPlayer->reinit = true;
}

script type("disconnect")
static void Sc_PlayerDisconnect(void)
{
   struct player *p = LocalPlayer;

   P_BIP_PQuit(p);

   ListDtor(&p->hudstrlist, true);

   fastmemset(p, 0, sizeof *p);
}

/* Extern Functions -------------------------------------------------------- */

stkcall
cstr P_Discrim(i32 pclass)
{
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

struct player *P_PtrFind(i32 tid, i32 ptr)
{
   i32 pnum = PtrPlayerNumber(tid, ptr);
   if(pnum >= 0) return &players[pnum];
   else          return nil;
}

stkcall
void P_GUI_Close(struct player *p)
{
   if(p->activegui != gui_none)
   {
      if(pauseinmenus)
      {
         ServCallI(sm_SetPaused, false);
         for_player() p->frozen--;
      }
      else
         p->frozen--;

      ACS_LocalAmbientSound(guisnd[p->activegui - 1].off, 127);
      p->activegui = gui_none;
   }
}

stkcall
void P_GUI_Use(struct player *p, i32 type)
{
   if(p->dead) return;
   if(p->activegui == gui_none)
   {
      if(pauseinmenus)
      {
         ServCallI(sm_SetPaused, true);
         for_player() p->frozen++;
      }
      else
         p->frozen++;

      if(ACS_Random(0, 10000) == 777)
         ACS_LocalAmbientSound(ss_player_cbi_win95, 127);
      else
         ACS_LocalAmbientSound(guisnd[type - 1].on, 127);
      p->activegui = type;
   }
   else if(p->activegui == type)
      P_GUI_Close(p);
   else
   {
      ACS_LocalAmbientSound(guisnd[p->activegui - 1].off, 127);
      ACS_LocalAmbientSound(guisnd[type - 1].on, 127);
      p->activegui = type;
   }
}

i96 P_Scr_Give(struct player *p, i96 score, bool nomul)
{
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
   if(p->upgrades[UPGR_CyberLegs].active && ACS_Random(0, 10000) == 0) {
      p->brouzouf += score;
      p->logB(1, "You gained brouzouf.");
   }

   if(p->upgrades[UPGR_TorgueMode].active && ACS_Random(0, 10) == 0) {
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

stkcall
void P_Scr_Take(struct player *p, i96 score)
{
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

script void P_GiveAllScore(i96 score, bool nomul)
{
   for_player() {
      p->setActivator();
      P_Scr_Give(p, score, nomul);
   }
}

script void P_GiveAllEXP(u64 amt)
{
   for_player() {
      p->setActivator();
      P_Lv_GiveEXP(p, amt);
   }
}

/* Static Functions -------------------------------------------------------- */

script
static void P_BossWarning(struct player *p)
{
   ACS_Delay(35 * 5);

   if(bossspawned)
      p->logB(1, LanguageC(LANG "LOG_BossWarn%s", p->discrim));
}

script
static void P_BossText(struct player *p, i32 boss)
{
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

stkcall
static void AttrRGE(struct player *p)
{
   i32 rge = p->attr.attrs[at_spc];

   if(p->health < p->oldhealth)
      p->rage += rge * (p->oldhealth - p->health) / 1000.0;

   p->rage = lerpk(p->rage, 0, 0.02);
}

stkcall
static void AttrCON(struct player *p)
{
   i32 rge = p->attr.attrs[at_spc];

   if(p->mana > p->oldmana)
      p->rage += rge * (p->mana - p->oldmana) / 1100.0;

   p->rage = lerpk(p->rage, 0, 0.03);
}

stkcall
static void P_Atr_PTick(struct player *p)
{
   if(Paused) return;

   k32  acc = p->attr.attrs[at_acc] / 150.0;
   k32  def = p->attr.attrs[at_def] / 170.0;
   i32 strn = p->attr.attrs[at_str];
   i32  stm = p->attr.attrs[at_stm];
   i32 stmt = 75 - stm;

   switch(p->pclass) {
   case pcl_marine:    AttrRGE(p); break;
   case pcl_cybermage: AttrCON(p); break;
   }

   p->maxhealth = p->spawnhealth + strn;
   SetPropK(0, APROP_DamageMultiplier, 1.0 + acc + p->rage);
   SetMembI(0, sm_DmgFac, minmax(100 * def, 0, 100));
   SetPropI(0, APROP_SpawnHealth, p->maxhealth);

   if(p->health < stm+10 && (stmt < 2 || p->ticks % stmt == 0))
      p->health = p->health + 1;
}

stkcall
static void P_Scr_PTickPre(struct player *p)
{
   if(!p->scoreaccumtime || p->score < p->old.score)
   {
      p->scoreaccum = 0;
      p->scoreaccumtime = 0;
   }

        if(p->scoreaccumtime > 0) p->scoreaccumtime--;
   else if(p->scoreaccumtime < 0) p->scoreaccumtime++;
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr("Lith_Markiplier")
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

script_str ext("ACS") addr("Lith_SetAdviceMarker")
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

script_str type("net") ext("ACS") addr("Lith_KeyGlare")
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

script_str ext("ACS") addr("Lith_TimelineInconsistent")
void Sc_TimelineInconsistent(void) {
   for(;;) {
      for_player() {
         p->setActivator();
         ACS_FadeTo(0, 0, 0, 1.0, 0.0);
         SetSize(320, 240);
         PrintTextChS("This timeline is irreparable,\n"
                      "and cannot continue.\n"
                      "You must start anew.");
         PrintText(s_bigupper, CR_WHITE, 160,4, 120,0);
         p->health = -1;
      }
      ACS_Delay(1);
   }
}

/* EOF */
