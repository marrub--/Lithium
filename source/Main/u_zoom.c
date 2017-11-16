// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
// Required for sincos(3).
#define _GNU_SOURCE
#include "lith_upgrades_common.h"

#define UData UData_Zoom(upgr)

#define IsKindaSmallNumber(x) ((x) > -0.5 && (x) < 0.5)
#define KindaCloseEnough(x, y) (IsKindaSmallNumber(x - y))

// Scripts -------------------------------------------------------------------|

//
// Lith_KeyZoom
//
[[__call("ScriptS"), __extern("ACS"), __script("Net")]]
void Lith_KeyZoom(int amt)
{
   if(ACS_Timer() < 10) return;

   withplayer(LocalPlayer)
   {
      upgrade_t *upgr = p->getUpgr(UPGR_Zoom);
      if(upgr) {
         if(upgr->active)    UData.zoom += amt;
         if(UData.zoom < 0)  UData.zoom  = 0;
         if(UData.zoom > 80) UData.zoom  = 80;
      }
   }
}

// Extern Functions ----------------------------------------------------------|

//
// Deactivate
//
void Upgr_Zoom_Deactivate(player_t *p, upgrade_t *upgr)
{
   UData.zoom = UData.vzoom = 0;
}

//
// Update
//
[[__call("ScriptS")]]
void Upgr_Zoom_Update(player_t *p, upgrade_t *upgr)
{
   if(UData.vzoom < UData.zoom)
      UData.vzoom = lerpf(UData.vzoom, UData.zoom, 0.099);
   else
      UData.vzoom = lerpf(UData.vzoom, UData.zoom, 0.2);

   if(!KindaCloseEnough(UData.vzoom, UData.zoom))
   {
      if(UData.vzoom < UData.zoom)
         ACS_LocalAmbientSound("player/zoomin",  30);
      else
         ACS_LocalAmbientSound("player/zoomout", 30);
   }
   else
      UData.vzoom = UData.zoom;
}


//
// Render
//
void Upgr_Zoom_Render(player_t *p, upgrade_t *upgr)
{
   if(UData.vzoom)
   {
      DrawSpriteAlpha("lgfx/Vignette.png", hid_zoomvignette, 160, 120, TICSECOND, 0.3);

      ACS_SetCameraToTexture(p->cameratid, "LITHCAM2", 90 - UData.vzoom);
      DrawSpritePlain("LITHCAM2", hid_zoomcam, 160, 120, TICSECOND);

      DrawSpriteAlpha("lgfx/ZoomOverlay.png", hid_zoombase, 160, 120, TICSECOND, 0.5);

      __str points[8] = {"E", "SE", "S", "SW", "W", "NW", "N", "NE"};

      for(int i = 0; i < 8; i++)
      {
         float xang = p->yawf + pi + (pi4 * i);
         float xs, xc;
         sincosf(xang, &xs, &xc);
         int x = atan2f(xs, xc) * 320;

         HudMessageF("CBIFONT", points[i]);
         HudMessagePlain(hid_zoomcomp + i, 160 + x, 120, TICSECOND);
      }
   }
}

// EOF

