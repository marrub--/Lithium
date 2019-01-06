// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// p_payout.c: Level payout UI.

#if LITHIUM
#include "common.h"
#include "p_player.h"
#include "w_world.h"
#include "p_hudid.h"

// Extern Functions ----------------------------------------------------------|

script
void Lith_PlayerPayout(struct player *p)
{
#define Left(...) \
   ( \
      HudMessageF(s_cnfont, __VA_ARGS__), \
      HudMessageParams(HUDMSG_FADEOUT, hid--, CR_WHITE, 16.1, y + .1, TS, .2) \
   )

#define Right(...) \
   ( \
      HudMessageF(s_cnfont, __VA_ARGS__), \
      HudMessageParams(HUDMSG_FADEOUT, hid--, CR_WHITE, 280.2, y + .1, TS, .2) \
   )

#define Head(...) \
   ( \
      HudMessageF(s_dbigfont, __VA_ARGS__), \
      HudMessageParams(HUDMSG_FADEOUT, hid--, CR_WHITE, 8.1, y + 0.1, TS, 0.2) \
   )

#define GenCount(word, name) \
   if(1) \
   { \
      Left(word " %.1lk%%", pay.name##pct); \
      \
      if(i < 35) \
      { \
         HudMessageF(s_cnfont, "%s\Cnscr", scoresep(lerplk(0, pay.name##scr, i / 34.0lk))); \
         HudMessageParams(HUDMSG_FADEOUT, hid, CR_WHITE, 280.2, y + 0.1, 2, 0.2); \
      } \
      \
      y += 9; \
      hid--; \
   } else (void)0

   struct payoutinfo pay = payout;

   p->setActivator();
   ACS_Delay(25);
   ACS_SetHudSize(320, 240);

   for(i32 i = 0; i < 35*3; i++)
   {
      i32 hid = hid_base_payout;
      i32 y = 20;
      bool counting = false;

      Head("RESULTS");

      if(i < 16) {
         HudMessageF(s_dbigfont, "RESULTS");
         HudMessageParams(HUDMSG_FADEOUT | HUDMSG_ADDBLEND, hid, CR_WHITE, 8 + 0.1, y + .1, TS, 0.5);
      }

      y += 16;
      hid--;

      if(pay.killmax) {GenCount("ELIMINATED", kill); counting |= pay.killnum;}
      if(pay.itemmax) {GenCount("ARTIFACTS",  item); counting |= pay.itemnum;}

      if(i > 35) {y += 7; Head("TOTAL"); y += 16;}
      if(i > 35 * 1.25) {Left("Tax"); Right("%s\Cnscr", scoresep(pay.tax)); y += 9;}

      if(i > 35 * 1.5)
      {
         Left("Total"); Right("%s\Cnscr", scoresep(pay.total)); y += 16;

         Head("PAYMENT"); y += 16;
         Left("Primary Account"); Right("%sTRANSACTION CLOSED", (i % 6) == 0 ? "\Cn" : "");
      }

      if(p->getCVarI(sc_player_resultssound))
      {
         if(counting) {
                 if(i <  35) ACS_LocalAmbientSound(ss_player_counter, 80);
            else if(i == 35) ACS_LocalAmbientSound(ss_player_counterdone, 80);
         }

         if(i == (i32)(35 * 1.25) || i == (i32)(35 * 1.5))
            ACS_LocalAmbientSound(ss_player_counterdone, 80);
      }

      ACS_Delay(1);
   }

   ACS_Delay(20);

   p->giveScore(pay.total, true);
}
#endif

// EOF
