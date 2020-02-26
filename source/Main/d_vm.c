/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Main dialogue VM.
 *
 * ---------------------------------------------------------------------------|
 */

#if LITHIUM
#include "common.h"
#include "w_world.h"
#include "p_player.h"
#include "p_hudid.h"
#include "d_vm.h"
#include "d_compile.h"
#include "w_world.h"

/* Types ------------------------------------------------------------------- */

enum {
   R1_S_PC = 0,
   R1_S_SP = 16,
   R1_S_VA = 24,

   R1_M_PC = 0x0000FFFF,
   R1_M_SP = 0x00FF0000,
   R1_M_VA = 0xFF000000,

   R2_S_AC = 0,
   R2_S_RX = 8,
   R2_S_RY = 16,
   R2_S_SR = 24,

   R2_M_AC = 0x000000FF,
   R2_M_RX = 0x0000FF00,
   R2_M_RY = 0x00FF0000,
   R2_M_SR = 0xFF000000,
};

enum {
   SR_C_BIT = 0,
   SR_Z_BIT = 1,
   SR_I_BIT = 2,
   SR_D_BIT = 3,
   SR_B_BIT = 4,
   SR_V_BIT = 6,
   SR_N_BIT = 7,
};

enum {
   SR_C = 1 << SR_C_BIT << R2_S_SR,
   SR_Z = 1 << SR_Z_BIT << R2_S_SR,
   SR_I = 1 << SR_I_BIT << R2_S_SR,
   SR_D = 1 << SR_D_BIT << R2_S_SR,
   SR_B = 1 << SR_B_BIT << R2_S_SR,
   SR_V = 1 << SR_V_BIT << R2_S_SR,
   SR_N = 1 << SR_N_BIT << R2_S_SR,
};

sync typedef void (*vm_action)(struct player *p);

/* Extern Objects ---------------------------------------------------------- */

struct dlg_def lmvar dlgdefs[DNUM_MAX];

struct dcd_info const dcdinfo[0xFF] = {
   #define DCD(n, op, ty) [n] = {#op "." #ty, ADRM_##ty},
   #include "d_vm.h"
};

/* Static Objects ---------------------------------------------------------- */

noinit static Cps_Decl(memory, 0xFFFF);

/* VM state */
static struct gui_state gst;

static u32 r1, r2;

Vec_Decl(char, text, static);

#define ACT(name) sync static void Act##name(struct player *p);
#include "d_vm.h"

static vm_action actions[] = {
   #define ACT(name) &Act##name,
   #include "d_vm.h"
};

static cstr const action_names[] = {
   #define ACT(name) #name,
   #include "d_vm.h"
};

/* Static Functions -------------------------------------------------------- */

/* utilities */
#define SignB1(v) ((v) & 0x0080)
#define SignB2(v) ((v) & 0x8000)

#define WrapB1(p) ((u32)(p) & 0x00FF)
#define WrapB2(p) ((u32)(p) & 0xFFFF)

local i32 SignExtendB1(u32 v) {return SignB1(v) ? (v) | 0xFFFFFF00 : (v);}
local i32 SignExtendB2(u32 v) {return SignB2(v) ? (v) | 0xFFFF0000 : (v);}

/* registers */
local u32 GetPC() {return (r1 & R1_M_PC) >> R1_S_PC;}
local u32 GetSP() {return (r1 & R1_M_SP) >> R1_S_SP;}
local u32 GetVA() {return (r1 & R1_M_VA) >> R1_S_VA;}
local u32 GetAC() {return (r2 & R2_M_AC) >> R2_S_AC;}
local u32 GetRX() {return (r2 & R2_M_RX) >> R2_S_RX;}
local u32 GetRY() {return (r2 & R2_M_RY) >> R2_S_RY;}
local u32 GetSR() {return (r2 & R2_M_SR) >> R2_S_SR;}

#define Set(reg, msk, bit) \
   reg &= ~msk; \
   reg |= v << bit & msk; \
   return (reg & msk) >> bit
local u32 SetPC(u32 v) {Set(r1, R1_M_PC, R1_S_PC);}
local u32 SetSP(u32 v) {Set(r1, R1_M_SP, R1_S_SP);}
local u32 SetVA(u32 v) {Set(r1, R1_M_VA, R1_S_VA);}
local u32 SetAC(u32 v) {Set(r2, R2_M_AC, R2_S_AC);}
local u32 SetRX(u32 v) {Set(r2, R2_M_RX, R2_S_RX);}
local u32 SetRY(u32 v) {Set(r2, R2_M_RY, R2_S_RY);}
local u32 SetSR(u32 v) {Set(r2, R2_M_SR, R2_S_SR);}
#undef Set

local u32 DecSP(u32 n) {u32 x = GetSP(); SetSP(x - n); return x;}

local u32 IncPC(u32 n) {u32 x = GetPC(); SetPC(x + n); return x;}
local u32 IncSP(u32 n) {u32 x = GetSP(); SetSP(x + n); return x;}

/* processor flags */
local bool GetSR_C() {return r2 & SR_C;}
local bool GetSR_Z() {return r2 & SR_Z;}
local bool GetSR_I() {return r2 & SR_I;}
local bool GetSR_D() {return r2 & SR_D;}
local bool GetSR_B() {return r2 & SR_B;}
local bool GetSR_V() {return r2 & SR_V;}
local bool GetSR_N() {return r2 & SR_N;}

