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

#define countof(a) (sizeof((a)) / sizeof(*(a)))

#define IsSmallNumber(x) ((x) > -0.001 && (x) < 0.001)

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
typedef long long int score_t;
typedef long fixed fixed64_t;

// Random
[[__optional_args(1)]] float RandomFloat(float max, float min);

// Printing
__str StrParam(__str fmt, ...);
__str Language(__str fmt, ...);
void HudMessage(__str fmt, ...);
void HudMessageRainbows(__str fmt, ...);
void Log(__str fmt, ...);
void PrintBold(__str fmt, ...);

// Utilities
__str StrUpper(__str in);
bool ButtonPressed(struct player_s *p, int bt);
bool ButtonPressedUI(struct player_s *p, int bt);
void *cpyalloc(size_t num, size_t size, void *src);
unsigned StrHash(__str s);
[[__optional_args(2)]]
bool Lith_SetPlayerVelocity(struct player_s *p, fixed velx, fixed vely, fixed velz, bool add, bool setbob);
[[__call("ScriptS"), __optional_args(1)]] int Lith_GetTID(int tid, int ptr);
[[__call("ScriptS"), __optional_args(1)]] int Lith_GetPlayerNumber(int tid, int ptr);
int Lith_CheckActorInventory(int tid, __str item);
void Lith_GiveActorInventory(int tid, __str item, int amount);
__str Lith_ScoreSep(score_t num);
char *Lith_strcpy_str(char *dest, char __str_ars const *src);
int Lith_strcmp_str(char const *s1, char __str_ars const *s2);

// Math
fixed lerpk(fixed a, fixed b, fixed t);
fixed64_t lerplk(fixed64_t a, fixed64_t b, fixed64_t t);
float lerpf(float a, float b, float t);
float normf(float x, float min, float max);
bool bpcldi(int bx1, int by1, int bx2, int by2, int x2, int y2); // Box / Point collision (integer)
bool l1xcldi(int lx1, int lx2, int x); // Line (1-d) / X collision (integer)
float pymagf(float x, float y);
fixed pymagk(fixed x, fixed y);
float angle2df(float x1, float y1, float x2, float y2);
fixed dist2dk(fixed x1, fixed y1, fixed x2, fixed y2);
float dist2df(float x1, float y1, float x2, float y2);
fixed dist3dk(fixed x1, fixed y1, fixed z1, fixed x2, fixed y2, fixed z2);
int ceilk(fixed n);

// Constants
static float const pi  = 3.14159265358979323846f;
static float const pi2 = pi / 2.0f;
static float const tau = pi * 2.0f;

#endif

