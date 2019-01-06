// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// u_zoom.c: Zoom upgrade.

#include "u_common.h"

#include <math.h>

#define UData UData_Zoom(upgr)

// Scripts -------------------------------------------------------------------|

script_str ext("ACS") type("net")
void Lith_KeyZoom(i32 amt)
{
   if(ACS_Timer() < 10) return;

   withplayer(LocalPlayer)
   {
      struct upgrade *upgr = p->getUpgr(UPGR_Zoom);
      if(upgr) {
         if(upgr->active)    UData.zoom += amt;
         if(UData.zoom < 0)  UData.zoom  = 0;
         if(UData.zoom > 80) UData.zoom  = 80;
      }
   }
}

// Extern Functions ----------------------------------------------------------|

stkcall
void Upgr_Zoom_Deactivate(struct player *p, struct upgrade *upgr)
{
   UData.zoom = UData.vzoom = 0;
}

script
void Upgr_Zoom_Update(struct player *p, struct upgrade *upgr)
{
   if(UData.zoom == 0 && UData.vzoom == 0) return;

   if(UData.vzoom < UData.zoom)
      UData.vzoom = lerplk(UData.vzoom, UData.zoom, 0.099);
   else
      UData.vzoom = lerplk(UData.vzoom, UData.zoom, 0.2);

   k32 diff = UData.vzoom - UData.zoom;
   if(diff > 0.5 || diff < -0.5)
   {
      if(UData.vzoom < UData.zoom) ACS_LocalAmbientSound(ss_player_zoomin,  30);
      else                         ACS_LocalAmbientSound(ss_player_zoomout, 30);
   }
   else
      UData.vzoom = UData.zoom;
}

stkcall
void Upgr_Zoom_Render(struct player *p, struct upgrade *upgr)
{
   if(UData.vzoom)
   {
      PrintSpriteA(sp_Vignette, 160,0, 120,0, 0.3);

      ACS_SetCameraToTexture(p->cameratid, s_LITHCAM2, 90 - UData.vzoom);
      PrintSprite(s_LITHCAM2, 160,0, 120,0);

      PrintSpriteA(sp_ZoomOverlay, 160,0, 120,0, 0.5);

      static char const points[] = "E SES SWW NWN NE";

      for(i32 i = 0; i < 8; i++)
      {
         k32 yaw = (p->yaw + i * 0.125 + 0.125) % 1.0;
         i32 x = yaw * 320 * 4;
         if(x < 0 || x > 320) continue;

         PrintTextChr(&points[i * 2], 2);
         PrintText(s_cbifont, CR_WHITE, x,1, 120,0);
      }
   }
}

// EOF
