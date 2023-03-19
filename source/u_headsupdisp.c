// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ HeadsUpDisp upgrade.                                                     │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"

#define udata pl.upgrdata.headsupdisp

static i32 hudtype, hudcolor;

static void HUD_Log(void) {
   i32 logcolor = CVarGetI(sc_hud_logcolor);
   if(logcolor == 'l') logcolor = hudtype != _hud_old ? hudcolor : CR_GREEN;
   else                logcolor = Draw_GetCr(logcolor);
   i32 logy = 0;
   switch(hudtype) {
   case _hud_cybermage: logy = -5;  break;
   case _hud_assassin:  logy =  5;  break;
   case _hud_old:       logy =  30; break;
   }
   P_Log(logcolor, logy);
}

static void HUD_TopRight(void) {
   cstr beg, end;
   str  fnt;
   i32  clr = hudcolor;
   if(hudtype != _hud_informant) {
      udata.score.value_display = pl.score;
   } else {
      udata.score.value = pl.score;
      lerpscr(&udata.score);
   }
   beg = scoresep(udata.score.value_display);
   switch(hudtype) {
   default:
   case _hud_marine:
      end = "\Cnscr";
      fnt = sf_smallfnt;
      clr = CR_WHITE;
      break;
   case _hud_cybermage:
      end = " \CnScr";
      fnt = sf_litalics;
      break;
   case _hud_informant:
      end = " \CnScore";
      fnt = sf_lmidfont;
      break;
   case _hud_assassin:
   case _hud_darklord:
      end = beg;
      beg = u8"\Cj\uE100\uE103\uE106\C-";
      fnt = sf_lmidfont;
      break;
   }
   i32 y = 3;
   if(CVarGetI(sc_hud_showscore)) {
      BeginPrintStr(beg);
      PrintStr(end);
      PrintText(fnt, clr, pl.hudrpos,2, y,1, _u_no_unicode);
      if(hudtype != _hud_cybermage && hudtype != _hud_old) {
         if(pl.score > pl.old.score) {
            SetFade(fid_schit1, 4, 12);
         } else if(pl.score < pl.old.score) {
            score_t scrdif = pl.old.score - pl.score;
            i32     tics   = scrdif * 8 / 0xFFFF;
            i32     mmx    = 30000 - clampi(scrdif / 4, 3276, 29490);
            SetFade(fid_schit2, clampi(tics, 1, 35), mmx >> 8);
         }
         if(CheckFade(fid_schit1)) {
            BeginPrintStr(beg);
            PrintStr(end);
            PrintText(fnt, CR_ORANGE, pl.hudrpos,2, 3,1, _u_no_unicode|_u_fade, fid_schit1);
         } else if(CheckFade(fid_schit2)) {
            BeginPrintStr(beg);
            PrintStr(end);
            PrintText(fnt, CR_PURPLE, pl.hudrpos,2, 3,1, _u_no_unicode|_u_fade, fid_schit2);
         }
         y += 7;
         static score_t scoreaccum_disp;
         if(pl.scoreaccumtime > 0) {
            SetFade(fid_scacum, 5, 12);
            scoreaccum_disp = pl.scoreaccum;
         }
         if(CheckFade(fid_scacum)) {
            ACS_BeginPrint();
            ACS_PrintChar(scoreaccum_disp >= 0 ? '+' : ' ');
            PrintStr(scoresep(scoreaccum_disp));
            PrintText(fnt, CR_WHITE, pl.hudrpos,2, y,1, _u_no_unicode|_u_fade, fid_scacum);
         }
      }
      y += 7;
   }
   if(CVarGetI(sc_hud_showlvl)) {
      BeginPrintStrL("Lv.");
      ACS_PrintInt(pl.attr.level);
      if(pl.attr.points) {
         PrintStrL(" (\Cn");
         ACS_PrintInt(pl.attr.points);
         switch(hudtype) {
         default:
         case _hud_marine:    PrintStrL("\C- pts)"); break;
         case _hud_cybermage: PrintStrL("\C- Pts)"); break;
         case _hud_informant: PrintStrL("\C- Points)"); break;
         }
      }
      PrintText(fnt, clr, pl.hudrpos,2, y,1, _u_no_unicode);
      y += 7;
   }
   i32 expbar = CVarGetI(sc_hud_expbar);
   if(expbar > 0) {
      PrintSprite(sp_Bars_ExpBar0, pl.hudrpos,2, y,1);
      i32 fr =
         ((pl.attr.exp     - pl.attr.expprev) * 24) /
          (pl.attr.expnext - pl.attr.expprev);
      ACS_BeginPrint();
      PrintStrL(":Bars:ExpBar");
      ACS_PrintInt(expbar);
      PrintSpriteClip(ACS_EndStrParam(), pl.hudrpos-24,1, y,1, 0,0,fr,2);
      y += 3;
   }
}

