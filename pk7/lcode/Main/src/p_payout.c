#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"
#include "lith_hudid.h"


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_PlayerPayout
//
[[__call("ScriptS")]]
void Lith_PlayerPayout(player_t *p)
{
#define Left(...) \
   ( \
      HudMessageF("CNFONT", __VA_ARGS__), \
      HudMessageParams(HUDMSG_FADEOUT, hid--, CR_WHITE, 16.1, y + .1, TICSECOND, .2) \
   )
      
#define Right(...) \
   ( \
      HudMessageF("CNFONT", __VA_ARGS__), \
      HudMessageParams(HUDMSG_FADEOUT, hid--, CR_WHITE, 280.2, y + .1, TICSECOND, .2) \
   )
   
#define Head(...) \
   ( \
      HudMessageF("DBIGFONT", __VA_ARGS__), \
      HudMessageParams(HUDMSG_FADEOUT, hid--, CR_WHITE, 8.1, y + 0.1, TICSECOND, 0.2) \
   )
   
#define GenCount(word, name) \
   if(1) \
   { \
      Left(word " %.1lk%%", pay.name##pct); \
      \
      if(i < 35) \
      { \
         HudMessageF("CNFONT", "%S\Cnscr", Lith_ScoreSep(lerplk(0, pay.name##scr, i / 34.0lk))); \
         HudMessageParams(HUDMSG_FADEOUT, hid, CR_WHITE, 280.2, y + 0.1, 2, 0.2); \
      } \
      \
      y += 9; \
      hid--; \
   } else (void)0
   
   payoutinfo_t pay = payout;
   
   ACS_SetActivator(p->tid);
   ACS_Delay(25);
   ACS_SetHudSize(320, 200);
   
   for(int i = 0; i < 35*3; i++)
   {
      int hid = hid_base_payout;
      int y = 16;
      bool counting = false;
      
      Head("RESULTS");
      
      if(i < 16)
      {
         HudMessageF("DBIGFONT", "RESULTS");
         HudMessageParams(HUDMSG_FADEOUT | HUDMSG_ADDBLEND, hid, CR_WHITE, 8 + 0.1, y + .1, TICSECOND, 0.5);
      }
      
      y += 16;
      hid--;
      
      if(pay.killmax) {GenCount("ELIMINATED", kill); counting |= pay.killnum;}
      if(pay.itemmax) {GenCount("ARTIFACTS",  item); counting |= pay.itemnum;}
      
      if(i > 35) {y += 7; Head("TOTAL"); y += 16;}
      if(i > 35 * 1.25) {Left("Tax"); Right("%i\Cnscr", pay.tax); y += 9;}
      
      if(i > 35 * 1.5)
      {
         Left("Total"); Right("%S\Cnscr", Lith_ScoreSep(pay.total)); y += 16;
         
         Head("PAYMENT"); y += 16;
         Left("Primary Account"); Right("%STRANSACTION CLOSED", (i % 6) == 0 ? "\Cn" : "");
      }
      
      if(ACS_GetUserCVar(p->number, "lith_player_resultssound"))
      {
         if(counting)
            if(i < 35)
               ACS_LocalAmbientSound("player/counter", 80);
            else if(i == 35)
               ACS_LocalAmbientSound("player/counterdone", 80);
         
         if(i == (int)(35 * 1.25) || i == (int)(35 * 1.5))
            ACS_LocalAmbientSound("player/counterdone", 80);
      }
      
      ACS_Delay(1);
   }
   
   ACS_Delay(20);
   
   Lith_GiveScore(p, pay.total, true);
   
#undef Left
#undef Right
#undef Head
#undef GenCount
}

// EOF

