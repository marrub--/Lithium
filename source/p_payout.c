/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Level payout UI.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "w_world.h"
#include "p_hudid.h"

/* Extern Functions -------------------------------------------------------- */

script
void P_Scr_Payout(struct player *p) {
   Str(sp_resultframe,        s":UI:ResultFrame");
   Str(ss_player_counter,     s"player/counter");
   Str(ss_player_counterdone, s"player/counterdone");

   enum {
      _begin_total      = 35,
      _begin_tax        = 44,
      _begin_grandtotal = 52,
   };

#define Msg(x, xa, font, ...) \
   (PrintTextFmt(__VA_ARGS__), \
    PrintTextF(font, CR_WHITE, x,xa, y,1, fid_result))

#define Fram()    PrintSpriteF(sp_resultframe, 14,1, y-1,1, fid_result)
#define Left(...) Msg(16,  1, s_smallfnt, __VA_ARGS__)
#define Rght(...) Msg(144, 6, s_smallfnt, __VA_ARGS__)
#define Head(...) Msg(8,   1, s_bigupper, __VA_ARGS__)

#define CountScr(scr) \
   scoresep(i < _begin_total ? lerplk(0, scr, i / 34.0lk) : scr)

   struct payoutinfo pay = payout;

   p->setActivator();
   ACS_Delay(25);

   SetFade(fid_result,  35 * 3, 8);
   SetFade(fid_result2, 16,     16);

   for(i32 i = 0; CheckFade(fid_result); i++) {
      i32 y = 20;
      bool counting = false;
      cstr res = LC(LANG "RES_RESULTS");

      SetSize(320, 240);

      PrintRectA(4, 16, 152, 208, 0x000000, GetFade(fid_result) / 2);
      Head(res);

      if(CheckFade(fid_result2)) {
         PrintTextChS(res);
         PrintTextFX(s_bigupper, CR_WHITE, 8,1, y,1, fid_result2, ptf_add);
      }

      y += 16;

#define Pay(name) \
   if(pay.name##max) { \
      Fram(); \
      Left(LC(LANG "RES_" #name), pay.name##pct); \
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
      Left(LC(LANG "RES_" #name)); \
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
         Head(LC(LANG "RES_TOTAL"));
         y += 16;
      }

      if(i > _begin_tax) {
         Fram();
         Left(LC(LANG "RES_TAX"));
         Rght("%s\Cnscr", scoresep(pay.tax));
         y += 9;
      }

      if(i > _begin_grandtotal) {
         Fram();
         Left(LC(LANG "RES_SUBTOTAL"));
         Rght("%s\Cnscr", scoresep(pay.total));
         y += 16;

         Head(LC(LANG "RES_PAYMENT"));
         y += 16;

         Fram();
         Left(LC(LANG "RES_ACCOUNT"));
         Rght(LC(LANG "RES_CLOSED"), (i % 6) < 3 ? 'n' : '-');
      }

      if(p->getCVarI(sc_player_resultssound)) {
         if(counting) {
            str snd = snil;

            /**/ if(i <  _begin_total) snd = ss_player_counter;
            else if(i == _begin_total) snd = ss_player_counterdone;

            if(snd != snil) ACS_LocalAmbientSound(snd, 80);
         }

         if(i == _begin_tax || i == _begin_grandtotal)
            ACS_LocalAmbientSound(ss_player_counterdone, 80);
      }

      ACS_Delay(1);
   }

   ACS_Delay(20);

   P_Scr_Give(p, pay.total, true);
}

/* EOF */
