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
void P_Scr_Payout(struct player *p)
{
   enum {
      begin_total      = 35,
      begin_tax        = 44,
      begin_grandtotal = 52,
   };

#define Left(...) \
   (PrintTextFmt(__VA_ARGS__), \
    PrintTextF(s_smallfnt, CR_WHITE, 16,1, y,1, fid_result))

#define Right(...) \
   (PrintTextFmt(__VA_ARGS__), \
    PrintTextF(s_smallfnt, CR_WHITE, 280,2, y,1, fid_result))

#define Head(...) \
   (PrintTextFmt(__VA_ARGS__), \
    PrintTextF(s_bigupper, CR_WHITE, 8,1, y,1, fid_result))

#define CountScr(scr) scoresep(i < begin_total ? \
                               lerplk(0, scr, i / 34.0lk) :\
                               scr)

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
      Head(res);

      if(CheckFade(fid_result2)) {
         PrintTextChS(res);
         PrintTextFX(s_bigupper, CR_WHITE, 8,1, y,1, fid_result2, ptf_add);
      }

      y += 16;

      if(pay.killmax) {
         Left(LC(LANG "RES_ELIMINATED"), pay.killpct);
         Right("%s\Cnscr", CountScr(pay.killscr));
         counting |= pay.killnum;
         y += 9;
      }

      if(pay.itemmax) {
         Left(LC(LANG "RES_ARTIFACTS"), pay.itempct);
         Right("%s\Cnscr", CountScr(pay.itemscr));
         counting |= pay.itemnum;
         y += 9;
      }

      if(i > begin_total) {
         y += 7;
         Head(LC(LANG "RES_TOTAL"));
         y += 16;
      }

      if(i > begin_tax) {
         Left(LC(LANG "RES_TAX"));
         Right("%s\Cnscr", scoresep(pay.tax));
         y += 9;
      }

      if(i > begin_grandtotal) {
         Left(LC(LANG "RES_SUBTOTAL"));
         Right("%s\Cnscr", scoresep(pay.total));
         y += 16;

         Head(LC(LANG "RES_PAYMENT"));
         y += 16;

         Left(LC(LANG "RES_ACCOUNT"));
         Right(LC(LANG "RES_CLOSED"), (i % 6) == 0 ? 'n' : '-');
      }

      if(p->getCVarI(sc_player_resultssound)) {
         if(counting) {
            str snd = snil;
                 if(i <  begin_total) snd = ss_player_counter;
            else if(i == begin_total) snd = ss_player_counterdone;
                 if(snd != snil)      ACS_LocalAmbientSound(snd, 80);
         }

         if(i == begin_tax || i == begin_grandtotal)
            ACS_LocalAmbientSound(ss_player_counterdone, 80);
      }

      ACS_Delay(1);
   }

   ACS_Delay(20);

   P_Scr_Give(p, pay.total, true);
}

/* EOF */