static void HUD_BottomRight(void) {
   struct invweapon const *wep = &pl.weapon.cur;
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
      xmg = pl.hudrpos-103;
      ymg = 232;
      xam = 64;
      if(showweps) {
         ymg -= 13;
      }
      if(wep->ammotype & AT_Disp) {
         PrintSprite(sp_HUD_M_BarBig, pl.hudrpos-42,2, ymg+7,2);
         PrintSprite(showweps ? sp_HUD_M_SplitRight : sp_HUD_M_SplitFront,
                     pl.hudrpos-1,2, 239,2);
         switch(wep->ammotype & AT_Disp) {
         case AT_ADis|AT_MDis:
            PrintSprite(sp_HUD_M_BarBig, pl.hudrpos-106,2, ymg+7,2);
         case AT_ADis: BeginPrintStrL("AMMO"); break;
         case AT_MDis: BeginPrintStrL("MAG");  break;
         }
         PrintText(fnt, hudcolor, pl.hudrpos-39,1, 238,2, _u_no_unicode);
      }
      break;
   case _hud_cybermage:
      xmg = pl.hudrpos-78;
      ymg = 229;
      xam = 58;
      if(wep->ammotype & AT_Disp) {
         PrintSprite(sp_HUD_C_SplitFront, pl.hudrpos,2, 238,2);
         switch(wep->ammotype & AT_Disp) {
         case AT_ADis|AT_MDis: PrintSprite(sp_HUD_C_Back, pl.hudrpos-80,2, 238,2);
         case AT_ADis:         PrintSprite(sp_HUD_C_AMMO, pl.hudrpos- 4,2, 235,2); break;
         case AT_MDis:         PrintSprite(sp_HUD_C_MAG,  pl.hudrpos- 4,2, 235,2); break;
         }
      }
      break;
   case _hud_informant:
      xmg = pl.hudrpos-77;
      ymg = 229;
      xam = 62;
      if(wep->ammotype & AT_Disp) {
         PrintSprite(sp_HUD_I_AmmoBack, pl.hudrpos,2, 239,2);
         switch(wep->ammotype & AT_Disp) {
         case AT_ADis|AT_MDis: PrintSprite(sp_HUD_I_AmmoExtend, pl.hudrpos-80,2, 238,2);
                               PrintSprite(sp_HUD_I_AMAG, pl.hudrpos-4,2, 236,2); break;
         case AT_ADis:         PrintSprite(sp_HUD_I_AMMO, pl.hudrpos-4,2, 236,2); break;
         case AT_MDis:         PrintSprite(sp_HUD_I_NMAG, pl.hudrpos-4,2, 236,2); break;
         }
      }
      break;
   case _hud_assassin:
      fnt = sf_lmidfont;
      xmg = pl.hudrpos-52;
      ymg = 228;
      xam = -2;
      yam = -7;
      if(wep->ammotype & AT_Disp) {
         PrintSprite(sp_HUD_A_RightBack, pl.hudrpos,2, 239,2);
      }
      break;
   case _hud_darklord:
      xmg = pl.hudrpos-88;
      ymg = 228;
      xam = 62;
      if(wep->ammotype & AT_Disp) {
         PrintSprite(sp_HUD_D_AmmoBack, pl.hudrpos,2, 239,2);
         switch(wep->ammotype & AT_Disp) {
         case AT_ADis|AT_MDis: PrintSprite(sp_HUD_D_Ammo2Back, pl.hudrpos-90,2, 239,2);
         case AT_ADis:         BeginPrintStrL(u8"\n"); break;
         case AT_MDis:         BeginPrintStrL(u8"\n"); break;
         }
         PrintText(sf_lmidfont, hudcolor, pl.hudrpos-11,4, 230,0, _u_no_unicode);
      }
      break;
   case _hud_old:
      xmg = pl.hudrpos;
      ymg = 234;
      xam = 77;
      xal = 2;
      if(wep->ammotype & AT_Disp) {
         PrintSprite(sp_HUD_H_B2, pl.hudrpos,2, 240,2);
         switch(wep->ammotype & AT_Disp) {
         case AT_ADis|AT_MDis: PrintSprite(sp_HUD_H_B2, pl.hudrpos-77,2, 240,2);
         case AT_ADis:         PrintSprite(sp_HUD_H_A2, pl.hudrpos   ,2, 240,2); break;
         case AT_MDis:         PrintSprite(sp_HUD_H_A1, pl.hudrpos   ,2, 240,2); break;
         }
      }
      break;
   }
   if(wep->ammotype & AT_MDis) {
      ACS_BeginPrint();
      if(wep->ammotype & AT_Ammo && !wep->ammocur) {
         ACS_PrintChar('O');
         ACS_PrintChar('U');
         ACS_PrintChar('T');
      } else {
         if(!(wep->ammotype & AT_Ammo)) {
            PrintStrL(u8"∞");
         }
         i32 magcur, magmax;
         switch(wep->ammotype & AT_Type) {
         case AT_Mgzn|AT_Ammo:
         case AT_Mgzn:
            magcur = wep->magmax - wep->magcur;
            magmax = wep->magmax;
            break;
         case AT_Ammo:
            magcur = wep->ammocur;
            magmax = wep->ammomax;
            break;
         default:
            magmax = -1;
         }
         if(magmax >= 0) {
            if(magmax >= 1000 && magcur < 1000) {
               ACS_PrintChar(' ');
            }
            if(magmax >= 100 && magcur < 100) {
               ACS_PrintChar(' ');
            }
            if(magmax >= 10 && magcur < 10) {
               ACS_PrintChar(' ');
            }
            ACS_PrintInt(magcur);
            ACS_PrintChar('/');
            ACS_PrintInt(magmax);
         }
      }
      PrintText(fnt, hudcolor, xmg,xal, ymg,0, _u_no_unicode);
   }
   if(wep->ammotype & AT_ADis) {
      i32 x = xmg - (wep->ammotype & AT_MDis ? xam : 0);
      i32 y = ymg - (wep->ammotype & AT_MDis ? yam : 0);
      i32 ammo;
      switch(wep->ammotype & AT_Type) {
      case AT_Ammo|AT_Mgzn:
      case AT_Ammo: ammo = wep->ammocur; break;
      case AT_Mgzn: ammo = wep->magcur;  break;
      default:      ammo = -1;           break;
      }
      ACS_BeginPrint();
      if(ammo >= 0) {
         ACS_PrintInt(ammo);
      } else {
         PrintStrL(u8"∞");
      }
      PrintText(fnt, hudcolor, x,xal, y,0, _u_no_unicode);
   }
   if(showweps) {
      i32 cr1, cr2, cr3, crc;
      i32 _x, _y;
      switch(hudtype) {
      default:
      case _hud_marine:
         PrintSprite(sp_HUD_M_Bar, pl.hudrpos-42,2, 239,2);
         cr1 = Cr(wselm1); cr2 = Cr(wselm2);
         cr3 = Cr(wselm3); crc = Cr(wselms);
         _x = pl.hudrpos-39; _y = 238;
         break;
      case _hud_cybermage:
         PrintSprite(sp_HUD_C_Bar, pl.hudrpos,2, 220,2);
         cr1 = Cr(wselc1); cr2 = Cr(wselc2);
         cr3 = Cr(wselc3); crc = Cr(wselcs);
         _x = pl.hudrpos+3; _y = 219;
         break;
      case _hud_informant:
         cr1 = Cr(wseli1); cr2 = Cr(wseli2);
         cr3 = Cr(wseli3); crc = Cr(wselis);
         _x = pl.hudrpos+3; _y = 219;
         break;
      case _hud_assassin:
         cr1 = Cr(wsela1); cr2 = Cr(wsela2);
         cr3 = Cr(wsela3); crc = Cr(wselas);
         _x = pl.hudrpos+3; _y = 222;
         break;
      case _hud_darklord:
         PrintSprite(sp_HUD_D_WepBack, pl.hudrpos,2, 218,2);
         cr1 = Cr(wseld1); cr2 = Cr(wseld2);
         cr3 = Cr(wseld3); crc = Cr(wselds);
         _x = pl.hudrpos+3; _y = 216;
         break;
      case _hud_old:
         PrintSprite(sp_HUD_H_W1, pl.hudlpos+78,1, 240,2);
         cr1 = CR_BLUE; cr2 = CR_BLUE;
         cr3 = CR_BLUE; crc = CR_GOLD;
         _x = pl.hudlpos+154; _y = 240;
         break;
      }
      for(i32 i = 1; i < SLOT_MAX; i++) ifauto(i32, slot, pl.weapon.slot[i]) {
         i32 x = _x+0.2 - 8 * (SLOT_MAX - i);
         i32 y = _y+0.2;
         i32 cr;
         switch(slot) {
         case 1:  cr = cr1; break;
         case 2:  cr = cr2; break;
         default: cr = cr3; break;
         }
         ACS_BeginPrint();
         ACS_PrintInt(i);
         PrintText(sf_lsmlhfnt, cr, x,2, y,2, _u_no_unicode);
         if(pl.weapon.cur.info->slot == i) {
            SetFade(fid_slotnS + i, 1, 6);
         }
         if(CheckFade(fid_slotnS + i)) {
            ACS_BeginPrint();
            ACS_PrintInt(i);
            PrintText(sf_lsmlhfnt, crc, x,2, y,2, _u_no_unicode|_u_fade, fid_slotnS + i);
         }
      }
   }
   switch(P_Wep_CurType()) {
   case weapon_m_rifle:
      if(EDataI(_edt_riflegrenade)) {
         PrintSprite(sp_HUD_H_D44, pl.hudrpos-40,1, 237,1);
      }
      break;
   case weapon_c_smg: {
      i32 y = showweps ? 0 : 14;
      i32 heat = InvNum(so_SMGHeat)/500.0k*63;
      PrintSprite(sp_HUD_C_BarSmall, pl.hudrpos-63,1, 196+y,1);
      PrintSpriteClip(sp_HUD_C_HeatBar, pl.hudrpos-63,1, 196+y,1, 0,0,heat,9);
      break;
   }
   }
}

