/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * HeadsUpDisp upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

#define udata pl.upgrdata.headsupdisp

static
i32 hudtype, hudcolor;

static
void HUD_Log() {
   i32 logcolor = CVarGetI(sc_hud_logcolor);
   if(logcolor == 'l') logcolor = hudtype != _hud_old ? hudcolor : CR_GREEN;
   else                logcolor = Draw_GetCr(logcolor);

   i32 logx = 0;
   i32 logy = 0;
   switch(hudtype) {
   case _hud_cybermage: logy = -5;  break;
   case _hud_assassin:  logy =  5;  break;
   case _hud_old:       logy =  30; break;
   }

   P_Log(logcolor, logx, logy);
}

static
void HUD_TopRight() {
   cstr fmt;
   str  fnt;
   i32  clr = hudcolor;

   if(hudtype != _hud_informant) {
      udata.score.value_display = pl.score;
   } else {
      udata.score.value = pl.score;
      lerplli(&udata.score);
   }

   switch(hudtype) {
   default:
   case _hud_marine:
      fmt = "%s\Cnscr";
      fnt = sf_smallfnt;
      clr = CR_WHITE;
      break;
   case _hud_cybermage:
      fmt = "%s \CnScr";
      fnt = sf_italic;
      break;
   case _hud_informant:
      fmt = "%s \CnScore";
      fnt = sf_lmidfont;
      break;
   case _hud_assassin:
      fmt = "%s \CjSCR";
      fnt = sf_lmidfont;
      break;
   case _hud_darklord:
      fmt = "%s \CjSCR";
      fnt = sf_lmidfont;
      break;
   }

   i32 y = 3;

   if(CVarGetI(sc_hud_showscore)) {
      char scr[64];
      sprintf(scr, fmt, scoresep(udata.score.value_display));

      PrintTextChS(scr);
      PrintTextX(fnt, clr, 320,2, y,1, _u_no_unicode);

      if(hudtype != _hud_cybermage && hudtype != _hud_old) {
         if(pl.score > pl.old.score) {
            SetFade(fid_schit1, 4, 12);
         } else if(pl.score < pl.old.score) {
            i96 scrdif = pl.old.score - pl.score;
            u32 tics = scrdif * 8 / 0xFFFF;
            u32 mmx = 30000 - minmax(scrdif / 4, 3276, 29490);
            SetFade(fid_schit2, minmax(tics, 1, 35), mmx >> 8);
         }

         if(CheckFade(fid_schit1)) {
            PrintTextChS(scr);
            PrintTextFX(fnt, CR_ORANGE, 320,2, 3,1, fid_schit1, _u_no_unicode);
         } else if(CheckFade(fid_schit2)) {
            PrintTextChS(scr);
            PrintTextFX(fnt, CR_PURPLE, 320,2, 3,1, fid_schit2, _u_no_unicode);
         }

         y += 7;

         if(pl.scoreaccumtime > 0) {
            SetFade(fid_scacum, 5, 12);
            pl.scoreaccumstr = StrParam("%c%s", pl.scoreaccum >= 0 ? '+' : ' ', scoresep(pl.scoreaccum));
         }

         if(CheckFade(fid_scacum))
            PrintTextFX_str(pl.scoreaccumstr, fnt, CR_WHITE, 320,2, y,1, fid_scacum, _u_no_unicode);
      }

      y += 7;
   }

   if(CVarGetI(sc_hud_showlvl)) {
      PrintTextFmt("Lv.%u", pl.attr.level);
      if(pl.attr.points) __nprintf(" (\Cn%u\C- pts)", pl.attr.points);
      PrintTextX(fnt, clr, 320,2, y,1, _u_no_unicode);
      y += 7;
   }

   i32 expbar = CVarGetI(sc_hud_expbar);
   if(expbar > 0) {
      PrintSprite(sp_Bars_ExpBar0, 320,2, y,1);
      u32 fr =
         ((u64)(pl.attr.exp     - pl.attr.expprev) * 24) /
          (u64)(pl.attr.expnext - pl.attr.expprev);
      SetClip(296, y, fr, 2);
      PrintSprite(StrParam(":Bars:ExpBar%i", expbar), 296,1, y,1);
      ClearClip();
      y += 3;
   }
}