local void SetSR_C(bool v) {r2 &= ~SR_C; if(v) r2 |= SR_C;}
local void SetSR_Z(bool v) {r2 &= ~SR_Z; if(v) r2 |= SR_Z;}
local void SetSR_I(bool v) {r2 &= ~SR_I; if(v) r2 |= SR_I;}
local void SetSR_D(bool v) {r2 &= ~SR_D; if(v) r2 |= SR_D;}
local void SetSR_B(bool v) {r2 &= ~SR_B; if(v) r2 |= SR_B;}
local void SetSR_V(bool v) {r2 &= ~SR_V; if(v) r2 |= SR_V;}
local void SetSR_N(bool v) {r2 &= ~SR_N; if(v) r2 |= SR_N;}

local void ModSR_Z(u32 v) {SetSR_Z((v) == 0);}
local void ModSR_N(u32 v) {SetSR_N(SignB1(v));}
local void ModSR_V(u32 ua, u32 ub, u32 ur)
   {SetSR_V(!SignB1(ua ^ ub) && SignB1(ub) != SignB1(ur));}

local void ModSR_ZN(u32 v) {ModSR_Z(v); ModSR_N(v);}

/* direct memory access */
local cstr MemSC_G(u32 p) {return Cps_ExpandNT(memory, p);}
local str  MemSA_G(u32 p) {return Cps_ExpandNT_str(memory, p);}
local u32  MemB1_G(u32 p) {return Cps_GetC(memory, p);}
local u32  MemB2_G(u32 p) {return MemB1_G(p) | (MemB1_G(p + 1) << 8);}
local u32  MemC1_G()      {return MemB1_G(GetPC());}
local u32  MemC2_G()      {return MemB2_G(GetPC());}
local u32  MemI1_G()      {return MemB1_G(IncPC(1));}
local u32  MemI2_G()      {return MemB2_G(IncPC(2));}
local u32  MemIZ_G(u32 p) {return WrapB1(MemI1_G() + p);}
local u32  MemIA_G(u32 p) {return WrapB2(MemI2_G() + p);}

local void MemB1_S(u32 p, u32 v) {Cps_SetC(memory, p, WrapB1(v));}
local void MemB2_S(u32 p, u32 v) {MemB1_S(p, v); MemB1_S(p + 1, v >> 8);}

/* addressed memory access */
local u32 AdrAI_V() {return MemI2_G();}
local u32 AdrAX_V() {return MemIA_G(GetRX());}
local u32 AdrAY_V() {return MemIA_G(GetRY());}
local u32 AdrII_V() {return MemB2_G(MemI2_G());}
local u32 AdrIX_V() {return MemB2_G(MemIZ_G(GetRX()));}
local u32 AdrIY_V() {return WrapB2(MemB2_G(MemI1_G()) + GetRY());}
local u32 AdrRI_V() {return SignExtendB1(MemI1_G());}
local u32 AdrVI_V() {return MemI1_G();}
local u32 AdrZI_V() {return MemI1_G();}
local u32 AdrZX_V() {return MemIZ_G(GetRX());}
local u32 AdrZY_V() {return MemIZ_G(GetRY());}

local u32 AdrAC_G(u32 a, u32 b)
{
   if(b & 2) return GetAC();
   else      return MemB1_G(a);
}

local u32 AdrAI_G() {return MemB1_G(AdrAI_V());}
local u32 AdrAX_G() {return MemB1_G(AdrAX_V());}
local u32 AdrAY_G() {return MemB1_G(AdrAY_V());}
local u32 AdrIX_G() {return MemB1_G(AdrIX_V());}
local u32 AdrIY_G() {return MemB1_G(AdrIY_V());}
local u32 AdrZI_G() {return MemB1_G(AdrZI_V());}
local u32 AdrZX_G() {return MemB1_G(AdrZX_V());}
local u32 AdrZY_G() {return MemB1_G(AdrZY_V());}

local void AdrAC_S(u32 a, u32 b, u32 r)
{
   if(b & 2) SetAC(r);
   else      MemB1_S(a, r);
}

local void AdrAI_S(u32 v) {MemB1_S(AdrAI_V(), v);}
local void AdrAX_S(u32 v) {MemB1_S(AdrAX_V(), v);}
local void AdrAY_S(u32 v) {MemB1_S(AdrAY_V(), v);}
local void AdrIX_S(u32 v) {MemB1_S(AdrIX_V(), v);}
local void AdrIY_S(u32 v) {MemB1_S(AdrIY_V(), v);}
local void AdrZI_S(u32 v) {MemB1_S(AdrZI_V(), v);}
local void AdrZX_S(u32 v) {MemB1_S(AdrZX_V(), v);}
local void AdrZY_S(u32 v) {MemB1_S(AdrZY_V(), v);}

/* stack */
local u32 StaB1_G() {return MemB1_G(STA_BEG + IncSP(1));}
local u32 StaB2_G() {return MemB2_G(STA_BEG + IncSP(2));}

