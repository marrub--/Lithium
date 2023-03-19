// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Zoom upgrade.                                                            │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"

#include <math.h>

#define udata pl.upgrdata.zoom

void Upgr_Zoom_Deactivate(void) {
   udata.zoom = udata.vzoom = 0;
}

void Upgr_Zoom_Update(void) {
   if(udata.zoom == 0 && udata.vzoom == 0) return;

   if(udata.vzoom < udata.zoom) {
      udata.vzoom = lerplk(udata.vzoom, udata.zoom, 0.099);
   } else {
      udata.vzoom = lerplk(udata.vzoom, udata.zoom, 0.2);
   }

   k32 diff = udata.vzoom - udata.zoom;
   if(diff > 0.5 || diff < -0.5) {
      if(udata.vzoom < udata.zoom) AmbientSound(ss_player_zoomin,  0.23);
      else                         AmbientSound(ss_player_zoomout, 0.23);
   }
   else
      udata.vzoom = udata.zoom;
}

void Upgr_Zoom_Render(void) {
   if(udata.vzoom) {
      PrintSprite(sp_HUD_Vignette, 160,0, 120,0, _u_alpha, 0.3);

      ACS_SetCameraToTexture(0, sp_LITHCAM2, 90 - udata.vzoom);
      PrintSprite(sp_LITHCAM2, 160,0, 120,0);

      PrintSprite(sp_HUD_ZoomOverlay, 160,0, 120,0, _u_alpha, 0.5);

      static
      char const points[] = "E SES SWW NWN NE";

      for(i32 i = 0; i < 8; i++) {
         k32 yaw = fastfrack(pl.yaw + i * 0.125 + 0.125);
         i32 x = yaw * 320 * 4;
         if(x < 0 || x > 320) continue;

         BeginPrintStrN(&points[i * 2], 2);
         PrintText(sf_smallfnt, CR_WHITE, x,1, 120,0);
      }
   }
}

script_str ext("ACS") type("net") addr(OBJ "KeyZoom")
void Z_KeyZoom(i32 amt) {
   if(get_bit(pl.upgrades[UPGR_Zoom].flags, _ug_active)) udata.zoom += amt;
   if(udata.zoom < 0)                                    udata.zoom  = 0;
   if(udata.zoom > 80)                                   udata.zoom  = 80;
}

/* EOF */
