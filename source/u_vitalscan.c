/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * VitalScan upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"
#include "w_world.h"
#include "w_monster.h"

#include <math.h>

#define UData p->upgrdata.vitalscan

/* Extern Functions -------------------------------------------------------- */

script void Upgr_VitalScan_Update(struct player *p, struct upgrade *upgr) {
   Str(devile_six,  s"RLDeVileSix");
   Str(countkill_s, s"COUNTKILL");

   ACS_SetActivator(0, AAPTR_PLAYER_GETTARGET);

   bool six = ACS_StrCmp(ACS_GetActorClass(0), devile_six, 11) == 0;

   bool validtarget =
      six ||
      ACS_CheckFlag(0, countkill_s) ||
      ACS_PlayerNumber() != -1;

   if(GetPropI(0, APROP_Health) <= 0) {
      UData = (struct upgr_data_VitalScan){};
   } else if(validtarget) {
      Str(boss_s,     s"BOSS");
      Str(invuln_s,   s"INVULNERABLE");
      Str(nodamage_s, s"NODAMAGE");
      Str(shadow_s,   s"SHADOW");

      Str(legendary_monster_token,       s"LDLegendaryMonsterToken");
      Str(legendary_monster_transformed, s"LDLegendaryMonsterTransformed");

      bool legendary = legendoom && InvNum(legendary_monster_token);
      bool henshin   = legendoom && InvNum(legendary_monster_transformed);

      bool phantom = InvNum(so_IsPhantom);

      bool freak  = ACS_CheckFlag(0, invuln_s) || ACS_CheckFlag(0, nodamage_s);
      bool boss   = ACS_CheckFlag(0, boss_s);
      bool shadow = ACS_CheckFlag(0, shadow_s);

      i32 chp = GetPropI(0, APROP_Health);
      i32 shp = GetPropI(0, APROP_SpawnHealth);

      i32 id = UniqueID();
      dmon_t const *const m = DmonSelf();

      i32 ot = UData.target;

      if(UData.target != id) {
         fastmemset(&UData, 0, sizeof UData);
         UData.oldhealth = UData.health;
      }

      UData.oldtarget = ot;
      UData.target    = id;

      bool healthset = false;

      if((freak || boss) && !phantom) {
         extern str RandomName(i32 id);

         UData.tagstr = RandomName(freak ? 0 : id);

         if(p->getCVarI(sc_scanner_bar)) {
            UData.oldhealth = UData.health = ACS_Random(0, 666666);
            UData.maxhealth = ACS_Random(0, 666666);
            healthset = true;
         }
      } else {
         Str(six_tag, s"\Cg6");

         /**/ if(six)     UData.tagstr = six_tag;
         else if(henshin) UData.tagstr = StrParam("\CgLegendary\C- %tS\C-", 0);
         else if(phantom) UData.tagstr = StrParam("\Cg%tS\C-", 0);
         else             UData.tagstr = StrParam("%tS\C-", 0);
      }

      if(!healthset) {
         UData.oldhealth = UData.health;
         UData.health    = chp;
         UData.maxhealth = m ? m->maxhealth : shp;
      }

      if(m) {
         i32 level    = shadow ? m->level - ACS_Random(-5, 5) : m->level;
         UData.tagstr = StrParam("%S lv.%i", UData.tagstr, level);
         UData.rank   = m->rank;
         UData.exp    = m->exp / (k32)_monster_level_exp;
      } else {
         UData.rank = 0;
         UData.exp  = 1.0;
      }

      i32 splitr = chp % shp;
      i32 split  = chp / shp;
      UData.splitfrac = splitr / (k32)shp;
      UData.split     = minmax(split, 0, 13);

      UData.freak  = six || freak || phantom || boss;
      UData.cangle = ACS_VectorAngle(p->x - GetX(0), p->y - GetY(0)) * tau;

      /* Hit indicator */
      if(UData.hdtime != 0) UData.hdtime--;
      else                  UData.hdelta = 0;

      if(UData.health < UData.oldhealth) {
         UData.hdelta = UData.oldhealth - UData.health;
         UData.hdtime = 30;
      }
   }
}

void Upgr_VitalScan_Render(struct player *p, struct upgrade *upgr) {
   if(!p->hudenabled || !UData.target) return;

   if(UData.hdtime == 30) SetFade(fid_vscan, 10, 12);

   i32 ox = p->getCVarI(sc_scanner_xoffs);
   i32 oy = p->getCVarI(sc_scanner_yoffs);

   if(p->getCVarI(sc_scanner_slide)) {
      k64 diff = p->yawf - (k64)UData.cangle;
      k32 ds = ACS_Sin(diff / tau) * tau;
      k32 dc = ACS_Cos(diff / tau) * tau;
      UData.oangle = lerplk(UData.oangle, atan2f(ds, dc), 0.1);
      ox += UData.oangle * 64;
   }

   /* Rank */
   if(UData.rank) for(i32 i = 0; i < UData.rank; i++) {
      StrAry(rs, s":UI:Rank1", s":UI:Rank2", s":UI:Rank3", s":UI:Rank4",
                 s":UI:Rank5", s":UI:Rank6", s":UI:Rank7");
      i32 y = 216 + (i > 4 ? 8 : 0);
      PrintSprite(rs[i], 106+ox + i%5*6,1, y+oy,1);
   }

   /* Hit indicator */
   if(UData.hdelta && CheckFade(fid_vscan)) {
      PrintTextFmt("-%i", UData.hdelta);
      PrintTextF(s_smallfnt, CR_RED, 160+ox,4, 235+oy,2, fid_vscan);
   }

   /* Tag and health */
   bool afnt = p->getCVarI(sc_scanner_altfont);
   str  font = afnt ? s_lmidfont : s_smallfnt;

   int cr = Draw_GetCr(p->getCVarI(sc_scanner_color));

   PrintText_str(UData.tagstr, font, cr, 160+ox,4, 216+oy,2);

   if(UData.maxhealth) PrintTextFmt("%u/%u", UData.health, UData.maxhealth);
   else                PrintTextFmt("%uhp",  UData.health);
   PrintTextX(UData.freak ? s_alienfont : font, CR_WHITE, 160+ox,4, 225+oy,2, ptf_no_utf);

   /* Health bar */
   if(p->getCVarI(sc_scanner_bar)) {
      StrAry(bs, s":UI:HealthBar1",  s":UI:HealthBar2",  s":UI:HealthBar3",
                 s":UI:HealthBar4",  s":UI:HealthBar5",  s":UI:HealthBar6",
                 s":UI:HealthBar7",  s":UI:HealthBar8",  s":UI:HealthBar9",
                 s":UI:HealthBar10", s":UI:HealthBar11", s":UI:HealthBar12",
                 s":UI:HealthBar13", s":UI:HealthBar14");

      i32 x = 120 + ox;
      i32 y = (afnt ? 201 : 205) + oy;

      if(UData.split > 0)
         PrintSprite(bs[UData.split - 1], x,1, y,1);

      SetClip(x, y, 80 * UData.splitfrac, 2);
      PrintSprite(bs[UData.split], x,1, y,1);
      ClearClip();

      Str(exp_bar_1, s":UI:ExpBar1");
      SetClip(x, y+3, 24 * UData.exp, 2);
      PrintSprite(exp_bar_1, x,1, y+3,1);
      ClearClip();
   }
}

/* EOF */