local void StaB1_S(u32 v) {MemB1_S(STA_BEG + DecSP(1),     v);}
local void StaB2_S(u32 v) {MemB2_S(STA_BEG + DecSP(2) - 1, v);}

/* trace */
local void TraceReg()
{
   printf("PC:%04X SP:%02X VA:%02X "
          "AC:%02X RX:%02X RY:%02X SR:%02X\n",
          GetPC(), GetSP(), GetVA(),
          GetAC(), GetRX(), GetRY(), GetSR());
}

/* jumps */
#define JmpVI { \
      /* jump next byte */ \
      u32 next = MemI1_G(); \
      if(dbglevel & log_dlg) { \
         Dlg_WriteCode(def, next, GetPC() - PRG_BEG); \
         putc(' ', stdout); \
         TraceReg(); \
      } \
      goto *cases[next]; \
   }

#define JmpHL { \
      /* jump halt */ \
      goto halt; \
   }

/* VM action auxiliary */
script static void TerminalGUI(struct player *p, u32 tact)
{
   enum {
      /* background */
      sizex = 480, sizey = 360,
      right  = sizex,
      top    = sizey*.08,
      bottom = sizey*.75,

      midx = right/2, midy = bottom/2,

      /* text */
      tsizex  = 320, tsizey = 240,
      tright  = tsizex,
      ttop    = tsizey*.08,
      tbottom = tsizey*.75,
      tleft   = tsizex/2 - 10,

      tmidx = tright/2, tmidy = tbottom/2,
   };

   G_Begin(&gst, sizex, sizey);
   G_UpdateState(&gst, p);

   /* Background */
   SetSize(480, 300);
   PrintSprite(sp_Terminal_Back,   480/2,0, 0,1);
   PrintSprite(sp_Terminal_Border, 480/2,0, 0,1);

   /* Top-left text */
   SetSize(tsizex, tsizey);
   PrintText_str(st_term_sgxline, s_ltrmfont, CR_RED, 0,1, 0,1);

   /* Top-right text */
   u32 tra = MemB2_G(VAR_REMOTEL);
   str tr = tra ? MemSA_G(tra) : s"<unknown>@raddr.4E19";
   switch(tact) {
      default:          tr = StrParam("Remote: %S",               tr); break;
      case TACT_LOGON:  tr = StrParam("Opening Connection to %S", tr); break;
      case TACT_LOGOFF: tr = st_term_disconnecting;                    break;
   }
   PrintText_str(tr, s_ltrmfont, CR_RED, tright,2, 0,1);

   /* Bottom-left text */
   PrintText_str(st_term_ip, s_ltrmfont, CR_RED, 0,1, tbottom,2);

   /* Bottom-right text */
   str br;
   switch(tact) {
      case TACT_LOGON:
      case TACT_LOGOFF: br = l_strdup(CanonTime(ct_date)); break;
      default:          br = st_term_use_to_ack;           break;
   }
   PrintText_str(br, s_ltrmfont, CR_RED, tright,2, tbottom,2);

   /* Contents */
   SetSize(gst.w, gst.h);

   char pict[32] = ":Terminal:"; strcat(pict, MemSC_G(MemB2_G(VAR_PICTL)));

   switch(tact) {
      case TACT_LOGON:
      case TACT_LOGOFF:
         __with(i32 y = midy;) {
            if(textV) {
               SetSize(tsizex, tsizey);
               PrintTextChr(textV, textC);
               PrintText(s_ltrmfont, CR_WHITE, tmidx,0, tmidy + 35,0);
               SetSize(gst.w, gst.h);

               y -= 10;
            }

            PrintSprite(l_strdup(pict), midx,0, y,0);
         }
         break;
      case TACT_PICT:
         PrintSprite(l_strdup(pict), midx/2,0, midy,0);

         SetSize(tsizex, tsizey);
         SetClipW(tleft, ttop, 150, 300, 150);

         PrintTextChr(textV, textC);
         PrintText(s_ltrmfont, CR_WHITE, tleft,1, ttop,1);

         SetSize(gst.w, gst.h);
         ClearClip();
         break;
      case TACT_INFO:
         SetSize(tsizex, tsizey);

         PrintTextChr(textV, textC);
         PrintText(s_ltrmfont, CR_WHITE, 2,1, ttop+2,1);

         SetSize(gst.w, gst.h);
         break;
   }

   G_End(&gst, gui_curs_outlineinv);

   if(p->buttons & BT_USE && !(p->old.buttons & BT_USE) && p->old.dlg.active) {
      ACS_LocalAmbientSound(ss_player_trmswitch, 127);
      MemB1_S(VAR_UACT, UACT_ACKNOWLEDGE);
   }
}

