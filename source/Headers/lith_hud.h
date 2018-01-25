// Copyright © 2018 Graham Sanderson, all rights reserved.
#ifndef LITH_HUD_H
#define LITH_HUD_H

#include "lith_world.h"

#define HID(name, n) int const name = h->id; h->id += n

#define Lith_HUD_StaParam(type, ...) \
   __with(static struct type const _a = {__VA_ARGS__}, *const _ap = &_a;)

#define Lith_HUD_WeaponSlots(h, ...) \
   Lith_HUD_StaParam(hud_wsl, __VA_ARGS__) Lith_HUD_WeaponSlots_Impl((h), _ap)

#define Lith_HUD_Score(h, fmt, scr, ...) \
   Lith_HUD_StaParam(hud_scr, __VA_ARGS__) Lith_HUD_Score_Impl((h), StrParam(fmt, Lith_ScoreSep(scr)), _ap)

#define Lith_HUD_KeyInd(h, ...) \
   Lith_HUD_StaParam(hud_key, __VA_ARGS__) Lith_HUD_KeyInd_Impl((h), _ap)

#define Lith_HUD_Log(h, ...) \
   Lith_HUD_StaParam(hud_log, __VA_ARGS__) Lith_HUD_Log_Impl((h), _ap)

#define Lith_HUD_End(h) \
   DebugStat("hud draw: %i/%i ids", h->id - h->beg, h->end - h->beg)

struct hud
{
   struct player *p;
   int beg;
   int end;
   int id;
};

struct hud_log
{
   int cr;
   int x;
   int y;
};

struct hud_key
{
   int x;
   int y;
   bool horz;
   fixed a;
};

struct hud_scr
{
   __str font;
   __str cr;
   fixed x;
   fixed y;
};

struct hud_wsl
{
   int   ncol[3];
   __str scol;
   int   x;
   int   y;
};

void Lith_HUD_Begin(struct hud *h);
void Lith_HUD_Clear(struct hud *h);
void Lith_HUD_WeaponSlots_Impl(struct hud *h, struct hud_wsl const *a);
void Lith_HUD_Score_Impl(struct hud *h, __str scr, struct hud_scr const *a);
void Lith_HUD_KeyInd_Impl(struct hud *h, struct hud_key const *a);
[[__call("ScriptS")]] void Lith_HUD_Log_Impl(struct hud *h, struct hud_log const *a);

#endif