static void HUD_BottomLeft(void) {
   i32 health = pl.health;
   i32 x;
   i32 y;
   i32 cr_g = 0;
   i32 cr_t;
   str wgfx = sa_wgfx[pl.weapon.cur.info->slot];
   switch(hudtype) {
   default:
   case _hud_marine:
      PrintSprite(InvNum(so_PowerStrength) ? sp_HUD_M_SplitBackRed : sp_HUD_M_SplitBack, pl.hudlpos,1, 240,2);
      BeginPrintStrL("VIT");
      PrintText(sf_bigupper, hudcolor, pl.hudlpos+2,1, 238,2, _u_no_unicode);
      for(i32 i = 0, x_ = (8 + pl.ticks) % 57; i < 20; i++) {
         i32 xx = x_ - i;
         if(xx < 0) xx += 57;
         i32 y_ = 9;
         if(xx < 11) y_ += 11 - xx % 12;
         PrintSprite(wgfx, pl.hudlpos+88-xx,1, 215+y_,1, _u_alpha, (20 - i) / 20.0);
      }
      x    = pl.hudlpos+34;
      y    = 232;
      cr_g = CR_YELLOW;
      cr_t = CR_PURPLE;
      break;
   case _hud_cybermage:
      PrintSprite(sp_HUD_C_SplitBack, pl.hudlpos  ,1, 239,2);
      PrintSprite(sp_HUD_C_VIT,       pl.hudlpos+2,1, 237,2);
      x = pl.hudlpos+21;
      y = 229;
      break;
   case _hud_informant:
      PrintSprite(sp_HUD_I_HPBack, pl.hudlpos,1, 239,2);
      udata.health.value = pl.health;
      lerpscr(&udata.health);
      health = udata.health.value_display;
      x      = pl.hudlpos+25;
      y      = 230;
      cr_g   = CR_WHITE;
      cr_t   = CR_RED;
      break;
   case _hud_assassin:
      PrintSprite(sp_HUD_A_LeftBack, pl.hudlpos,1, 239,2);
      x    = pl.hudlpos+4;
      y    = 231;
      cr_g = CR_RED;
      cr_t = CR_ICE;
      break;
   case _hud_darklord:
      PrintSprite(sp_HUD_D_HPBack, pl.hudlpos,1, 239,2);
      if(pl.shieldmax) {
         ifw(i32 fr = pl.shield / (k32)pl.shieldmax * 49, fr > 0) {
            PrintSpriteClip(sp_HUD_D_ShieldBack, pl.hudlpos+16,1, 239,2, 0,0,fr,21);
         }
      }
      if(pl.regenwaitmax && pl.shield != pl.shieldmax) {
         ifw(i32 fr = (pl.regenwaitmax - pl.regenwait) / (k32)pl.regenwaitmax * 49, fr > 0) {
            PrintSpriteClip(sp_HUD_D_ShieldCharge, pl.hudlpos+16,1, 239,2, 0,0,fr,21, _u_alpha, (ACS_Sin(pl.ticks / 30.0k) + 1.0k) / 2.0k);
         }
      }
      if(CheckFade(fid_shielddone)) {
         PrintSprite(sp_HUD_D_ShieldDone, pl.hudlpos+16,1, 239,2, _u_add|_u_fade, fid_shielddone);
      }
      BeginPrintStrL(u8"");
      PrintText(sf_lmidfont, hudcolor, pl.hudlpos+8,4, 229,0, _u_no_unicode);
      for(i32 i = 0, x_ = (8 + pl.ticks) % 41; i < 20; i++) {
         i32 xx = x_ - i;
         if(xx < 0) xx += 41;
         PrintSprite(wgfx, pl.hudlpos+20+xx,1, 220,1, _u_alpha, (20 - i) / 20.0);
      }
      x    = pl.hudlpos+18;
      y    = 228;
      cr_g = CR_CYAN;
      cr_t = CR_FIRE;
      break;
   case _hud_old:
      PrintSprite(InvNum(so_PowerStrength) ? sp_HUD_H_B4 : sp_HUD_H_B1, pl.hudlpos,1, 240,2);
      for(i32 i = 0, x_ = (8 + pl.ticks) % 77; i < 20; i++) {
         i32 xx = x_ - i;
         if(xx < 0) xx += 77;
         i32 y_ = 9;
         if(xx < 11) y_ += 11 - xx % 12;
         PrintSprite(wgfx, pl.hudlpos+77-xx,1, 218+y_,1, _u_alpha, (20 - i) / 20.0);
      }
      x    = pl.hudlpos;
      y    = 234;
      cr_g = CR_YELLOW;
      cr_t = CR_PURPLE;
      break;
   }
   if(cr_g) {
      k32 ft = 0;
      if(pl.health < pl.old.health) {
         udata.cr = cr_t;
         ft = (pl.old.health - pl.health) / 300.0;
         ft = clampk(ft, 0.1, 1.0);
      } else if(pl.health > pl.old.health) {
         udata.cr = cr_g;
         ft = 0.1;
      }
      if(ft) {
         SetFade(fid_health, 4, ft * 255);
      }
   }
   i32 protect = pl.megaProtect;
   ACS_BeginPrint();
   ACS_PrintInt(health);
   PrintText(sf_bigupper, hudcolor, x,1, y,0, _u_no_unicode);
   if(protect) {
      k32 amt = protect / 200.0;
      ACS_BeginPrint();
      ACS_PrintInt(health);
      PrintText(sf_bigupper, CR_TAN, x,1, y,0, _u_no_unicode|_u_alpha, amt);
   }
   if(CheckFade(fid_health)) {
      ACS_BeginPrint();
      ACS_PrintInt(health);
      PrintText(sf_bigupper, udata.cr, x,1, y,0, _u_no_unicode|_u_fade, fid_health);
   }
}

void Upgr_HeadsUpDisp_Activate(void) {
   lerpscr_init(&udata.score,     pl.score,     4);
   lerpscr_init(&udata.health,    pl.health,    1);
   lerpscr_init(&udata.overdrive, pl.overdrive, 1);
}

void Upgr_HeadsUpDisp_Render(void) {
   if(!pl.hudenabled) {
      return;
   }
   hudtype = CVarGetI(sc_hud_type);
   if(hudtype <= 0 || hudtype > _hud_max) {
      hudtype = pl.pclass;
   } else {
      --hudtype;
   }
   hudcolor = hudtype != _hud_old ? P_Color(hudtype) : CR_RED;
   HUD_Log();
   HUD_TopRight();
   HUD_BottomRight();
   HUD_BottomLeft();
}

/* EOF */