script static void DialogueGUI(struct player *p)
{
   enum {left = 37, top = 75};

   str snam = MemSA_G(MemB2_G(VAR_NAMEL));
   str srem = MemSA_G(MemB2_G(VAR_REMOTEL));
   char icon[32] = ":Dialogue:Icon"; strcat(icon, MemSC_G(MemB2_G(VAR_ICONL)));

   G_Begin(&gst, 320, 240);
   G_UpdateState(&gst, p);

   PrintSpriteA(sp_Dialogue_Back, 0,1, 0,1, 0.7);
   PrintSpriteA(l_strdup(icon),   0,1, 0,1, 0.7);

   PrintTextStr(snam);
   PrintText(s_lhudfont, CR_GREEN, 30,1, 35,1);

   SetClipW(left, top, 263, 157, 263);
   PrintTextFmt("\Cd> Remote: %S\n\Cd> Date: %s\n\n\C-%.*s",
                srem, CanonTime(ct_full), textC, textV);
   PrintText(s_smallfnt, CR_WHITE, left,1, top,1);
   ClearClip();

   u32 oc = MemB1_G(VAR_OPT_CNT);

   if(oc) {
      i32 y = 220 - 14 * oc;

      for(i32 i = 0; i < oc; i++, y += 14) {
         u32 adr = MemB2_G(StructOfs(OPT, NAML, i));

         if(G_Button_Id(&gst, i, MemSC_G(adr), 45, y, Pre(btndlgsel))) {
            MemB1_S(VAR_UACT, UACT_SELOPTION);
            MemB1_S(VAR_OPT_SEL, i);
         }
      }
   }

   G_End(&gst, gui_curs_outlineinv);
}

void GuiAct(void)
{
   u32 action = MemB1_G(VAR_UACT);
   MemB1_S(VAR_UACT, UACT_NONE);

   Vec_Resize(text, 1);
   textV[0] = '\0';

   switch(action) {
      case UACT_ACKNOWLEDGE:
         break;
      case UACT_SELOPTION: {
         u32 sel = MemB1_G(VAR_OPT_SEL);
         u32 adr = MemB2_G(StructOfs(OPT, PTRL, sel));
         SetVA(ACT_JUMP);
         SetPC(adr);
         MemB1_S(VAR_OPT_SEL, 0);
         MemB1_S(VAR_OPT_CNT, 0);
         break;
      }
      case UACT_EXIT:
         SetVA(ACT_HALT);
         break;
   }
}

script static void TeleportOutEffect(struct player *p)
{
   ACS_AmbientSound(ss_misc_teleout, 127);

   ACS_SetHudSize(320, 200);
   ACS_SetCameraToTexture(p->tid, s_LITHCAM3, 90);

   DrawSpritePlain(sp_Terminal_TeleportOut, hid_teleportback, 160.0, 100.0, 1);

   for(i32 j = 1; j <= 25; j++)
   {
      k32 e = j / 25.lk * 30;
      ACS_SetHudSize(320 * e, 240);
      DrawSpriteFade(s_LITHCAM3, hid_teleport, (i32)(160 * e), 120, TS, 0.2);
      ACS_Delay(1);
   }
}

static void SetText(cstr s)
{
   i32 l = strlen(s) + 1;
   Vec_Resize(text, l);
   memmove(textV, s, l);
}

static void AddText(cstr s)
{
   if(s[0]) {
      i32 l = strlen(s);
      Vec_Grow(text, l + 1);
      textC--;
      Vec_MoveEnd(text, s, l);
      Vec_Next(text) = MemB1_G(VAR_CONCAT) ? ' ' : '\n';
      Vec_Next(text) = '\0';
   } else {
      Vec_Grow(text, 2);
      textC--;
      Vec_Next(text) = '\n';
      Vec_Next(text) = '\n';
      Vec_Next(text) = '\0';
   }
}

/* VM actions */
sync static void ActDLG_WAIT(struct player *p)
{
   SetVA(ACT_NONE);

   ACS_LocalAmbientSound(ss_player_cbi_dlgopen, 127);

   p->frozen++;
   P_SetVel(p, 0, 0, 0);

   if(textV[0]) HudMessageLog("%.*s", textC, textV);

   do {
      DialogueGUI(p);
      ACS_Delay(1);
   } while(MemB1_G(VAR_UACT) == UACT_NONE);

   p->frozen--;
   GuiAct();
}

sync static void ActLD_ITEM(struct player *p)
{
   SetVA(ACT_NONE);

   ModSR_ZN(SetAC(InvNum(MemSA_G(MemB2_G(VAR_ADRL)))));
}

sync static void ActLD_OPT(struct player *p)
{
   SetVA(ACT_NONE);

   u32 cnt = MemB1_G(VAR_OPT_CNT);
   MemB1_S(VAR_OPT_CNT, cnt + 1);

   MemB2_S(StructOfs(OPT, NAML, cnt), MemB2_G(VAR_ADRL));
   MemB2_S(StructOfs(OPT, PTRL, cnt), MemB2_G(VAR_RADRL));
}

sync static void ActSCRIPT_I(struct player *p)
{
   SetVA(ACT_NONE);

   u32 s0 = MemB1_G(VAR_SCP0), s1 = MemB1_G(VAR_SCP1);
   u32 s2 = MemB1_G(VAR_SCP2), s3 = MemB1_G(VAR_SCP3);
   u32 s4 = MemB1_G(VAR_SCP4);

   ModSR_ZN(SetAC(ACS_ExecuteWithResult(s0, s1, s2, s3, s4)));
}

sync static void ActSCRIPT_S(struct player *p)
{
   SetVA(ACT_NONE);

   str s0 = MemSA_G(MemB2_G(VAR_ADRL));
   u32 s1 = MemB1_G(VAR_SCP1), s2 = MemB1_G(VAR_SCP2);
   u32 s3 = MemB1_G(VAR_SCP3), s4 = MemB1_G(VAR_SCP4);

   ModSR_ZN(SetAC(ACS_NamedExecuteWithResult(s0, s1, s2, s3, s4)));
}

