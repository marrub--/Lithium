/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Flashlight upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

#include <math.h>

#define UData pl.upgrdata.flashlight

/* Extern Functions -------------------------------------------------------- */

void Upgr_Flashlight_Enter(struct upgrade *upgr) {
   UData.on        = false;
   UData.battery   = 0xffff;
   UData.intensity = 0.0;
   UData.target    = 0.0;
   UData.speed     = 1.0;
}

void Upgr_Flashlight_Deactivate(struct upgrade *upgr) {
   UData.on        = false;
   UData.intensity = 0.0;
   UData.target    = 0.0;
   UData.speed     = 1.0;
}

script void Upgr_Flashlight_Update(struct upgrade *upgr) {
   i32 bat_life  = CVarGetI(sc_light_battery) * 35;
   i32 bat_regen = CVarGetI(sc_light_regen);

   bool was_on = UData.on;

   if(UData.was_on != UData.on) {
      AmbientSound(UData.on ? ss_player_lighton : ss_player_lightoff, 1.0);

      if(UData.was_on) {
         UData.target = 0.0;
         UData.speed  = 0.05;
      } else {
         UData.target = 1.0;
         UData.speed  = 0.15;
      }
   }

   if(bat_life != 0) {
      if(UData.on) {UData.battery--;}
      else         {UData.battery += bat_regen;}

      if(UData.battery > bat_life) {
         UData.battery = bat_life;
      } else if(UData.battery < 0) {
         UData.battery = 0;
         UData.on = false;
      } else if(UData.battery < bat_life / 4 && ticks % 10 == 0) {
         UData.intensity = ACS_RandomFixed(0.25, 0.75);
      }
   }

   if(UData.intensity != UData.target) {
      UData.intensity = lerplk(UData.intensity, UData.target, UData.speed);
   }

   UData.was_on = was_on;

   Dbg_Stat("*target: %lk\n*intensity: %lk\n*speed: %lk\n"
            "*battery: %u\n",
            UData.target, UData.intensity, UData.speed, UData.battery);
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "GetFlashlightIntensity")
k32 Sc_GetFlashlightIntensity(void) {
   if(!P_None()) {return UData.intensity;}
   return 0.0;
}

script_str type("net") ext("ACS") addr(OBJ "KeyLight")
void Sc_KeyLight(void) {if(!P_None()) {UData.on = !UData.on;}}

/* EOF */