static
void HUD_BottomRight() {
   struct invweapon const *wep = pl.weapon.cur;

   bool showweps = CVarGetI(sc_hud_showweapons);

   str fnt = sf_bigupper;
   i32 xmg;
   i32 ymg;
   i32 xam = 0;
   i32 yam = 0;
   i32 xal = 1;

   switch(hudtype) {
   default:
   case _hud_marine:
      xmg = 217;
      ymg = 232;
      xam = 64;
      if(showweps) {
         ymg -= 13;
      }
      if(wep->ammotype & AT_AMag) {
         PrintSprite(sp_HUD_M_BarBig, 278,2, ymg+7,2);
         PrintSprite(showweps ? sp_HUD_M_SplitRight : sp_HUD_M_SplitFront,
                     319,2, 239,2);
         switch(wep->ammotype & AT_AMag) {
         case AT_AMag: PrintSprite(sp_HUD_M_BarBig, 214,2, ymg+7,2);
         case AT_Ammo: PrintTextChS("AMMO"); break;
         case AT_NMag: PrintTextChS("MAG"); break;
         }
         PrintTextX(fnt, hudcolor, 281,1, 238,2, _u_no_unicode);
      }
      break;
   case _hud_cybermage:
      xmg = 242;
      ymg = 229;
      xam = 58;
      if(wep->ammotype & AT_AMag) {
         PrintSprite(sp_HUD_C_SplitFront, 320,2, 238,2);
         switch(wep->ammotype & AT_AMag) {
         case AT_AMag: PrintSprite(sp_HUD_C_Back, 240,2, 238,2);
         case AT_Ammo: PrintSprite(sp_HUD_C_AMMO, 316,2, 235,2); break;
         case AT_NMag: PrintSprite(sp_HUD_C_MAG,  316,2, 235,2); break;
         }
      }
      break;
   case _hud_informant:
      xmg = 243;
      ymg = 229;
      xam = 62;
      if(wep->ammotype & AT_AMag) {
         PrintSprite(sp_HUD_I_AmmoBack, 320,2, 239,2);
         switch(wep->ammotype & AT_AMag) {
         case AT_AMag: PrintSprite(sp_HUD_I_AmmoExtend, 240,2, 238,2);
                       PrintSprite(sp_HUD_I_AMAG, 316,2, 236,2); break;
         case AT_Ammo: PrintSprite(sp_HUD_I_AMMO, 316,2, 236,2); break;
         case AT_NMag: PrintSprite(sp_HUD_I_NMAG, 316,2, 236,2); break;
         }
      }
      break;
   case _hud_assassin:
      fnt = sf_lmidfont;
      xmg = 268;
      ymg = 228;
      xam = -2;
      yam = -7;
      if(wep->ammotype & AT_AMag) {
         PrintSprite(sp_HUD_A_RightBack, 320,2, 239,2);
      }
      break;
   case _hud_darklord:
      xmg = 232;
      ymg = 228;
      xam = 62;
      if(wep->ammotype & AT_AMag) {
         PrintSprite(sp_HUD_D_AmmoBack, 320,2, 239,2);
         switch(wep->ammotype & AT_AMag) {
         case AT_AMag: PrintSprite(sp_HUD_D_Ammo2Back, 230,2, 239,2);
         case AT_Ammo: PrintTextChS(u8"\n"); break;
         case AT_NMag: PrintTextChS(u8"\n"); break;
         }
         PrintTextX(sf_lmidfont, hudcolor, 309,4, 230,0, _u_no_unicode);
      }
      break;
   case _hud_old:
      xmg = 320;
      ymg = 234;
      xam = 77;
      xal = 2;
      if(wep->ammotype & AT_AMag) {
         PrintSprite(sp_HUD_H_B2, 320,2, 240,2);
         switch(wep->ammotype & AT_AMag) {
         case AT_AMag: PrintSprite(sp_HUD_H_B2, 243,2, 240,2);
         case AT_Ammo: PrintSprite(sp_HUD_H_A2, 320,2, 240,2); break;
         case AT_NMag: PrintSprite(sp_HUD_H_A1, 320,2, 240,2); break;
         }
      }
      break;
   }

   if(wep->ammotype & AT_NMag) {
      ACS_BeginPrint();

      if(wep->ammotype & AT_Ammo && !wep->ammocur) {
         ACS_PrintChar('O');
         ACS_PrintChar('U');
         ACS_PrintChar('T');
      } else {
         ACS_PrintInt(wep->magmax - wep->magcur);
         ACS_PrintChar('/');
         ACS_PrintInt(wep->magmax);
      }

      PrintTextX(fnt, hudcolor, xmg,xal, ymg,0, _u_no_unicode);
   }

   if(wep->ammotype & AT_Ammo) {
      i32 x = xmg - (wep->ammotype & AT_NMag ? xam : 0);
      i32 y = ymg - (wep->ammotype & AT_NMag ? yam : 0);

      ACS_BeginPrint();
      ACS_PrintInt(wep->ammocur);
      PrintTextX(fnt, hudcolor, x,xal, y,0, _u_no_unicode);
   }

   if(showweps) {
      i32 cr1, cr2, cr3, crc;
      i32 _x, _y;

      switch(hudtype) {
      default:
      case _hud_marine:
         PrintSprite(sp_HUD_M_Bar, 278,2, 239,2);
         cr1 = Cr(wselm1); cr2 = Cr(wselm2);
         cr3 = Cr(wselm3); crc = Cr(wselms);
         _x = 281; _y = 238;
         break;
      case _hud_cybermage:
         PrintSprite(sp_HUD_C_Bar, 320,2, 220,2);
         cr1 = Cr(wselc1); cr2 = Cr(wselc2);
         cr3 = Cr(wselc3); crc = Cr(wselcs);
         _x = 323; _y = 219;
         break;
      case _hud_informant:
         cr1 = Cr(wseli1); cr2 = Cr(wseli2);
         cr3 = Cr(wseli3); crc = Cr(wselis);
         _x = 323; _y = 219;
         break;
      case _hud_assassin:
         cr1 = Cr(wsela1); cr2 = Cr(wsela2);
         cr3 = Cr(wsela3); crc = Cr(wselas);
         _x = 323; _y = 222;
         break;
      case _hud_darklord:
         PrintSprite(sp_HUD_D_WepBack, 320,2, 218,2);
         cr1 = Cr(wseld1); cr2 = Cr(wseld2);
         cr3 = Cr(wseld3); crc = Cr(wselds);
         _x = 323; _y = 216;
         break;
      case _hud_old:
         PrintSprite(sp_HUD_H_W1, 78,1, 240,2);
         cr1 = CR_BLUE; cr2 = CR_BLUE;
         cr3 = CR_BLUE; crc = CR_GOLD;
         _x = 154; _y = 240;
         break;
      }

      for(i32 i = 1; i < SLOT_MAX; i++) ifauto(i32, slot, pl.weapon.slot[i]) {
         k32 x = _x+0.2 - 8 * (SLOT_MAX - i);
         k32 y = _y+0.2;
         i32 cr;
         switch(slot) {
         case 1:  cr = cr1; break;
         case 2:  cr = cr2; break;
         default: cr = cr3; break;
         }

         ACS_BeginPrint();
         ACS_PrintInt(i);
         PrintTextX(sf_lsmlhfnt, cr, x,2, y,2, _u_no_unicode);

         if(pl.weapon.cur->info->slot == i)
            SetFade(fid_slotnS + i, 1, 6);

         if(CheckFade(fid_slotnS + i)) {
            ACS_BeginPrint();
            ACS_PrintInt(i);
            PrintTextFX(sf_lsmlhfnt, crc, x,2, y,2, fid_slotnS + i, _u_no_unicode);
         }
      }
   }

   switch(P_Wep_CurType()) {
   case weapon_m_rifle:
      if(ServCallI(sm_GetRifleGrenade)) {
         PrintSprite(sp_HUD_H_D44, 280,1, 237,1);
      }
      break;
   case weapon_c_smg: {
      i32 y = showweps ? 0 : 14;
      k32 heat = InvNum(so_SMGHeat)/500.0k;
      PrintSprite(sp_HUD_C_BarSmall, 257,1, 196+y,1);
      SetClip(257, 196+y, 63 * heat, 9);
      PrintSprite(sp_HUD_C_HeatBar, 257,1, 196+y,1);
      ClearClip();
      break;
   }
   }
}