sync static void ActTELEPORT_INTERLEVEL(struct player *p)
{
   u32 tag = MemB2_G(VAR_ADRL);

   ACS_Delay(5);
   TeleportOutEffect(p);
   ACS_Delay(34);
   ACS_Teleport_NewMap(tag, 0, 0);

   SetVA(ACT_HALT);
}

sync static void ActTELEPORT_INTRALEVEL(struct player *p)
{
   u32 tag = MemB2_G(VAR_ADRL);

   ACS_Delay(5);
   ACS_Teleport(0, tag, false);

   SetVA(ACT_HALT);
}

sync static void ActTEXT_ADDI(struct player *p)
{
   SetVA(ACT_NONE);
   AddText(MemSC_G(MemB2_G(VAR_ADRL)));
}

sync static void ActTEXT_ADDL(struct player *p)
{
   SetVA(ACT_NONE);
   AddText(LC(MemSC_G(MemB2_G(VAR_ADRL))));
}

sync static void ActTEXT_SETI(struct player *p)
{
   SetVA(ACT_NONE);
   SetText(MemSC_G(MemB2_G(VAR_ADRL)));
}

sync static void ActTEXT_SETL(struct player *p)
{
   SetVA(ACT_NONE);
   SetText(LC(MemSC_G(MemB2_G(VAR_ADRL))));
}

sync static void ActTRM_WAIT(struct player *p)
{
   SetVA(ACT_NONE);

   u32 tact = MemB1_G(VAR_TACT);
   MemB1_S(VAR_TACT, TACT_NONE);

   if(tact != TACT_NONE) {
      i32 timer;

      if(tact == TACT_LOGON || tact == TACT_LOGOFF) {
         ACS_LocalAmbientSound(ss_player_trmopen, 127);
         timer = 45;
      } else {
         timer = INT32_MAX;
      }

      p->frozen++;
      P_SetVel(p, 0, 0, 0);

      if(textV[0]) HudMessageLog("%.*s", textC, textV);

      do {
         TerminalGUI(p, tact);
         ACS_Delay(1);
      } while(MemB1_G(VAR_UACT) == UACT_NONE && --timer >= 0);

      p->frozen--;
      GuiAct();
   }
}

/* Extern Functions -------------------------------------------------------- */

