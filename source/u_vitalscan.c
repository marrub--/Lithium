// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ VitalScan upgrade.                                                       │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"
#include "w_world.h"
#include "w_monster.h"

#include <math.h>

#define udata pl.upgrdata.vitalscan

Decl_Update void Upgr_VitalScan_Update(void) {
   ACS_SetActivator(0, AAPTR_PLAYER_GETTARGET);

   bool six = ACS_StrCmp(ACS_GetActorClass(0), so_RLDeVileSix, 11) == 0;

   dmon_t const *const m = DmonSelf();
   bool validtarget = m || six || ACS_CheckFlag(0, sm_countkill) || ACS_PlayerNumber() != -1;

   if(GetHealth(0) <= 0) {
      fastmemset(&udata, 0, sizeof udata);
   } else if(validtarget) {
      i32 chp = GetHealth(0);
      i32 shp = EDataI(_edt_spawnhealth);

      i32 id = UniqueID();

      i32 ot = udata.target;

      if(udata.target != id) {
         fastmemset(&udata, 0, sizeof udata);
         udata.oldhealth = udata.health;
      }

      udata.oldtarget = ot;
      udata.target    = id;

      if(m) {
         udata.rank      = m->rank;
         udata.exp       = m->exp / (k32)_monster_level_exp;
         udata.damagfrac = m->damagemul / 4.0;
      } else {
         udata.rank      = 0;
         udata.exp       = 1.0;
         udata.damagfrac = 0.0;
      }

      udata.freak = ACS_CheckFlag(0, sm_invuln) || ACS_CheckFlag(0, sm_nodamage);

      ACS_BeginPrint();
      if(udata.rank == 6) {
         PrintStrL("\C[Lith_Dark]");
      } else if(udata.rank == 7) {
         PrintStrL("\C[Lith_Angelic]");
      } else if(udata.rank == 8) {
         PrintStrL("\C[Lith_Evil]");
      }

      if(udata.rank < 6 && (udata.freak || ACS_CheckFlag(0, sm_boss))) {
         RandomName(!udata.freak &? id);
      } else if(six) {
         ACS_PrintString(st_six_tag);
      } else {
         ACS_PrintName(0);
      }
      ACS_PrintChar('\C');
      ACS_PrintChar('-');

      if(get_bit(wl.compat, _comp_legendoom) && InvNum(so_LDLegendaryMonsterTransformed)) {
         PrintStrL(" (\CgLegendary\C-)");
      }

      udata.oldhealth = udata.health;
      udata.health    = chp;
      udata.maxhealth = m ? m->maxhealth : shp;

      if(m) {
         i32 level =
            ACS_CheckFlag(0, sm_shadow) ?
            m->level - ACS_Random(-5, 5) :
            m->level;
         PrintStrL(" lv.");
         ACS_PrintInt(level);
      }

      udata.tagstr = ACS_EndStrParam();

      if(shp) {
         i32 splitr = chp % shp;
         i32 split  = chp / shp;
         udata.splitfrac = splitr / (k32)shp;
         udata.split     = split;
      } else {
         udata.splitfrac = 0.0;
         udata.split     = 0;
      }

      udata.x = GetX(0);
      udata.y = GetY(0);
      udata.z = GetZ(0);

      /* Hit indicator */
      if(udata.hdtime != 0) udata.hdtime--;
      else                  udata.hdelta = 0;

      if(udata.health < udata.oldhealth) {
         udata.hdelta = udata.oldhealth - udata.health;
         udata.hdtime = 30;
      }
   }
}

Decl_Render void Upgr_VitalScan_Render(void) {
   if(!pl.hudenabled || !udata.target) return;

   if(udata.hdtime == 30) SetFade(fid_vscan, 10, 12);

   i32 ox = CVarGetI(sc_scanner_xoffs);
   i32 oy = CVarGetI(sc_scanner_yoffs);

   i32 x;
   i32 y;

   switch(CVarGetI(sc_scanner_slide)) {
   case _ssld_slide: {
      k32 cangle = atan2k(pl.y - udata.y, pl.x - udata.x) * tau;
      k32 yawf   = pl.yaw * tau - pi;
      k32 diff = yawf - cangle;
      k32 ds = ACS_Sin(diff / tau) * tau;
      k32 dc = ACS_Cos(diff / tau) * tau;
      udata.oangle = lerplk(udata.oangle, atan2f(ds, dc), 0.1);
      ox += udata.oangle * 64;
      // fall through
   }
   default:
   case _ssld_fixed: {
      x = 120 + ox;
      y = 205 + oy;
      break;
   }
   case _ssld_under: {
      bool seen;
      struct i32v2 pos = project(udata.x, udata.y, udata.z, &seen);
      x = pos.x + ox - 39;
      y = pos.y + oy + 8;
      break;
   }
   }

   /* Rank */
   if(udata.rank) for(i32 i = 0; i < udata.rank; i++) {
      PrintSprite(sa_ranks[i], x - 14 + i%5*6,1, y + 11 + (i > 4 ? 8 : 0),1);
   }

   /* Hit indicator */
   if(udata.hdelta && CheckFade(fid_vscan)) {
      ACS_BeginPrint();
      ACS_PrintChar('-');
      ACS_PrintInt(udata.hdelta);
      PrintText(sf_smallfnt, CR_RED, x+40,4, y+30,2, _u_no_unicode|_u_fade, fid_vscan);
   }

   /* Tag and health */
   str font;
   switch(CVarGetI(sc_scanner_font)) {
   case _sfont_small:  font = sf_smallfnt; break;
   case _sfont_mid:    font = sf_lmidfont; break;
   case _sfont_italic: font = sf_litalics; break;
   }

   i32 cr = Draw_GetCr(CVarGetI(sc_scanner_color));

   PrintText_str(udata.tagstr, font, cr, x+40,4, y+11,2);

   ACS_BeginPrint();
   if(udata.maxhealth) {
      if(udata.freak) {
         PrintStr(alientext(udata.health));
         ACS_PrintChar('/');
         PrintStr(alientext(udata.maxhealth));
      } else {
         ACS_PrintInt(udata.health);
         ACS_PrintChar('/');
         ACS_PrintInt(udata.maxhealth);
      }
   } else {
      ACS_PrintInt(udata.health);
      ACS_PrintChar('h');
      ACS_PrintChar('p');
   }
   PrintText(font, CR_WHITE, x+40,4, y+20,2, _u_no_unicode);

   /* Health bar */
   if(CVarGetI(sc_scanner_bar)) {
      PrintSpriteClip(sp_Bars_DamageBar, x,1, y+2,1, 0,0,80*udata.damagfrac,2);
      if(udata.split > 0) {
         PrintSprite(sa_healthbars[(udata.split - 1) % countof(sa_healthbars)], x,1, y,1);
      }
      PrintSpriteClip(sa_healthbars[udata.split % countof(sa_healthbars)], x,1, y,1, 0,0,80*udata.splitfrac,2);
      PrintSpriteClip(sp_Bars_ExpBar1, x,1, y+3,1, 0,0,24*udata.exp,2);
   }
}

/* EOF */