static
void HUD_BottomLeft() {
   i32 health = pl.health;
   i32 x;
   i32 y;
   i32 cr_g = 0;
   i32 cr_t;
   str wgfx = sa_wgfx[pl.weapon.cur->info->slot];

   switch(hudtype) {
   default:
   case _hud_marine:
      PrintSprite(InvNum(so_PowerStrength) ? sp_HUD_M_SplitBackRed : sp_HUD_M_SplitBack, 0,1, 240,2);

      PrintTextChr("VIT", 3);
      PrintTextX(sf_bigupper, hudcolor, 2,1, 238,2, _u_no_unicode);

      for(i32 i = 0, x_ = (8 + pl.ticks) % 57; i < 20; i++) {
         i32 xx = x_ - i;
         if(xx < 0) xx += 57;
         i32 y_ = 9;
         if(xx < 11) y_ += 11 - xx % 12;
         PrintSpriteA(wgfx, 88-xx,1, 215+y_,1, (20 - i) / 20.0);
      }

      x    = 34;
      y    = 232;
      cr_g = CR_YELLOW;
      cr_t = CR_PURPLE;
      break;
   case _hud_cybermage:
      PrintSprite(sp_HUD_C_SplitBack, 0,1, 239,2);
      PrintSprite(sp_HUD_C_VIT,       2,1, 237,2);

      x = 21;
      y = 229;
      break;
   case _hud_informant:
      PrintSprite(sp_HUD_I_HPBack, 0,1, 239,2);

      udata.health.value = pl.health;
      lerplli(&udata.health);

      health = udata.health.value_display;
      x      = 25;
      y      = 230;
      cr_g   = CR_WHITE;
      cr_t   = CR_RED;
      break;
   case _hud_assassin:
      PrintSprite(sp_HUD_A_LeftBack, 0,1, 239,2);

      x    = 4;
      y    = 231;
      cr_g = CR_RED;
      cr_t = CR_ICE;
      break;
   case _hud_darklord:
      PrintSprite(sp_HUD_D_HPBack, 0,1, 239,2);

      PrintTextChS(u8"");
      PrintTextX(sf_lmidfont, hudcolor, 8,4, 229,0, _u_no_unicode);

      for(i32 i = 0, x_ = (8 + pl.ticks) % 40; i < 20; i++) {
         i32 xx = x_ - i;
         if(xx < 0) xx += 40;
         PrintSpriteA(wgfx, 21+xx,1, 220,1, (20 - i) / 20.0);
      }

      x    = 18;
      y    = 228;
      cr_g = CR_CYAN;
      cr_t = CR_FIRE;
      break;
   case _hud_old:
      PrintSprite(InvNum(so_PowerStrength) ? sp_HUD_H_B4 : sp_HUD_H_B1, 0,1, 240,2);

      for(i32 i = 0, x_ = (8 + pl.ticks) % 77; i < 20; i++) {
         i32 xx = x_ - i;
         if(xx < 0) xx += 77;
         i32 y_ = 9;
         if(xx < 11) y_ += 11 - xx % 12;
         PrintSpriteA(wgfx, 77-xx,1, 218+y_,1, (20 - i) / 20.0);
      }

      x    = 0;
      y    = 234;
      cr_g = CR_YELLOW;
      cr_t = CR_PURPLE;
      break;
   }

   if(cr_g) {
      k32 ft = 0;

      if(pl.health < pl.oldhealth) {
         udata.cr = cr_t;
         ft = (pl.oldhealth - pl.health) / 300.0;
         ft = minmax(ft, 0.1, 1.0);
      } else if(pl.health > pl.oldhealth) {
         udata.cr = cr_g;
         ft = 0.1;
      }

      if(ft) SetFade(fid_health, 4, ft * 255);
   }

   i32 protect = pl.megaProtect;

   ACS_BeginPrint();
   ACS_PrintInt(health);
   PrintTextX(sf_bigupper, hudcolor, x,1, y,0, _u_no_unicode);

   if(protect) {
      k32 amt = protect / 200.0;
      ACS_BeginPrint();
      ACS_PrintInt(health);
      PrintTextAX(sf_bigupper, CR_TAN, x,1, y,0, amt, _u_no_unicode);
   }

   if(CheckFade(fid_health)) {
      ACS_BeginPrint();
      ACS_PrintInt(health);
      PrintTextFX(sf_bigupper, udata.cr, x,1, y,0, fid_health, _u_no_unicode);
   }
}

void Upgr_HeadsUpDisp_Activate(struct upgrade *upgr) {
   lerplli_init(&udata.score,     pl.score,     4);
   lerplli_init(&udata.health,    pl.health,    1);
   lerplli_init(&udata.overdrive, pl.overdrive, 1);
}

void Upgr_HeadsUpDisp_Render(struct upgrade *upgr) {
   if(!pl.hudenabled) return;

   hudtype = CVarGetI(sc_hud_type);
   if(hudtype <= 0 || hudtype > _hud_max)
      hudtype = pl.pclass_b;
   else
      --hudtype;

   hudcolor = hudtype != _hud_old ? P_Color(dst_bit(hudtype)) : CR_RED;

   HUD_Log();
   HUD_TopRight();
   HUD_BottomRight();
   HUD_BottomLeft();
}

/* EOF */
