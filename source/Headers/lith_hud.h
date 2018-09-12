// Copyright © 2018 Graham Sanderson, all rights reserved.
#ifndef LITH_HUD_H
#define LITH_HUD_H

#include "lith_world.h"

#define Lith_HUD_StaParam(type, ...) \
   __with(static struct type const _a = {__VA_ARGS__}, *const _ap = &_a;)

#define Lith_HUD_WeaponSlots(h, ...) \
   Lith_HUD_StaParam(hud_wsl, __VA_ARGS__) Lith_HUD_WeaponSlots_Impl((h), _ap)

struct hud_wsl
{
   int   ncol[3];
   __str scol;
   int   x;
   int   y;
};

void Lith_HUD_WeaponSlots_Impl(struct player *p, struct hud_wsl const *a);
void Lith_HUD_Score(struct player *p, __str fmt, i96 scr, __str font, __str cr, int x, int xa, int y, int ya);
void Lith_HUD_KeyInd(struct player *p, int x, int y, bool horz, fixed a);
script void Lith_HUD_Log(struct player *p, int cr, int x, int yy);

#endif
