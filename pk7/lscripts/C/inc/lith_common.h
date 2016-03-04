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

#define max(x, y) ((x) < (y) ? (y) : (x))
#define min(x, y) ((x) > (y) ? (y) : (x))
#define minmax(x, mi, ma) (min(max(x, mi), ma))

#define HudMessageParams(flags, id, tr, x, y, hold, ...) \
   ( \
      ACS_OptHudMessage(flags, id, tr, x, y, hold), \
      ACS_EndHudMessage(__VA_ARGS__) \
   )

#define DrawSprite(name, flags, id, tr, x, y, hold, ...) \
   ( \
      ACS_SetFont(name), \
      HudMessage("A"), \
      HudMessageParams(flags, id, tr, x, y, hold, __VA_ARGS__) \
   )

#define HudMessageF(font, ...) \
   ( \
      ACS_SetFont(font), \
      HudMessage(__VA_ARGS__) \
   )

bool ButtonPressed(struct player_s *p, int bt);

[[__optional_args(1)]]
int Random(int max, int min);
[[__optional_args(1)]]
fixed RandomFixed(fixed max, fixed min);
[[__optional_args(1)]]
float RandomFloat(float max, float min);

__str StrParam(__str fmt, ...);
void HudMessage(__str fmt, ...);
void Log(__str fmt, ...);

accum lerpk(accum a, accum b, accum t);
float lerpf(float a, float b, float t);

static float const pi  = 3.14159265358979323846f;
static float const pi2 = pi / 2.0f;
static float const tau = pi * 2.0f;

#endif

