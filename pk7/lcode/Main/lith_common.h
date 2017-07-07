#ifndef LITH_COMMON_H
#define LITH_COMMON_H

#if KDEV
typedef void const *__str;
#endif

#pragma GDCC FIXED_LITERAL ON
#pragma GDCC STRENT_LITERAL ON

#include <ACS_ZDoom.h>
#include <stdbool.h>
#include <stdfix.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#include "lith_tycho.h"

#define property __prop
#define prop_arg __arg
#define ifauto(type, name, ...) \
   __with(type name = (__VA_ARGS__);) \
      if(name)

#define countof(a) (sizeof((a)) / sizeof(*(a)))

#define IsSmallNumber(x) ((x) > -0.001 && (x) < 0.001)
#define CloseEnough(x, y) (IsSmallNumber(x - y))

#define sink ACS_Sin
#define cosk ACS_Cos

#define max(x, y) ((x) < (y) ? (y) : (x))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define minmax(x, mi, ma) (min(max(x, mi), ma))

#define Ticker(on, off) ((ACS_Timer() % 35) < 17 ? (on) : (off))

#define Lith_ScriptCall(...) \
   (world.grafZoneEntered ? ACS_ScriptCall(__VA_ARGS__) : 0)

#define Lith_ScriptCallFixed(...) \
   (world.grafZoneEntered ? ACS_ScriptCallFixed(__VA_ARGS__) : 0)

#define Lith_ScriptCallString(...) \
   (world.grafZoneEntered ? ACS_ScriptCallString(__VA_ARGS__) : "")

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

#define HudMessageAlpha(id, x, y, hold, alpha) \
   ( \
      HudMessageParams(HUDMSG_ALPHA, id, CR_UNTRANSLATED, x, y, hold, alpha) \
   )

#define HudMessageAdd(id, x, y, hold, alpha) \
   ( \
      HudMessageParams(HUDMSG_ALPHA | HUDMSG_ADDBLEND, id, CR_UNTRANSLATED, x, y, hold, alpha) \
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

// Types
typedef long fixed fixed64_t;
typedef uint64_t crc64_t;

// Random
[[__optional_args(1)]] float RandomFloat(float max, float min);

// Printing
__str StrParam(__str fmt, ...);
__str Language(__str fmt, ...);
__str LanguageNull(__str fmt, ...);
void HudMessage(__str fmt, ...);
void HudMessageRainbows(__str fmt, ...);
void Log(__str fmt, ...);
void PrintBold(__str fmt, ...);

// Utilities
__str StrUpper(__str in);
unsigned StrHash(char __str_ars const *s);
[[__call("ScriptS"), __optional_args(1)]] int Lith_GetTID(int tid, int ptr);
[[__call("ScriptS"), __optional_args(1)]] int Lith_GetPlayerNumber(int tid, int ptr);
[[__call("ScriptS"), __optional_args(1)]] bool Lith_ValidPointer(int tid, int ptr);
[[__call("ScriptS"), __optional_args(2)]] bool Lith_SetPointer(int tid, int ptr, int assign, int tid2, int ptr2, int flags);
int Lith_CheckActorInventory(int tid, __str item);
void Lith_GiveActorInventory(int tid, __str item, int amount);
__str Lith_ScoreSep(score_t num);
char *Lith_strcpy_str(char *dest, char __str_ars const *src);
int Lith_strcmp_str(char const *s1, char __str_ars const *s2);
[[__optional_args(1)]] crc64_t Lith_CRC64(void const *data, size_t len, crc64_t result);

// Math
fixed lerpk(fixed a, fixed b, fixed t);
fixed64_t lerplk(fixed64_t a, fixed64_t b, fixed64_t t);
float lerpf(float a, float b, float t);
bool bpcldi(int bx1, int by1, int bx2, int by2, int x2, int y2);
int ceilk(fixed n);

// Constants
static float const pi  = 3.14159265358979323846f;
static float const pi2 = pi / 2.0f;
static float const pi4 = pi / 4.0f;
static float const tau = pi * 2.0f;

#endif

