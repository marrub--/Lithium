// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
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
#include <stdint.h>

#define strnull (__str)0

#define salloc(t) calloc(1, sizeof(t))

#define ifw(decl, ...) __with(decl;) if(__VA_ARGS__)
#define ifauto(type, name, ...) ifw(type name = (__VA_ARGS__), name)

#define countof(a) (sizeof((a)) / sizeof(*(a)))
#define swap(t, a, b) \
   do { \
      t _tmp = a; \
      a = b; \
      b = _tmp; \
   } while(0)

#define IsSmallNumber(x) ((x) > -0.001 && (x) < 0.001)
#define CloseEnough(x, y) (IsSmallNumber(x - y))
#define LogDebug(level, ...) \
   do if(ACS_GetCVar("__lith_debug_level") & (level)) \
      Log(#level ": " __VA_ARGS__); \
   while(0)

#define max(x, y) ((x) < (y) ? (y) : (x))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define minmax(x, mi, ma) (min(max(x, mi), ma))

#define TickerT(t, on, off) ((ACS_Timer() % 35) < (t) ? (on) : (off))
#define Ticker(on, off) (TickerT(17, on, off))

#define HERMES(...) ACS_ScriptCall("Lith_HERMES", __VA_ARGS__)
#define HERMES_F(...) ACS_ScriptCallFixed("Lith_HERMES", __VA_ARGS__)
#define HERMES_S(...) ACS_ScriptCallString("Lith_HERMES", __VA_ARGS__)

#define URANUS(...) ACS_ScriptCall("Lith_URANUS", __VA_ARGS__)
#define URANUS_F(...) ACS_ScriptCallFixed("Lith_URANUS", __VA_ARGS__)
#define URANUS_S(...) ACS_ScriptCallString("Lith_URANUS", __VA_ARGS__)

#define Lith_IsPaused HERMES("GetPaused")

#define Lith_PausableTick() \
   do ACS_Delay(1); \
   while(Lith_IsPaused)

#define PrintSprite(name, x, xa, y, ya) \
   URANUS("_LPS", name, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)-1.0)

#define PrintSpriteA(name, x, xa, y, ya, a) \
   URANUS("_LPS", name, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)(a))

#define PrintTextFmt(...) StrParamBegin(__VA_ARGS__)
#define PrintTextStr(s) (ACS_BeginPrint(), ACS_PrintString(s))

#define PrintText(font, cr, x, xa, y, ya) \
   URANUS("_LPT", ACS_EndStrParam(), font, cr, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)-1.0)

#define PrintTextA(font, cr, x, xa, y, ya, a) \
   URANUS("_LPT", ACS_EndStrParam(), font, cr, (int)(x), (int)(y), (int)(xa), (int)(ya), (fixed)(a))

#define SetClip(x, y, w, h) \
   URANUS("_LSC", (int)(x), (int)(y), (int)(w), (int)(h), 0)

#define SetClipW(x, y, w, h, ww) \
   URANUS("_LSC", (int)(x), (int)(y), (int)(w), (int)(h), (int)(ww))

#define ClearClip() \
   URANUS("_LSC", 0, 0, 0, 0, 0)

#define SetSize(w, h) \
   URANUS("_LHS", (int)(w), (int)(h))

#define HudMessageLog(...) \
   ( \
      HudMessage(__VA_ARGS__), \
      ACS_OptHudMessage(HUDMSG_NOTWITHFULLMAP|HUDMSG_NOTWITH3DVIEW|HUDMSG_NOTWITHOVERLAYMAP|HUDMSG_LOG, hid_log_throwaway, CR_UNTRANSLATED, 0, 0, TICSECOND), \
      ACS_EndHudMessage() \
   )

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

#define DebugStat(...) \
   (world.dbgLevel & log_devh ? Lith_DebugStat(__VA_ARGS__) : (void)0)

#define DebugNote(...) \
   (world.dbgLevel & log_devh ? Lith_DebugNote(__VA_ARGS__) : (void)0)

#define CallbackDeclareFn(t, name) \
   extern void Lith_CbReg_##name(t cb);

#define CallbackDeclare(t, name) \
   extern t name##Cb[]; \
   extern int name##CbNum; \
   CallbackDeclareFn(t, name)

#ifndef EXTERNAL_CODE
#define CallbackDeclareInternal(t, name) CallbackDeclare(t, name)
#else
#define CallbackDeclareInternal(t, name) CallbackDeclareFn(t, name)
#endif

#define CallbackDefine(t, name) \
   t name##Cb[16]; \
   int name##CbNum; \
   void Lith_CbReg_##name(t cb) {name##Cb[name##CbNum++] = cb;}