/* Main dialogue VM. */
script void Dlg_Run(struct player *p, u32 num)
{
   if(p->dead || p->dlg.active > 1)
      return;

   /* get the dialogue by number */
   register struct dlg_def lmvar *def = &dlgdefs[num];

   p->dlg.active++;

   if(!def->codeV) {
      Log("%s: dialogue %u has no code", __func__, num);
      JmpHL;
   }

   /* GUI state */
   fastmemset(&gst, 0, sizeof gst);
   gst.cx = 320 / 2;
   gst.cy = 200 / 2;
   gst.gfxprefix = ":UI_Green:";
   G_Init(&gst);

   /* VM state */
   register u32 ua, ub, ur;
   register i32 sa, sb, sr;

   r1 = 0;
   r2 = 0;

   Vec_Resize(text, 1);
   textV[0] = '\0';

   /* copy program data into memory */
   for(u32 i = 0; i < def->codeC; i++) memory[PRG_BEG_C + i] = def->codeV[i];
   for(u32 i = 0; i < def->stabC; i++) memory[STR_BEG_C + i] = def->stabV[i];

   if(dbglevel & log_dlg) {
      Log("Dumping segment PRG..");
      Dbg_PrintMemC(&memory[PRG_BEG_C], def->codeC);
      Log("Dumping segment STR..");
      Dbg_PrintMemC(&memory[STR_BEG_C], def->stabC);
   }

   /* copy some constants into memory */
   MemB1_S(VAR_PCLASS, p->pclass);

   static __label *const cases[0xFF] = {
      #define DCD(n, op, ty) [n] = &&op##_##ty,
      #include "d_vm.h"
   };

   /* all right, start the damn VM already! */
   SetPC(PRG_BEG + def->pages[p->dlg.page]);
   JmpVI;

vmaction:
   while(ua = GetVA(), (ua != ACT_NONE && ua < ACT_MAX)) {
	   Dbg_Log(log_dlg, "action %02X %s", ua, action_names[ua]);
      actions[ua](p);
      switch(GetVA()) {
         case ACT_HALT: JmpHL;
         case ACT_JUMP: JmpVI;
      }
   }
   JmpVI;

/* No-op */
NOP_NP:
   JmpVI;

/* Jumps */
branch:
   if(ub) {
      SetPC(GetPC() + sa);
      JmpVI;
   }
   JmpVI;

BRK_NP:
   JmpHL;

JSR_AI:
   StaB2_S(GetPC());
   SetPC(AdrAI_V());
   JmpVI;

JMP_AI:
   SetPC(AdrAI_V());
   JmpVI;

JMP_II:
   SetPC(AdrII_V());
   JmpVI;

JPG_VI:
   SetPC(PRG_BEG + def->pages[AdrVI_V()]);
   JmpVI;

RTI_NP:
   SetSR(StaB1_G());
   SetPC(StaB2_G());
   JmpVI;

RTS_NP:
   SetPC(StaB2_G());
   JmpVI;

BCS_RI: sa = AdrRI_V(), ub = GetSR_C() != 0; goto branch;
BCC_RI: sa = AdrRI_V(), ub = GetSR_C() == 0; goto branch;
BMI_RI: sa = AdrRI_V(), ub = GetSR_N() != 0; goto branch;
BPL_RI: sa = AdrRI_V(), ub = GetSR_N() == 0; goto branch;
BVS_RI: sa = AdrRI_V(), ub = GetSR_V() != 0; goto branch;
BVC_RI: sa = AdrRI_V(), ub = GetSR_V() == 0; goto branch;
BEQ_RI: sa = AdrRI_V(), ub = GetSR_Z() != 0; goto branch;
BNE_RI: sa = AdrRI_V(), ub = GetSR_Z() == 0; goto branch;

/* Comparison */
compare:
   ur = ua - ub;
   SetSR_C(ur & 0x100);
   ModSR_ZN(ur);
   JmpVI;
bit:
   ua = GetAC();
   ModSR_Z(ua & ub);
   SetSR_V(ub & 0x40);
   SetSR_N(ub & 0x80);
   JmpVI;

BIT_AI: ub = AdrAI_G(); goto bit;
BIT_ZI: ub = AdrZI_G(); goto bit;

CMP_AI: ua = GetAC(), ub = AdrAI_G(); goto compare;
CMP_AX: ua = GetAC(), ub = AdrAX_G(); goto compare;
CMP_AY: ua = GetAC(), ub = AdrAY_G(); goto compare;
CMP_IX: ua = GetAC(), ub = AdrIX_G(); goto compare;
CMP_IY: ua = GetAC(), ub = AdrIY_G(); goto compare;
CMP_VI: ua = GetAC(), ub = AdrVI_V(); goto compare;
CMP_ZI: ua = GetAC(), ub = AdrZI_G(); goto compare;
CMP_ZX: ua = GetAC(), ub = AdrZX_G(); goto compare;

CPX_AI: ua = GetRX(), ub = AdrAI_G(); goto compare;
CPX_VI: ua = GetRX(), ub = AdrVI_V(); goto compare;
CPX_ZI: ua = GetRX(), ub = AdrZI_G(); goto compare;

CPY_AI: ua = GetRY(), ub = AdrAI_G(); goto compare;
CPY_VI: ua = GetRY(), ub = AdrVI_V(); goto compare;
CPY_ZI: ua = GetRY(), ub = AdrZI_G(); goto compare;

/* Stack */
PHA_NP: StaB1_S(GetAC());           JmpVI;
PHP_NP: StaB1_S(GetSR());           JmpVI;
PLA_NP: ModSR_ZN(SetAC(StaB1_G())); JmpVI;
PLP_NP: SetSR(StaB1_G());           JmpVI;

/* Flags */
CLC_NP: SetSR_C(0); JmpVI;
CLD_NP: SetSR_D(0); JmpVI;
CLI_NP: SetSR_I(0); JmpVI;
CLV_NP: SetSR_V(0); JmpVI;
SEC_NP: SetSR_C(1); JmpVI;
SED_NP: SetSR_D(1); JmpVI;
SEI_NP: SetSR_I(1); JmpVI;

/* Load */
LDA_AI: ModSR_ZN(SetAC(AdrAI_G())); JmpVI;
LDA_AX: ModSR_ZN(SetRY(AdrAX_G())); JmpVI;
LDA_AY: ModSR_ZN(SetRY(AdrAY_G())); JmpVI;
LDA_IX: ModSR_ZN(SetRY(AdrIX_G())); JmpVI;
LDA_IY: ModSR_ZN(SetRY(AdrIY_G())); JmpVI;
LDA_VI: ModSR_ZN(SetAC(AdrVI_V())); JmpVI;
LDA_ZI: ModSR_ZN(SetAC(AdrZI_G())); JmpVI;
LDA_ZX: ModSR_ZN(SetAC(AdrZX_G())); JmpVI;

LDX_AI: ModSR_ZN(SetRX(AdrAI_G())); JmpVI;
LDX_AY: ModSR_ZN(SetRY(AdrAX_G())); JmpVI;
LDX_VI: ModSR_ZN(SetRX(AdrVI_V())); JmpVI;
LDX_ZI: ModSR_ZN(SetRX(AdrZI_G())); JmpVI;
LDX_ZY: ModSR_ZN(SetRX(AdrZY_G())); JmpVI;

LDY_AI: ModSR_ZN(SetRY(AdrAI_G())); JmpVI;
LDY_AX: ModSR_ZN(SetRY(AdrAX_G())); JmpVI;
LDY_VI: ModSR_ZN(SetRY(AdrVI_V())); JmpVI;
LDY_ZI: ModSR_ZN(SetRY(AdrZI_G())); JmpVI;
LDY_ZX: ModSR_ZN(SetRY(AdrZX_G())); JmpVI;

LDV_AI: SetVA(AdrAI_G()); goto vmaction;
LDV_AX: SetVA(AdrAX_G()); goto vmaction;
LDV_VI: SetVA(AdrVI_V()); goto vmaction;
LDV_ZI: SetVA(AdrZI_G()); goto vmaction;
LDV_ZX: SetVA(AdrZX_G()); goto vmaction;

/* Transfer */
TAX_NP: ModSR_ZN(SetRX(GetAC())); JmpVI;
TAY_NP: ModSR_ZN(SetRY(GetAC())); JmpVI;
TSX_NP: ModSR_ZN(SetRX(GetSP())); JmpVI;
TXA_NP: ModSR_ZN(SetAC(GetRX())); JmpVI;
TXS_NP:          SetSP(GetRX());  JmpVI;
TYA_NP: ModSR_ZN(SetAC(GetRY())); JmpVI;

/* Store */
STA_AI: AdrAI_S(GetAC()); JmpVI;
STA_AX: AdrAX_S(GetAC()); JmpVI;
STA_AY: AdrAY_S(GetAC()); JmpVI;
STA_IX: AdrIX_S(GetAC()); JmpVI;
STA_IY: AdrIY_S(GetAC()); JmpVI;
STA_ZI: AdrZI_S(GetAC()); JmpVI;
STA_ZX: AdrZX_S(GetAC()); JmpVI;

STX_AI: AdrAI_S(GetRX()); JmpVI;
STX_ZI: AdrZI_S(GetRX()); JmpVI;
STX_ZY: AdrZY_S(GetRX()); JmpVI;

STY_AI: AdrAI_S(GetRY()); JmpVI;
STY_ZI: AdrZI_S(GetRY()); JmpVI;
STY_ZX: AdrZX_S(GetRY()); JmpVI;

/* Arithmetic */
adc:
   ua = GetAC();
   ur = ua + ub + GetSR_C();
   SetSR_C(ur & 0x100);
   ModSR_V(ua, ub, ur);
   ModSR_ZN(SetAC(ur));
   JmpVI;
sbc:
   ua = GetAC();
   ur = ua - ub - GetSR_C();
   SetSR_C(!(ub & 0x100));
   ModSR_V(ua, ub, ur);
   ModSR_ZN(SetAC(ur));
   JmpVI;

and: ModSR_ZN(SetAC(GetAC() & ub)); JmpVI;
eor: ModSR_ZN(SetAC(GetAC() ^ ub)); JmpVI;
ora: ModSR_ZN(SetAC(GetAC() | ub)); JmpVI;

asl:
   ur = AdrAC_G(ua, ub);

   SetSR_C(ur & 0x80);
   ur <<= 1;

   AdrAC_S(ua, ub, ur);
   ModSR_ZN(ur);
   JmpVI;
lsr:
   ur = AdrAC_G(ua, ub);

   SetSR_C(ur & 1);
   ur >>= 1;

   AdrAC_S(ua, ub, ur);
   ModSR_ZN(ur);
   JmpVI;
rol:
   ur = AdrAC_G(ua, ub);

   ub |= GetSR_C();
   SetSR_C(ur & 0x80);
   ur <<= 1;
   ur |= ub & 1;

   AdrAC_S(ua, ub, ur);
   ModSR_ZN(ur);
   JmpVI;
ror:
   ur = AdrAC_G(ua, ub);

   ub |= GetSR_C() << 7;
   SetSR_C(ur & 1);
   ur >>= 1;
   ur |= ub & 0x80;

   AdrAC_S(ua, ub, ur);
   ModSR_ZN(ur);
   JmpVI;
dec:
   ub = MemB1_G(ua) - 1;

   MemB1_S(ua, ub);
   ModSR_ZN(ub);
   JmpVI;
inc:
   ub = MemB1_G(ua) + 1;

   MemB1_S(ua, ub);
   ModSR_ZN(ub);
   JmpVI;

ADC_AI: ub = AdrAI_G(); goto adc;
ADC_AX: ub = AdrAX_G(); goto adc;
ADC_AY: ub = AdrAY_G(); goto adc;
ADC_IX: ub = AdrIX_G(); goto adc;
ADC_IY: ub = AdrIY_G(); goto adc;
ADC_VI: ub = AdrVI_V(); goto adc;
ADC_ZI: ub = AdrZI_G(); goto adc;
ADC_ZX: ub = AdrZX_G(); goto adc;

SBC_AI: ub = AdrAI_G(); goto sbc;
SBC_AX: ub = AdrAX_G(); goto sbc;
SBC_AY: ub = AdrAY_G(); goto sbc;
SBC_IX: ub = AdrIX_G(); goto sbc;
SBC_IY: ub = AdrIY_G(); goto sbc;
SBC_VI: ub = AdrVI_V(); goto sbc;
SBC_ZI: ub = AdrZI_G(); goto sbc;
SBC_ZX: ub = AdrZX_G(); goto sbc;

AND_AI: ub = AdrAI_G(); goto and;
AND_AX: ub = AdrAX_G(); goto and;
AND_AY: ub = AdrAY_G(); goto and;
AND_IX: ub = AdrIX_G(); goto and;
AND_IY: ub = AdrIY_G(); goto and;
AND_VI: ub = AdrVI_V(); goto and;
AND_ZI: ub = AdrZI_G(); goto and;
AND_ZX: ub = AdrZX_G(); goto and;

EOR_AI: ub = AdrAI_G(); goto eor;
EOR_AX: ub = AdrAX_G(); goto eor;
EOR_AY: ub = AdrAY_G(); goto eor;
EOR_IX: ub = AdrIX_G(); goto eor;
EOR_IY: ub = AdrIY_G(); goto eor;
EOR_VI: ub = AdrVI_V(); goto eor;
EOR_ZI: ub = AdrZI_G(); goto eor;
EOR_ZX: ub = AdrZX_G(); goto eor;

ORA_AI: ub = AdrAI_G(); goto ora;
ORA_AX: ub = AdrAX_G(); goto ora;
ORA_AY: ub = AdrAY_G(); goto ora;
ORA_IX: ub = AdrIX_G(); goto ora;
ORA_IY: ub = AdrIY_G(); goto ora;
ORA_VI: ub = AdrVI_V(); goto ora;
ORA_ZI: ub = AdrZI_G(); goto ora;
ORA_ZX: ub = AdrZX_G(); goto ora;

ASL_AI: ua = AdrAI_V(); goto asl;
ASL_AX: ua = AdrAX_V(); goto asl;
ASL_NP: ub = 2;         goto asl;
ASL_ZI: ua = AdrZI_V(); goto asl;
ASL_ZX: ua = AdrZX_V(); goto asl;

LSR_AI: ua = AdrAI_V(); goto lsr;
LSR_AX: ua = AdrAX_V(); goto lsr;
LSR_NP: ub = 2;         goto lsr;
LSR_ZI: ua = AdrZI_V(); goto lsr;
LSR_ZX: ua = AdrZX_V(); goto lsr;

ROL_AI: ua = AdrAI_V(); goto rol;
ROL_AX: ua = AdrAX_V(); goto rol;
ROL_NP: ub = 2;         goto rol;
ROL_ZI: ua = AdrZI_V(); goto rol;
ROL_ZX: ua = AdrZX_V(); goto rol;

ROR_AI: ua = AdrAI_V(); goto ror;
ROR_AX: ua = AdrAX_V(); goto ror;
ROR_NP: ub = 2;         goto ror;
ROR_ZI: ua = AdrZI_V(); goto ror;
ROR_ZX: ua = AdrZX_V(); goto ror;

DEC_AI: ua = AdrAI_V(); goto dec;
DEC_AX: ua = AdrAX_V(); goto dec;
DEC_ZI: ua = AdrZI_V(); goto dec;
DEC_ZX: ua = AdrZX_V(); goto dec;

INC_AI: ua = AdrAI_V(); goto inc;
INC_AX: ua = AdrAX_V(); goto inc;
INC_ZI: ua = AdrZI_V(); goto inc;
INC_ZX: ua = AdrZX_V(); goto inc;

DEX_NP: ModSR_ZN(SetRX(GetRX() - 1)); JmpVI;

DEY_NP: ModSR_ZN(SetRY(GetRY() - 1)); JmpVI;

INX_NP: ModSR_ZN(SetRX(GetRX() + 1)); JmpVI;

INY_NP: ModSR_ZN(SetRY(GetRY() + 1)); JmpVI;

/* Trace */
TRR_NP:
   TraceReg();
   JmpVI;

TRS_NP:
   for(u32 i = GetSP() + 1; i <= 0xFF; i++)
      Log("%02X: %02X", i, MemB1_G(STA_BEG + i));
   JmpVI;

TRV_NP:
   for(u32 i = 0; i <= 0xFF; i++)
      Log("%02X: %02X", i, MemB1_G(VAR_BEG + i));
   JmpVI;

TRT_NP:
   Log("%.*s", textC, textV);
   JmpVI;

halt:
   Dbg_Log(log_dlg, "%s: exited", __func__);

   Vec_Clear(text);

   p->dlg.active -= 2;
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr("Lith_TeleportOutEffect")
void Sc_TeleportOutEffect(struct player *p)
{
   with_player(LocalPlayer) TeleportOutEffect(p);
}

script_str ext("ACS") addr("Lith_RunProgram")
void Sc_RunProgram(i32 num)
{
   with_player(LocalPlayer) {
      if(!p->dlg.active) {
         p->dlg.num = DNUM_PRG_BEG + num;
         p->dlg.active++;
      }
   }
}

script_str ext("ACS") addr("Lith_RunDialogue")
void Sc_RunDialogue(i32 num)
{
   with_player(LocalPlayer) {
      if(!p->dlg.active) {
         p->dlg.num = DNUM_DLG_BEG + num;
         p->dlg.page = 0;
         p->dlg.active++;
      }
   }
}

script_str ext("ACS") addr("Lith_RunTerminal")
void Sc_RunTerminal(i32 num)
{
   with_player(LocalPlayer) {
      if(!p->dlg.active) {
         switch(mission) {
            case _unfinished: p->dlg.page = DPAGE_UNFINISHED; break;
            case _finished:   p->dlg.page = DPAGE_FINISHED;   break;
            case _failure:    p->dlg.page = DPAGE_FAILURE;    break;
         }

         p->dlg.num = DNUM_TRM_BEG + num;
         p->dlg.active++;
      }
   }
}
#endif

/* EOF */
