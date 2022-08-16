// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Miscellaneous scripts.                                                   │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"

static
void SetInventory(str item, i32 amount) {
   i32 s = InvNum(item) - amount;
        if(s < 0) InvTake(item, -s);
   else if(s > 0) InvGive(item,  s);
}

static
void SetActorInventory(i32 tid, str item, i32 amount) {
   i32 s = ACS_CheckActorInventory(tid, item) - amount;
        if(s < 0) ACS_TakeActorInventory(tid, item, -s);
   else if(s > 0) ACS_GiveActorInventory(tid, item,  s);
}

void FadeFlash(i32 r, i32 g, i32 b, k32 amount, k32 seconds) {
   ACS_FadeTo(r, g, b, amount, 0.0);
   ACS_FadeTo(r, g, b, 0.0, seconds);
}

script
i32 PtrTID(i32 tid, i32 ptr) {
   if(tid || ptr) {
      ACS_SetActivator(tid, ptr);
   }
   return ACS_ActivatorTID();
}

script
i32 PtrPlayerNumber(i32 tid, i32 ptr) {
   if(tid || ptr) {
      ACS_SetActivator(tid, ptr);
   }
   return ACS_PlayerNumber();
}

script
bool PtrValid(i32 tid, i32 ptr) {
   if(tid || ptr) {
      return ACS_SetActivator(tid, ptr);
   }
   else
      return true;
}

script
bool PtrSet(i32 tid, i32 ptr, i32 assign, i32 tid2, i32 ptr2, i32 flags) {
   if(tid || ptr) {
      ACS_SetActivator(tid, ptr);
   }
   return ACS_SetPointer(assign, tid2, ptr2, flags);
}

i32 PtrInvNum(i32 tid, str item) {
   if(tid == 0) return InvNum(item);
   else         return ACS_CheckActorInventory(tid, item);
}

void PtrInvGive(i32 tid, str item, i32 amount) {
   if(tid == 0) InvGive(item, amount);
   else         ACS_GiveActorInventory(tid, item, amount);
}

void PtrInvTake(i32 tid, str item, i32 amount) {
   if(tid == 0) InvTake(item, amount);
   else         ACS_TakeActorInventory(tid, item, amount);
}

void PtrInvSet(i32 tid, str item, i32 amount) {
   if(tid == 0) SetInventory(item, amount);
   else         SetActorInventory(tid, item, amount);
}

void BeginAngles(i32 x, i32 y) {
   wl.a_cur = 0;
   for(i32 i = 0; i < countof(wl.a_angles); i++) {
      wl.a_angles[i].ang = 0;
      wl.a_angles[i].dst = 0;
   }
   wl.a_x = x;
   wl.a_y = y;
}

k32 AddAngle(i32 x, i32 y) {
   if(wl.a_cur >= countof(wl.a_angles)) {
      return 0;
   }
   struct polar *p = &wl.a_angles[wl.a_cur++];
   *p = ctopol(x - wl.a_x, y - wl.a_y);
   return p->ang;
}

script ext("ACS") addr(lsc_addangle)
void Z_AddAngle(i32 x, i32 y) {
   AddAngle(x, y);
}

script_str ext("ACS") addr(OBJ "EmitScore")
void Z_EmitScore(i32 amount) {
   /* dummied out */
}

script_str ext("ACS") addr(OBJ "EmitEXP")
void Z_EmitEXP(i32 amount) {
   P_Lv_GiveEXP(amount);
}

script static
bool chtf_give_exp(cheat_params_t const params) {
   if(!IsDigit(params[0]) || !IsDigit(params[1]) || !IsDigit(params[2])) {
      return false;
   }
   P_Lv_GiveEXP((params[0] - '0') * 10000 +
                (params[1] - '0') * 1000 +
                (params[2] - '0') * 100);
   return true;
}

struct cheat cht_give_exp = cheat_s("pgtuition", 3, chtf_give_exp, "Tuition granted");

script_str ext("ACS") addr(OBJ "GiveScore")
void Z_GiveScore(i32 score) {
   P_Scr_Give(GetX(0), GetY(0), GetZ(0),
              score * (k64)ACS_RandomFixed(0.7, 1.2),
              false);
}

script_str ext("ACS") addr(OBJ "BoughtItemPickup")
void Z_BoughtItemPickup(i32 id) {
   if(P_None()) return;
   if(id) {
      struct upgrade *upgr = &pl.upgrades[id];

      if(!get_bit(upgr->flags, _ug_owned)) {
         switch(upgr->category) {
         case _uc_body: StartSound(ss_player_pickup_upgrbody, lch_item, CHANF_NOPAUSE, 1.0, ATTN_NONE); break;
         case _uc_weap: StartSound(ss_player_pickup_upgrweap, lch_item, CHANF_NOPAUSE, 1.0, ATTN_NONE); break;
         case _uc_extr: StartSound(ss_player_pickup_upgrextr, lch_item, CHANF_NOPAUSE, 1.0, ATTN_NONE); break;
         }

         P_Upg_SetOwned(upgr);
         P_Upg_Toggle(upgr);
      }
   } else {
      StartSound(ss_player_pickup_item, lch_item, CHANF_NOPAUSE, 1.0, ATTN_NONE);

      pl.itemsbought++;
   }
}

/* EOF */