#define CallbackRun(t, name, ...) \
   do for(int _cb_i = 0; _cb_i < name##CbNum; _cb_i++) \
      name##Cb[_cb_i](__VA_ARGS__); \
   while(0)

#define CallbackEach(t, name) \
   for(int _cb_i = 0; _cb_i < name##CbNum; _cb_i++) \
      __with(t cb = name##Cb[_cb_i];)

#define CallbackClear(t, name) name##CbNum = 0

#define CallbackRunAndClear(t, name, ...) \
   CallbackRun(t, name, __VA_ARGS__); \
   CallbackClear(t, name)

// Not 1.0 / 35.0 or even 0.028 because ZDoom is stupid.
#define TICSECOND (0.029)
#define TS TICSECOND

// To make pitch values down=0, up=1
#define PITCH_BASE (-0.5)

// π!
#define pi  (3.14159265358979323846f)
#define pi2 (pi / 2.0f)
#define pi4 (pi / 4.0f)
#define tau (pi * 2.0f)

// Types ---------------------------------------------------------------------|

typedef long long int score_t;
typedef long fixed fixed64_t;
typedef uint64_t crc64_t;

struct polar {
   fixed ang;
   fixed dst;
};

struct vec2f {
   float x, y;
};

struct vec2i {
   int x, y;
};

enum {
   log_none,
   log_dev   = 1 << 0,
   log_devh  = 1 << 1,
   log_boss  = 1 << 2,
   log_dmon  = 1 << 3,
   log_dmonV = 1 << 4,
   log_dlg   = 1 << 5,
};

typedef void (*basic_cb_t)(void);

// Printing ------------------------------------------------------------------|

void DrawSprite(__str name, int flags, int id, fixed x, fixed y, fixed hold);
void DrawSpriteX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1);
void DrawSpriteXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2);
void DrawSpriteXXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2, fixed a3);
void DrawSpritePlain(__str name, int id, fixed x, fixed y, fixed hold);
void DrawSpriteAlpha(__str name, int id, fixed x, fixed y, fixed hold, fixed alpha);
void DrawSpriteFade(__str name, int id, fixed x, fixed y, fixed hold, fixed fadetime);
void HudMessage(__str fmt, ...);
void HudMessageRainbows(__str fmt, ...);
void Log(__str fmt, ...);
void PrintBold(__str fmt, ...);

// Strings -------------------------------------------------------------------|

__str StrUpper(__str in);
unsigned StrHash(char __str_ars const *s);
unsigned CStrHash(char const *s);
char *Lith_strcpy_str(char *dest, char __str_ars const *src);
int Lith_strcmp_str(char const *s1, char __str_ars const *s2);
char *Lith_strdup_str(__str s);
__str Lith_ScoreSep(score_t num);
__str Language(__str fmt, ...);
__str LanguageNull(__str fmt, ...);
__str StrParam(__str fmt, ...);
void StrParamBegin(__str fmt, ...);

// Utilities -----------------------------------------------------------------|

void Lith_FadeFlash(int r, int g, int b, fixed amount, fixed seconds);
[[__call("ScriptS"), __optional_args(1)]] int Lith_GetTID(int tid, int ptr);
[[__call("ScriptS"), __optional_args(1)]] int Lith_GetPlayerNumber(int tid, int ptr);
[[__call("ScriptS"), __optional_args(1)]] bool Lith_ValidPointer(int tid, int ptr);
[[__call("ScriptS"), __optional_args(2)]] bool Lith_SetPointer(int tid, int ptr, int assign, int tid2, int ptr2, int flags);
int Lith_CheckActorInventory(int tid, __str item);
void Lith_GiveActorInventory(int tid, __str item, int amount);
void Lith_TakeActorInventory(int tid, __str item, int amount);
void Lith_SetActorInventory (int tid, __str item, int amount);
void Lith_DebugStat(__str fmt, ...);
void Lith_DebugNote(__str fmt, ...);

// Math ----------------------------------------------------------------------|

[[__optional_args(1)]] crc64_t Lith_CRC64(void const *data, size_t len, crc64_t result);
[[__optional_args(1)]] crc64_t Lith_CRC64_str(void __str_ars const *data, size_t len, crc64_t result);
[[__optional_args(1)]] float RandomFloat(float max, float min);
float mag2f(float x, float y);
fixed mag2k(fixed x, fixed y);
fixed lerpk(fixed a, fixed b, fixed t);
fixed64_t lerplk(fixed64_t a, fixed64_t b, fixed64_t t);
float lerpf(float a, float b, float t);
double lerp(double a, double b, double t);
bool aabb(int x, int y, int z, int w, int x2, int y2);
int ceilk(fixed n);
int bzpolyi(int a, int b, float t);
float bzpolyf(float a, float b, float t);
struct vec2i qbezieri(int x1, int y1, int x2, int y2, int x3, int y3, float t);
struct vec2f qbezierf(float x1, float y1, float x2, float y2, float x3, float y3, float t);
struct polar ctopol(fixed x, fixed y);

// Address Space Definitions -------------------------------------------------|

// This doesn't actually cause anything bad to happen. ZDoom expects that all
// modules know the exact size of every array, even if they don't use them
// at all and even account for that. So, we hide this to hide warnings.
#ifndef EXTERNAL_CODE
__addrdef extern __mod_arr lmvar;
__addrdef extern __hub_arr lwvar;
#endif

// Extern Objects ------------------------------------------------------------|

#ifndef EXTERNAL_CODE
extern __str dbgstat[],  dbgnote[];
extern int   dbgstatnum, dbgnotenum;
#endif

#endif

