// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Flashlight functionality.                                                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"

static i32 next_on;

static void P_lightOff(void) {
   next_on = -1;
   pl.light.on        =
   pl.light.was_on    = _light_off;
   pl.light.battery   = 0xffff;
   pl.light.intensity = 0.0;
   pl.light.target    = 0.0;
   pl.light.speed     = 1.0;
}

static void P_lightUpdate(void) {
   i32 bat_life  = CVarGetI(sc_light_battery) * 35;
   i32 bat_regen = CVarGetI(sc_light_regen);
   pl.light.was_on = pl.light.on;
   if(next_on != -1) {
      pl.light.on = next_on;
      next_on = -1;
   }
   if(bat_life != 0) {
      if(pl.light.on) {pl.light.battery--;}
      else            {pl.light.battery += bat_regen;}
      if(pl.light.battery > bat_life) {
         pl.light.battery = bat_life;
      } else if(pl.light.battery < 0) {
         pl.light.battery = 0;
         pl.light.on = _light_off;
      } else if(pl.light.battery < bat_life / 4 && ACS_Timer() % 10 == 0) {
         pl.light.intensity = ACS_RandomFixed(0.25, 0.75);
      }
   }
   if(pl.light.was_on != pl.light.on) {
      AmbientSound(pl.light.on ? ss_player_lighton : ss_player_lightoff, 1.0);
      if(!pl.light.on && pl.light.was_on) {
         pl.light.target = 0.0;
         pl.light.speed  = 0.05;
      } else {
         pl.light.target = 1.0;
         pl.light.speed  = 0.15;
      }
   }
   if(pl.light.intensity != pl.light.target) {
      pl.light.intensity =
         lerplk(pl.light.intensity, pl.light.target, pl.light.speed);
      if((pl.light.intensity < pl.light.target &&
          pl.light.intensity >= pl.light.target - 0.01lk) ||
         (pl.light.intensity > pl.light.target &&
          pl.light.intensity <= pl.light.target + 0.01lk))
      {
         pl.light.intensity = pl.light.target;
      }
   }
}

void Upgr_Flashlight_Activate(void) {P_lightOff();}
void Upgr_Flashlight_Update(void)   {P_lightUpdate();}
void Upgr_Lightspark_Activate(void) {P_lightOff();}
void Upgr_Lightspark_Update(void)   {P_lightUpdate();}

script_str type("net") ext("ACS") addr(OBJ "KeyLight")
void Z_KeyLight(void) {
   if(get_bit(pcl_intruders, pl.pclass)) {
      next_on = (pl.light.on + 1) % _light_max;
   } else {
      next_on = !pl.light.on;
   }
}

/* EOF */
