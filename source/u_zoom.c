/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Zoom upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

#include <math.h>

#define UData pl.upgrdata.zoom

/* Extern Functions -------------------------------------------------------- */

void Upgr_Zoom_Deactivate(struct upgrade *upgr)
{
   UData.zoom = UData.vzoom = 0;
}

script
void Upgr_Zoom_Update(struct upgrade *upgr)
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

void Upgr_Zoom_Render(struct upgrade *upgr)
{
   if(UData.vzoom)
   {
      PrintSpriteA(sp_HUD_Vignette, 160,0, 120,0, 0.3);

      ACS_SetCameraToTexture(pl.cameratid, sp_LITHCAM2, 90 - UData.vzoom);
      PrintSprite(sp_LITHCAM2, 160,0, 120,0);

      PrintSpriteA(sp_HUD_ZoomOverlay, 160,0, 120,0, 0.5);

      static
      char const points[] = "E SES SWW NWN NE";

      for(i32 i = 0; i < 8; i++)
      {
         k32 yaw = (pl.yaw + i * 0.125 + 0.125) % 1.0;
         i32 x = yaw * 320 * 4;
         if(x < 0 || x > 320) continue;

         PrintTextChr(&points[i * 2], 2);
         PrintText(sf_smallfnt, CR_WHITE, x,1, 120,0);
      }
   }
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") type("net") addr(OBJ "KeyZoom")
void Sc_KeyZoom(i32 amt)
{
   if(ACS_Timer() < 10) return;

   if(!P_None())
   {
      if(get_bit(pl.upgrades[UPGR_Zoom].flags, _ug_active)) UData.zoom += amt;
      if(UData.zoom < 0)                                    UData.zoom  = 0;
      if(UData.zoom > 80)                                   UData.zoom  = 80;
   }
}

/* EOF */
