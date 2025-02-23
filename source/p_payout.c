// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Level payout UI.                                                         │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"
#include "p_hudid.h"

dynam_aut script
void P_Scr_Payout(void) {
   enum {
      _begin_total      = 35,
      _begin_tax        = 44,
      _begin_grandtotal = 52,
   };

#define Msg(x, xa, font, ...) \
   (BeginPrintFmt(__VA_ARGS__), \
    PrintText(font, CR_WHITE, x,xa, y,1, _u_fade, fid_result))

#define Fram()    PrintSprite(sp_UI_ResultFrame, 14,1, y-1,1, _u_fade, fid_result)
#define Left(...) Msg(16,  1, sf_smallfnt, __VA_ARGS__)
#define Rght(...) Msg(144, 6, sf_smallfnt, __VA_ARGS__)
#define Head(...) Msg(8,   1, sf_bigupper, __VA_ARGS__)

#define CountScr(scr) \
   scoresep(i < _begin_total ? lerplk(0, scr, i / 34.0lk) : scr)

   i32 amounty;
   struct payoutinfo pay = wl.pay;

   pl.setActivator();
   ACS_Delay(25);
   WaitPause();

   SetFade(fid_result,  35 * 3, 8);
   SetFade(fid_result2, 16,     16);

   for(i32 i = 0; CheckFade(fid_result); i++) {
      i32 y = 20;
      bool counting = false;
      cstr res = tmpstr(sl_res_results);

      SetSize(320, 240);

      PrintRect(4, 16, 152, 208, (GetFade(fid_result) / 2) << 24);
      Head(res);

      if(CheckFade(fid_result2)) {
         BeginPrintStr(res);
         PrintText(sf_bigupper, CR_WHITE, 8,1, y,1, _u_add|_u_fade, fid_result2);
      }

      y += 16;

#define Pay(name) \
   if(pay.name##max) { \
      Fram(); \
      Left(tmpstr(sl_res_##name), pay.name##pct); \
      Rght("%s\Cnscr", CountScr(pay.name##scr)); \
      counting |= pay.name##num; \
      y += 9; \
   }

      Pay(kill);
      Pay(item);
      Pay(scrt);

#define Activity(name) \
   if(pay.activities.name) { \
      Fram(); \
      Left(tmpstr(sl_res_##name)); \
      Rght("%s\Cnscr", CountScr(pay.activities.name)); \
      counting = true; \
      y += 9; \
   }

      Activity(kill100);
      Activity(item100);
      Activity(scrt100);
      Activity(par);
      Activity(sponsor);

      if(i > _begin_total) {
         y += 7;
         Head(tmpstr(sl_res_total));
         y += 16;
      }

      if(i > _begin_tax) {
         Fram();
         Left(tmpstr(sl_res_tax));
         Rght("%s\Cnscr", scoresep(pay.tax));
         y += 9;
      }

      if(i > _begin_grandtotal) {
         Fram();
         Left(tmpstr(sl_res_subtotal));
         amounty = y;
         Rght("%s\Cnscr", scoresep(pay.total));
         y += 16;

         Head(tmpstr(sl_res_payment));
         y += 16;

         Fram();
         Left(tmpstr(sl_res_account));
         Rght(tmpstr(sl_res_closed), (i % 6) < 3 ? 'n' : '-');
      }

      if(cv.player_resultssound) {
         if(counting) {
            str snd = snil;

            /**/ if(i <  _begin_total) snd = ss_player_counter;
            else if(i == _begin_total) snd = ss_player_counterdone;

            if(snd != snil) AmbientSound(snd, 0.62);
         }

         if(i == _begin_tax || i == _begin_grandtotal) {
            AmbientSound(ss_player_counterdone, 0.62);
         }
      }

      ACS_Delay(1);
   }

   ACS_Delay(20);

   if(pay.total) {
      P_Scr_GivePos(16, amounty, pay.total, true);
   }
}

/* EOF */
