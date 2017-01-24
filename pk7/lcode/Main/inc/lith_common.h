#ifndef LITH_COMMON_H
#define LITH_COMMON_H

#pragma GDCC FIXED_LITERAL ON
#pragma GDCC STRENT_LITERAL ON

#include <ACS_ZDoom.h>
#include <stdbool.h>
#include <stdfix.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define IsSmallNumber(x) ((x) > -0.001 && (x) < -0.001)

#define sink ACS_Sin
#define cosk ACS_Cos

#define max(x, y) ((x) < (y) ? (y) : (x))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define minmax(x, mi, ma) (min(max(x, mi), ma))

#define HudMessageF(font, ...) \
   ( \
      ACS_SetFont(font), \
      HudMessage(__VA_ARGS__) \
   )

#define HudMessageRainbowsF(font, ...) \
   ( \
      ACS_SetFont(font), \
      HudMessageRainbows(__VA_ARGS__) \
   )

#define HudMessageParams(flags, id, tr, x, y, hold, ...) \
   ( \
      ACS_OptHudMessage((flags) | HUDMSG_NOTWITHFULLMAP, id, tr, x, y, hold), \
      ACS_EndHudMessage(__VA_ARGS__) \
   )

#define HudMessagePlain(id, x, y, hold) \
   ( \
      ACS_OptHudMessage(HUDMSG_PLAIN | HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold), \
      ACS_EndHudMessage() \
   )

#define HudMessageFade(id, x, y, hold, fadetime) \
   ( \
      ACS_OptHudMessage(HUDMSG_FADEOUT | HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold), \
      ACS_EndHudMessage(fadetime) \
   )

#define DrawSprite(name, flags, id, x, y, hold, ...) \
   ( \
      ACS_SetFont(name), \
      HudMessage("A"), \
      HudMessageParams((flags) | HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold, __VA_ARGS__) \
   )

#define DrawSpriteFade(name, id, x, y, hold, fadetime) \
   ( \
      ACS_SetFont(name), \
      HudMessage("A"), \
      HudMessageFade(id, x, y, hold, fadetime) \
   )

#define DrawSpritePlain(name, id, x, y, hold) \
   ( \
      ACS_SetFont(name), \
      HudMessage("A"), \
      HudMessagePlain(id, x, y, hold) \
   )

#define DrawSpriteAlpha(name, id, x, y, hold, alpha) \
   ( \
      ACS_SetFont(name), \
      HudMessage("A"), \
      HudMessageParams(HUDMSG_ALPHA, id, CR_UNTRANSLATED, x, y, hold, alpha) \
   )

// Not 1.0 / 35.0 or even 0.028 because ZDoom is stupid.
#define TICSECOND (0.029)

// To make pitch values down=0, up=1
#define PITCH_BASE -0.5

// Random
[[__optional_args(1)]]
int Random(int max, int min);
[[__optional_args(1)]]
fixed RandomFixed(fixed max, fixed min);
[[__optional_args(1)]]
float RandomFloat(float max, float min);

// Printing
__str StrParam(__str fmt, ...);
__str Language(__str fmt, ...);
void HudMessage(__str fmt, ...);
void HudMessageRainbows(__str fmt, ...);
void Log(__str fmt, ...);

// Utilities
__str StrUpper(__str in);
bool ButtonPressed(struct player_s *p, int bt);
bool ButtonPressedUI(struct player_s *p, int bt);
void *cpyalloc(size_t num, size_t size, void *src);
unsigned StrHash(__str s);
[[__optional_args(2)]]
bool Lith_SetPlayerVelocity(struct player_s *p, fixed velx, fixed vely, fixed velz, bool add, bool setbob);
[[__call("ScriptS")]] int GetTID(int tid, int ptr);
[[__call("ScriptS")]] int GetPlayerNumber(int tid, int ptr);

// Math
accum lerpk(accum a, accum b, accum t);
float lerpf(float a, float b, float t);
float normf(float x, float min, float max);
bool bpcldi(int bx1, int by1, int bx2, int by2, int x2, int y2); // Box / Point collision (integer)
bool l1xcldi(int lx1, int lx2, int x); // Line (1-d) / X collision (integer)
float pymagf(float x, float y);
accum pymagk(accum x, accum y);
float angle2df(float x1, float y1, float x2, float y2);
accum dist2dk(accum x1, accum y1, accum x2, accum y2);
float dist2df(float x1, float y1, float x2, float y2);
accum dist3dk(accum x1, accum y1, accum z1, accum x2, accum y2, accum z2);
int ceilk(accum n);

// Constants
static float const pi  = 3.14159265358979323846f;
static float const pi2 = pi / 2.0f;
static float const tau = pi * 2.0f;

// Types
typedef long long int score_t;

#endif

