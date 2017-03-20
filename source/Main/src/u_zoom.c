#include "lith_upgrades_common.h"

#define UserData upgr->UserData_Zoom

#define IsKindaSmallNumber(x) ((x) > -0.5 && (x) < 0.5)
#define KindaCloseEnough(x, y) (IsKindaSmallNumber(x - y))


//----------------------------------------------------------------------------
// Scripts
//

//
// Lith_KeyZoom
//
[[__call("ScriptS"), __extern("ACS"), __script("Net")]]
void Lith_KeyZoom(int amt)
{
   if(ACS_Timer() < 10) return;
   
   player_t  *p    = LocalPlayer;
   upgrade_t *upgr = &p->upgrades[UPGR_Zoom];
   
   if(upgr->active)       UserData.zoom += amt;   
   if(UserData.zoom < 0)  UserData.zoom  = 0;
   if(UserData.zoom > 80) UserData.zoom  = 80;
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Deactivate
//
void Upgr_Zoom_Deactivate(player_t *p, upgrade_t *upgr)
{
   UserData.zoom = UserData.vzoom = 0;
}

//
// Update
//
[[__call("ScriptS")]]
void Upgr_Zoom_Update(player_t *p, upgrade_t *upgr)
{
   if(UserData.vzoom < UserData.zoom)
      UserData.vzoom = lerpf(UserData.vzoom, UserData.zoom, 0.099);
   else
      UserData.vzoom = lerpf(UserData.vzoom, UserData.zoom, 0.2);
   
   if(!KindaCloseEnough(UserData.vzoom, UserData.zoom))
   {
      if(UserData.vzoom < UserData.zoom)
         ACS_LocalAmbientSound("player/zoomin",  30);
      else
         ACS_LocalAmbientSound("player/zoomout", 30);
   }
   else
      UserData.vzoom = UserData.zoom;
}


//
// Render
//
void Upgr_Zoom_Render(player_t *p, upgrade_t *upgr)
{
   if(UserData.vzoom)
   {
      DrawSpriteAlpha("lgfx/Vignette.png", hid_zoomvignette, 160, 100, TICSECOND, 0.3);
      
      ACS_SetCameraToTexture(p->cameratid, "LITHCAM2", 90 - UserData.vzoom);
      DrawSpritePlain("LITHCAM2", hid_zoomcam, 160, 100, TICSECOND);
      
      DrawSpriteAlpha("lgfx/ZoomOverlay.png", hid_zoombase, 160, 100, TICSECOND, 0.5);
      
      __str points[8] = {"E", "SE", "S", "SW", "W", "NW", "N", "NE"};
      
      for(int i = 0; i < 8; i++)
      {
         float xang = p->yawf + pi + (pi4 * i);
         int x = atan2f(sinf(xang), cosf(xang)) * 320;
         
         HudMessageF("CBIFONT", points[i]);
         HudMessagePlain(hid_zoomcomp + i, 160 + x, 100, TICSECOND);
      }
   }
}

// EOF

