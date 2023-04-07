// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Main dialogue VM.                                                        │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "w_world.h"
#include "p_player.h"
#include "p_hudid.h"
#include "d_vm.h"
#include "d_compile.h"
#include "w_world.h"

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
   SR_C = R2_S_SR,
   SR_Z,
   SR_I,
   SR_D,
   SR_B,
   SR__,
   SR_V,
   SR_N,
};

noinit struct dlg_def dlgdefs[DNUM_MAX];

struct dcd_info const dcdinfo[0xFF] = {
   #define DCD(n, op, ty) [n] = {#op "." #ty, ADRM_##ty},
   #include "d_vm.h"
};

noinit static
cps_t memory[Cps_Size(0xFFFF)];

/* VM state */
noinit static
struct gui_state gst;

noinit static
u32 r1, r2;

static
cstr const action_names[] = {
   #define ACT(name) #name,
   #include "d_vm.h"
};

/* utilities */
#define stk_sta stkoff static

#define SignB1(v) get_bit(v,  7)
#define SignB2(v) get_bit(v, 15)

stk_sta i32 SignExtendB1(u32 v) {return SignB1(v) ? (v) | 0xFFFFFF00 : (v);}
stk_sta i32 SignExtendB2(u32 v) {return SignB2(v) ? (v) | 0xFFFF0000 : (v);}

/* registers */
#define GetPC() ((r1 & R1_M_PC) >> R1_S_PC)
#define GetSP() ((r1 & R1_M_SP) >> R1_S_SP)
#define GetVA() ((r1 & R1_M_VA) >> R1_S_VA)
#define GetAC() ((r2 & R2_M_AC) >> R2_S_AC)
#define GetRX() ((r2 & R2_M_RX) >> R2_S_RX)
#define GetRY() ((r2 & R2_M_RY) >> R2_S_RY)
#define GetSR() ((r2 & R2_M_SR) >> R2_S_SR)
#define Set(reg, msk, bit) \
   statement({ \
      reg &= ~msk; \
      reg |= v << bit & msk; \
      return (reg & msk) >> bit; \
   })
stk_sta u32 SetPC(u32 v) {Set(r1, R1_M_PC, R1_S_PC);}
stk_sta u32 SetSP(u32 v) {Set(r1, R1_M_SP, R1_S_SP);}
stk_sta u32 SetVA(u32 v) {Set(r1, R1_M_VA, R1_S_VA);}
stk_sta u32 SetAC(u32 v) {Set(r2, R2_M_AC, R2_S_AC);}
stk_sta u32 SetRX(u32 v) {Set(r2, R2_M_RX, R2_S_RX);}
stk_sta u32 SetRY(u32 v) {Set(r2, R2_M_RY, R2_S_RY);}
stk_sta u32 SetSR(u32 v) {Set(r2, R2_M_SR, R2_S_SR);}
#undef Set
stk_sta u32 DecSP(u32 n) {u32 x = GetSP(); SetSP(x - n); return x;}
stk_sta u32 IncPC(u32 n) {u32 x = GetPC(); SetPC(x + n); return x;}
stk_sta u32 IncSP(u32 n) {u32 x = GetSP(); SetSP(x + n); return x;}

/* processor flags */
#define GetSR_C() get_bit(r2, SR_C)
#define GetSR_Z() get_bit(r2, SR_Z)
#define GetSR_I() get_bit(r2, SR_I)
#define GetSR_D() get_bit(r2, SR_D)
#define GetSR_B() get_bit(r2, SR_B)
#define GetSR_V() get_bit(r2, SR_V)
#define GetSR_N() get_bit(r2, SR_N)

stk_sta void SetSR_C(bool v) {if(v) set_bit(r2, SR_C); else dis_bit(r2, SR_C);}
stk_sta void SetSR_Z(bool v) {if(v) set_bit(r2, SR_Z); else dis_bit(r2, SR_Z);}
stk_sta void SetSR_I(bool v) {if(v) set_bit(r2, SR_I); else dis_bit(r2, SR_I);}
stk_sta void SetSR_D(bool v) {if(v) set_bit(r2, SR_D); else dis_bit(r2, SR_D);}
stk_sta void SetSR_B(bool v) {if(v) set_bit(r2, SR_B); else dis_bit(r2, SR_B);}
stk_sta void SetSR_V(bool v) {if(v) set_bit(r2, SR_V); else dis_bit(r2, SR_V);}
stk_sta void SetSR_N(bool v) {if(v) set_bit(r2, SR_N); else dis_bit(r2, SR_N);}

stk_sta void ModSR_Z(u32 v) {SetSR_Z((v) == 0);}
stk_sta void ModSR_N(u32 v) {SetSR_N(SignB1(v));}
stk_sta void ModSR_V(u32 ua, u32 ub, u32 ur)
   {SetSR_V(!SignB1(ua ^ ub) && SignB1(ub) != SignB1(ur));}

stk_sta void ModSR_ZN(u32 v) {ModSR_Z(v); ModSR_N(v);}

/* direct memory access */
static cstr MemSC_G(u32 p) {return Cps_ExpandNT(memory, p);}
static str  MemSA_G(u32 p) {return Cps_ExpandNT_str(memory, p);}
static u32  MemB1_G(u32 p) {return Cps_GetC(memory, p);}
static u32  MemB2_G(u32 p) {return MemB1_G(p) | (MemB1_G(p + 1) << 8);}
#define     MemC1_G()      (MemB1_G(GetPC()))
#define     MemC2_G()      (MemB2_G(GetPC()))
#define     MemI1_G()      (MemB1_G(IncPC(1)))
#define     MemI2_G()      (MemB2_G(IncPC(2)))
static u32  MemIZ_G(u32 p) {return byte(MemI1_G() + p);}
static u32  MemIA_G(u32 p) {return word(MemI2_G() + p);}

#define     MemB1_S(p, v)         Cps_SetC(memory, p, byte(v))
static void MemB2_S(u32 p, u32 v) {MemB1_S(p, v); MemB1_S(p + 1, v >> 8);}

/* addressed memory access */
#define AdrAI_V() (MemI2_G())
#define AdrAX_V() (MemIA_G(GetRX()))
#define AdrAY_V() (MemIA_G(GetRY()))
#define AdrII_V() (MemB2_G(MemI2_G()))
#define AdrIX_V() (MemB2_G(MemIZ_G(GetRX())))
#define AdrIY_V() (word(MemB2_G(MemI1_G()) + GetRY()))
#define AdrRI_V() (SignExtendB1(MemI1_G()))
#define AdrVI_V() (MemI1_G())
#define AdrZI_V() (MemI1_G())
#define AdrZX_V() (MemIZ_G(GetRX()))
#define AdrZY_V() (MemIZ_G(GetRY()))

static
u32 AdrAC_G(u32 a, u32 b) {
   if(b & 2) return GetAC();
   else      return MemB1_G(a);
}

#define AdrAI_G() (MemB1_G(AdrAI_V()))
#define AdrAX_G() (MemB1_G(AdrAX_V()))
#define AdrAY_G() (MemB1_G(AdrAY_V()))
#define AdrIX_G() (MemB1_G(AdrIX_V()))
#define AdrIY_G() (MemB1_G(AdrIY_V()))
#define AdrZI_G() (MemB1_G(AdrZI_V()))
#define AdrZX_G() (MemB1_G(AdrZX_V()))
#define AdrZY_G() (MemB1_G(AdrZY_V()))

static
void AdrAC_S(u32 a, u32 b, u32 r) {
   if(b & 2) SetAC(r);
   else      MemB1_S(a, r);
}

static void AdrAI_S(u32 v) {MemB1_S(AdrAI_V(), v);}
static void AdrAX_S(u32 v) {MemB1_S(AdrAX_V(), v);}
static void AdrAY_S(u32 v) {MemB1_S(AdrAY_V(), v);}
static void AdrIX_S(u32 v) {MemB1_S(AdrIX_V(), v);}
static void AdrIY_S(u32 v) {MemB1_S(AdrIY_V(), v);}
static void AdrZI_S(u32 v) {MemB1_S(AdrZI_V(), v);}
static void AdrZX_S(u32 v) {MemB1_S(AdrZX_V(), v);}
static void AdrZY_S(u32 v) {MemB1_S(AdrZY_V(), v);}

/* stack */
#define StaB1_G() (MemB1_G(STA_BEG + IncSP(1)))
#define StaB2_G() (MemB2_G(STA_BEG + IncSP(2) + 1))

static void StaB1_S(u32 v) {MemB1_S(STA_BEG + DecSP(1), v);}
static void StaB2_S(u32 v) {MemB2_S(STA_BEG + DecSP(2) - 1, v);}

/* trace */
#ifndef NDEBUG
static void TraceReg(void) {
   __nprintf("PC:%04X SP:%02X VA:%02X "
             "AC:%02X RX:%02X RY:%02X SR:%02X",
             GetPC(), GetSP(), GetVA(),
             GetAC(), GetRX(), GetRY(), GetSR());
}
#endif

/* jumps */
#define JmpDbg() \
   Dbg_Log(log_dlg, \
      Dlg_WriteCode(def, next, GetPC() - PRG_BEG), \
      _c(' '), \
      TraceReg())

/* jump next byte */
#define JmpVI() \
   statement({ \
      mem_size_t next = MemI1_G(); \
      JmpDbg(); \
      goto *cases[next]; \
   })

/* VM action auxiliary */
static str GetText(void) {
   mem_size_t adr = MemB2_G(VAR_TEXTL);
   cstr       sc  = MemSC_G(adr);
   return adr ? lang(strp(_p(ml.lump), _c('_'), _p(sc))) : snil;
}

static str GetRemote(void) {
   mem_size_t adr = MemB2_G(VAR_REMOTEL);
   cstr       nam = adr ? MemSC_G(adr) : "UNKNOWN";
   return ns(lang(fast_strdup2(LANG "REMOTE_", nam)));
}

static str GetName(void) {
   mem_size_t adr = MemB2_G(VAR_NAMEL);
   cstr       nam = adr ? MemSC_G(adr) : "UNKNOWN";
   return ns(lang(fast_strdup2(LANG "PNAME_", nam)));
}

#define ResetName()   MemB2_S(VAR_NAMEL,   0)
#define ResetRemote() MemB2_S(VAR_REMOTEL, 0)
#define ResetText()   MemB2_S(VAR_TEXTL,   0)
#define ResetMusic()  MemB2_S(VAR_MUSICL,  0)

static
void TerminalGUI(i32 tact) {
   enum {
      /* text */
      twidth  = 640, theigh = 480,
      tright  = twidth,
      ttop    = theigh*0.08,
      tbottom = theigh*0.75,
      tleft   = twidth/2 - 10,
      ttwidth = twidth,
      ttheigh = tbottom - ttop * 2,

      tmidx = tright/2, tmidy = tbottom/2,
   };

   G_Begin(&gst, twidth, theigh);
   G_UpdateState(&gst);

   /* Background */
   PrintRect(0, 0,          twidth, tbottom, 0xFF000000);
   PrintRect(0, 0,          twidth, 12,      0xFF44000C);
   PrintRect(0, tbottom-12, twidth, 12,      0xFF44000C);

   /* Top-left text */
   PrintText_str(ns(lang(sl_term_sgxline)), sf_ltrmfont, CR_RED, 0,1, 0,1);

   /* Top-right text */
   str lng;
   str tr = GetRemote();
   switch(tact) {
   default:         lng = sl_term_remote; goto trmtxt;
   case TACT_LOGON: lng = sl_term_open_connect;
   trmtxt:
      tr = strp(_p(lang(lng)), _p(tr));
      break;
   case TACT_LOGOFF:
      tr = ns(lang(sl_term_disconnecting));
      break;
   }
   PrintText_str(tr, sf_ltrmfont, CR_RED, tright,2, 0,1);

   /* Bottom-left text */
   PrintText_str(ns(lang(sl_term_ip)), sf_ltrmfont, CR_RED, 0,1, tbottom,2);

   /* Bottom-right text */
   str br;
   switch(tact) {
   case TACT_LOGON:
   case TACT_LOGOFF: br = fast_strdup(CanonTime(ct_date, ACS_Timer())); break;
   default:          br = ns(lang(sl_term_use_to_ack));                 break;
   }
   PrintText_str(br, sf_ltrmfont, CR_RED, tright,2, tbottom,2);

   /* Contents */
   char pict[64] = ":Terminal:"; faststrcat(pict, MemSC_G(MemB2_G(VAR_PICTL)));

   switch(tact) {
   case TACT_LOGON:
   case TACT_LOGOFF: {
      i32 y = tmidy;
      str text = GetText();

      if(text) {
         PrintText_str(text, sf_ltrmfont, CR_WHITE, tmidx,0, tmidy + 35,0);
         y -= 10;
      }

      PrintSprite(fast_strdup(pict), tmidx,0, y,0);
      break;
   }
   case TACT_PICT: {
      str text = GetText();

      PrintSprite(fast_strdup(pict), tmidx/2,0, tmidy,0);

      G_Clip(&gst, tleft, ttop, tmidx, ttheigh);

      if(text) {
         PrintText_str(text, sf_ltrmfont, CR_WHITE, tleft,1, ttop,1);
      }

      G_ClipRelease(&gst);
      break;
   }
   case TACT_INFO: {
      str text = GetText();

      G_Clip(&gst, 0, ttop, ttwidth, ttheigh);

      if(text) {
         PrintText_str(text, sf_ltrmfont, CR_WHITE, 2,1, ttop+2,1);
      }

      G_ClipRelease(&gst);
      break;
   }
   }

   G_End(&gst, gui_curs_outlineinv);

   if(P_ButtonPressed(BT_USE) &&
      pl.old.modal == _gui_dlg) {
      AmbientSound(ss_player_trmswitch, 1.0);
      MemB1_S(VAR_UACT, UACT_ACKNOWLEDGE);
   }
}

static
void DialogueGUI(void) {
   enum {left = 37, top = 75, texttop = top + 24};

   str snam = GetName();
   str srem = GetRemote();
   str text = GetText();
   char icon[32] = ":Dialogue:Icon"; faststrcat(icon, MemSC_G(MemB2_G(VAR_ICONL)));

   G_Begin(&gst, 320, 240);
   G_UpdateState(&gst);

   PrintSprite(sp_Dialogue_Back,  0,1, 0,1, _u_alpha, 0.9);
   PrintSprite(fast_strdup(icon), 0,1, 0,1, _u_alpha, 0.9);

   PrintText_str(snam, sf_bigupper, CR_GREEN, 30,1, 35,1);

   G_Clip(&gst, left, top, 257, 150);
   ACS_BeginPrint();
   PrintStrL("\Cd> Remote: ");
   ACS_PrintString(srem);
   PrintStrL("\n\Cd> Date: ");
   PrintStr(CanonTime(ct_full, ACS_Timer()));
   PrintText(sf_lmidfont, CR_WHITE, left,1, top,1);

   if(text) {
      PrintText_str(text, sf_smallfnt, CR_WHITE, left,1, texttop,1);
   }

   G_ClipRelease(&gst);

   mem_size_t oc = MemB1_G(VAR_OPT_CNT);

   if(oc) {
      i32 y = 220 - 14 * oc;

      for(mem_size_t i = 0; i < oc; i++, y += 14) {
         mem_size_t adr = MemB2_G(StructOfs(OPT, NAML, i));
         str        txt = lang(strp(_p(ml.lump), _c('_'), _p(MemSC_G(adr)), _c(P_Char(pl.pclass))));
         if(!txt)  {txt = lang(strp(_p(ml.lump), _c('_'), _p(MemSC_G(adr))));}
         if(G_Button_HId(&gst, i, tmpstr(txt), 45, y, Pre(btndlgsel))) {
            MemB1_S(VAR_UACT, UACT_SELOPTION);
            MemB1_S(VAR_OPT_SEL, i);
         }
      }
   }

   G_End(&gst, gui_curs_outlineinv);
}

static
void GuiAct(void) {
   i32 action = MemB1_G(VAR_UACT);
   MemB1_S(VAR_UACT, UACT_NONE);

   ResetText();

   switch(action) {
   case UACT_ACKNOWLEDGE:
      break;
   case UACT_SELOPTION: {
      mem_size_t sel = MemB1_G(VAR_OPT_SEL);
      mem_size_t adr = MemB2_G(StructOfs(OPT, PTRL, sel));
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

stk_sta void F_drawBack(str bgnd) {
   SetSize(320, 200);
   PrintFill(0xFF000000);
   if(bgnd[0]) {
      PrintSprite(bgnd, 0,1, 0,1);
   }
   SetSize(320, 240);
}

stk_sta void F_drawFade(k32 amount) {
   PrintFill(0x000000 | ((i32)(255.0k * amount) << 24));
}

stk_sta void F_drawText(i32 h, str text) {
   if(text && text[0]) {
      SetClipW(10, 10, 300, h, 300);
      PrintFill(0x7F000000);
      PrintText_str(text, sf_smallfnt, CR_WHITE, 10,1, 10,1);
      ClearClip();
   }
}

/* VM actions */
stk_sta void ActEND_GAME(void) {
   SetVA(ACT_NONE);
   ServCallV(sm_ActuallyEndTheGame);
}

stkcall static void ActLD_ITEM(void) {
   SetVA(ACT_NONE);
   ModSR_ZN(SetAC(InvNum(MemSA_G(MemB2_G(VAR_ADRL)))));
}

stkcall static void ActLD_OPT(void) {
   SetVA(ACT_NONE);
   i32 cnt = MemB1_G(VAR_OPT_CNT);
   MemB1_S(VAR_OPT_CNT, cnt + 1);
   MemB2_S(StructOfs(OPT, NAML, cnt), MemB2_G(VAR_ADRL));
   MemB2_S(StructOfs(OPT, PTRL, cnt), MemB2_G(VAR_RADRL));
}

stkcall static void ActSCRIPT_I(void) {
   SetVA(ACT_NONE);
   i32 s0 = MemB1_G(VAR_SCP0), s1 = MemB1_G(VAR_SCP1),
       s2 = MemB1_G(VAR_SCP2), s3 = MemB1_G(VAR_SCP3),
       s4 = MemB1_G(VAR_SCP4);
   ModSR_ZN(SetAC(ACS_ExecuteWithResult(s0, s1, s2, s3, s4)));
}

stkcall static void ActSCRIPT_S(void) {
   SetVA(ACT_NONE);
   str s0 = MemSA_G(MemB2_G(VAR_ADRL));
   i32 s1 = MemB1_G(VAR_SCP1), s2 = MemB1_G(VAR_SCP2),
       s3 = MemB1_G(VAR_SCP3), s4 = MemB1_G(VAR_SCP4);
   ModSR_ZN(SetAC(ACS_NamedExecuteWithResult(s0, s1, s2, s3, s4)));
}

alloc_aut(0) sync static void ActTELEPORT_INTERLEVEL(void) {
   i32 tag = MemB2_G(VAR_ADRL);
   ACS_Delay(5);
   P_TeleportOut(tag);
   SetVA(ACT_HALT);
}

alloc_aut(0) sync static void ActTELEPORT_INTRALEVEL(void) {
   i32 tag = MemB2_G(VAR_ADRL);
   ACS_Delay(5);
   ACS_Teleport(0, tag, false);
   SetVA(ACT_HALT);
}

alloc_aut(0) sync static void ActDLG_WAIT(void) {
   SetVA(ACT_NONE);
   AmbientSound(ss_player_cbi_dlgopen, 1.0);
   FreezeTime();
   P_SetVel(0, 0, 0);
   do {
      DialogueGUI();
      ACS_Delay(1);
   } while(MemB1_G(VAR_UACT) == UACT_NONE);
   UnfreezeTime();
   GuiAct();
}

alloc_aut(0) sync static void ActTRM_WAIT(void) {
   SetVA(ACT_NONE);

   i32 tact = MemB1_G(VAR_TACT);
   MemB1_S(VAR_TACT, TACT_NONE);

   if(tact != TACT_NONE) {
      i32 timer;

      if(tact == TACT_LOGON || tact == TACT_LOGOFF) {
         AmbientSound(ss_player_trmopen, 1.0);
         timer = 45;
      } else {
         timer = INT32_MAX;
      }

      FreezeTime();
      P_SetVel(0, 0, 0);

      do {
         TerminalGUI(tact);
         ACS_Delay(1);
      } while(MemB1_G(VAR_UACT) == UACT_NONE && --timer >= 0);

      UnfreezeTime();
      GuiAct();
   }
}

alloc_aut(0) sync static void ActFIN_WAIT(void) {
   enum {
      _fill_x = 280,
      _fill_y = 220,
   };
   SetVA(ACT_NONE);
   i32 fact = MemB1_G(VAR_FACT);
   MemB1_S(VAR_FACT, FACT_NONE);
   mem_size_t musi = MemB2_G(VAR_MUSICL);
   if(musi) {
      ACS_SetMusicVolume(1.0k);
      ACS_SetMusic(MemSA_G(musi));
      ResetMusic();
   }
   str bgnd = MemSA_G(MemB2_G(VAR_PICTL));
   i32 tics = MemB2_G(VAR_ADRL);
   switch(fact) {
   case FACT_FADE_IN:
      for(i32 i = tics; i > 0; --i) {
         F_drawBack(bgnd);
         F_drawFade(i / (k32)tics);
         ACS_Delay(1);
      }
      break;
   case FACT_FADE_OUT: {
      str text = GetText();
      for(i32 i = tics; i > 0; --i) {
         F_drawBack(bgnd);
         F_drawText(220, text);
         F_drawFade((tics - i) / (k32)tics);
         ACS_Delay(1);
      }
      ResetText();
      break;
   }
   case FACT_WAIT: {
      str text = GetText();
      for(i32 i = tics; i > 0; --i) {
         F_drawBack(bgnd);
         F_drawText(220, text);
         ACS_Delay(1);
      }
      break;
   }
   case FACT_MUS_FADE:
      for(i32 i = tics; i > 0; --i) {
         F_drawBack(bgnd);
         ACS_SetMusicVolume(i / (k32)tics);
         ACS_Delay(1);
      }
      break;
   case FACT_CRAWL: {
      str text = GetText();
      i32 leng = ACS_StrLen(text);
      static struct gui_fil fil_fill;
      static struct gui_fil fil_skipfill;
      fil_fill.cur     = 0;
      fil_fill.tic     = 75;
      fil_skipfill.cur = 0;
      fil_skipfill.tic = 35;
      for(i32 i = tics; i >= 0; i--) {
         F_drawBack(bgnd);
         i32 p, h;
         if(i > 1) {
            if(ACS_Timer() % 3 == 0 && text[p] != ' ') {
               AmbientSound(ss_player_cbi_keypress, 1.0);
            }
            k32 mul = 1.0 - i / (k32)tics;
            p = leng * mul;
            h = 220  * mul;
         } else {
            p = leng;
            h = 220;
         }
         F_drawText(h, ACS_StrMid(text, 0, p));
         if(tics > 100) {
            ACS_BeginPrint();
            ACS_PrintBind(sc_use);
            _l(", ");
            ACS_PrintBind(sc_attack);
            _c(' ');
            _p(ns(lang(sl_continue_finale)));
            PrintText(sf_smallfnt, CR_WHITE, _fill_x,6, _fill_y,0);
            if(i == 1) {
               if(!G_Filler(_fill_x, _fill_y, &fil_fill,
                            P_ButtonHeld(BT_USE | BT_ATTACK))) {
                  i++;
               }
            } else if(G_Filler(_fill_x, _fill_y, &fil_skipfill,
                               P_ButtonHeld(BT_USE | BT_ATTACK))) {
               i = 2;
            }
         }
         ACS_Delay(1);
      }
      break;
   }
   }
}

/* Main dialogue VM. */
dynam_aut script void Dlg_Run(mem_size_t num) {
   if(pl.dead || pl.modal != _gui_dlg) {
      return;
   }
   /* get the dialogue by number */
   register struct dlg_def *def = &dlgdefs[num];
   if(!def->codeV) {
      PrintErr(_l("dialogue "), _p(num), _l(" has no code"));
      goto halt;
   }
   /* GUI state */
   fastmemset(&gst, 0, sizeof gst);
   gst.cx = 320 / 2;
   gst.cy = 200 / 2;
   gst.gfxprefix = ":UI_Green:";
   /* VM state */
   register u32 ua, ub, ur;
   register i32 sa, sb, sr;
   r1 = 0;
   r2 = 0;
   ResetName();
   ResetRemote();
   ResetText();
   /* copy program data into memory */
   fastmemcpy(&memory[PRG_BEG_C], def->codeV, def->codeC);
   fastmemcpy(&memory[STR_BEG_C], def->stabV, def->stabC);
   Dbg_Log(log_gsinfo,
      _l("--- begin dialogue "),
      ACS_PrintHex(num),
      _l(" ---\nDumping segment PRG...\n"),
      Dbg_PrintMemC(&memory[PRG_BEG_C], def->codeC),
      _l("Dumping segment STR...\n"),
      Dbg_PrintMemC(&memory[STR_BEG_C], def->stabC));
   /* copy some constants into memory */
   MemB1_S(VAR_PCLASS, dst_bit(pl.pclass));
   static
   __label *const cases[0xFF] = {
      #define DCD(n, op, ty) [n] = &&op##_##ty,
      #include "d_vm.h"
   };
   /* all right, start the damn VM already! */
   SetSP(0xFF);
   SetPC(PRG_BEG + def->pages[pl.dlg.page]);
   JmpVI();
vmaction:
   while(ua = GetVA(), (ua != ACT_NONE && ua < ACT_MAX)) {
      Dbg_Log(log_dlg, _l("action "), _p(ua), _c(' '), _p(action_names[ua]));
      switch(ua) {
      #define ACT(name) case ACT_##name: Act##name(); break;
      #include "d_vm.h"
      case ACT_HALT: goto halt;
      case ACT_JUMP: JmpVI();
      }
   }
   JmpVI();
/* No-op */
NOP_NP:
   JmpVI();
/* Jumps */
branch:
   if(ub) {
      SetPC(GetPC() + sa);
      JmpVI();
   }
   JmpVI();
BRK_NP:
   goto halt;
JSR_AI: {
   mem_size_t adr = AdrAI_V();
   StaB2_S(GetPC() - 1);
   SetPC(adr);
   JmpVI();
}
JMP_AI:
   SetPC(AdrAI_V());
   JmpVI();
JMP_II:
   SetPC(AdrII_V());
   JmpVI();
JPG_VI:
   SetPC(PRG_BEG + def->pages[AdrVI_V()]);
   JmpVI();
RTI_NP:
   SetSR(StaB1_G());
   SetPC(StaB2_G());
   JmpVI();
RTS_NP:
   SetPC(StaB2_G() + 1);
   JmpVI();
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
   JmpVI();
bit:
   ua = GetAC();
   ModSR_Z(ua & ub);
   SetSR_V(ub & 0x40);
   SetSR_N(ub & 0x80);
   JmpVI();
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
PHA_NP: StaB1_S(GetAC());           JmpVI();
PHP_NP: StaB1_S(GetSR());           JmpVI();
PLA_NP: ModSR_ZN(SetAC(StaB1_G())); JmpVI();
PLP_NP: SetSR(StaB1_G());           JmpVI();
/* Flags */
CLC_NP: SetSR_C(0); JmpVI();
CLD_NP: SetSR_D(0); JmpVI();
CLI_NP: SetSR_I(0); JmpVI();
CLV_NP: SetSR_V(0); JmpVI();
SEC_NP: SetSR_C(1); JmpVI();
SED_NP: SetSR_D(1); JmpVI();
SEI_NP: SetSR_I(1); JmpVI();
/* Load */
LDA_AI: ModSR_ZN(SetAC(AdrAI_G())); JmpVI();
LDA_AX: ModSR_ZN(SetRY(AdrAX_G())); JmpVI();
LDA_AY: ModSR_ZN(SetRY(AdrAY_G())); JmpVI();
LDA_IX: ModSR_ZN(SetRY(AdrIX_G())); JmpVI();
LDA_IY: ModSR_ZN(SetRY(AdrIY_G())); JmpVI();
LDA_VI: ModSR_ZN(SetAC(AdrVI_V())); JmpVI();
LDA_ZI: ModSR_ZN(SetAC(AdrZI_G())); JmpVI();
LDA_ZX: ModSR_ZN(SetAC(AdrZX_G())); JmpVI();
LDX_AI: ModSR_ZN(SetRX(AdrAI_G())); JmpVI();
LDX_AY: ModSR_ZN(SetRY(AdrAX_G())); JmpVI();
LDX_VI: ModSR_ZN(SetRX(AdrVI_V())); JmpVI();
LDX_ZI: ModSR_ZN(SetRX(AdrZI_G())); JmpVI();
LDX_ZY: ModSR_ZN(SetRX(AdrZY_G())); JmpVI();
LDY_AI: ModSR_ZN(SetRY(AdrAI_G())); JmpVI();
LDY_AX: ModSR_ZN(SetRY(AdrAX_G())); JmpVI();
LDY_VI: ModSR_ZN(SetRY(AdrVI_V())); JmpVI();
LDY_ZI: ModSR_ZN(SetRY(AdrZI_G())); JmpVI();
LDY_ZX: ModSR_ZN(SetRY(AdrZX_G())); JmpVI();
LDV_AI: SetVA(AdrAI_G()); goto vmaction;
LDV_AX: SetVA(AdrAX_G()); goto vmaction;
LDV_VI: SetVA(AdrVI_V()); goto vmaction;
LDV_ZI: SetVA(AdrZI_G()); goto vmaction;
LDV_ZX: SetVA(AdrZX_G()); goto vmaction;
/* Transfer */
TAX_NP: ModSR_ZN(SetRX(GetAC())); JmpVI();
TAY_NP: ModSR_ZN(SetRY(GetAC())); JmpVI();
TSX_NP: ModSR_ZN(SetRX(GetSP())); JmpVI();
TXA_NP: ModSR_ZN(SetAC(GetRX())); JmpVI();
TXS_NP:          SetSP(GetRX());  JmpVI();
TYA_NP: ModSR_ZN(SetAC(GetRY())); JmpVI();
/* Store */
STA_AI: AdrAI_S(GetAC()); JmpVI();
STA_AX: AdrAX_S(GetAC()); JmpVI();
STA_AY: AdrAY_S(GetAC()); JmpVI();
STA_IX: AdrIX_S(GetAC()); JmpVI();
STA_IY: AdrIY_S(GetAC()); JmpVI();
STA_ZI: AdrZI_S(GetAC()); JmpVI();
STA_ZX: AdrZX_S(GetAC()); JmpVI();
STX_AI: AdrAI_S(GetRX()); JmpVI();
STX_ZI: AdrZI_S(GetRX()); JmpVI();
STX_ZY: AdrZY_S(GetRX()); JmpVI();
STY_AI: AdrAI_S(GetRY()); JmpVI();
STY_ZI: AdrZI_S(GetRY()); JmpVI();
STY_ZX: AdrZX_S(GetRY()); JmpVI();
/* Arithmetic */
adc:
   ua = GetAC();
   ur = ua + ub + GetSR_C();
   SetSR_C(ur & 0x100);
   ModSR_V(ua, ub, ur);
   ModSR_ZN(SetAC(ur));
   JmpVI();
sbc:
   ua = GetAC();
   ur = ua - ub - GetSR_C();
   SetSR_C(!(ub & 0x100));
   ModSR_V(ua, ub, ur);
   ModSR_ZN(SetAC(ur));
   JmpVI();
and: ModSR_ZN(SetAC(GetAC() & ub)); JmpVI();
eor: ModSR_ZN(SetAC(GetAC() ^ ub)); JmpVI();
ora: ModSR_ZN(SetAC(GetAC() | ub)); JmpVI();
asl:
   ur = AdrAC_G(ua, ub);
   SetSR_C(ur & 0x80);
   ur <<= 1;
   AdrAC_S(ua, ub, ur);
   ModSR_ZN(ur);
   JmpVI();
lsr:
   ur = AdrAC_G(ua, ub);
   SetSR_C(ur & 1);
   ur >>= 1;
   AdrAC_S(ua, ub, ur);
   ModSR_ZN(ur);
   JmpVI();
rol:
   ur = AdrAC_G(ua, ub);
   ub |= GetSR_C();
   SetSR_C(ur & 0x80);
   ur <<= 1;
   ur |= ub & 1;
   AdrAC_S(ua, ub, ur);
   ModSR_ZN(ur);
   JmpVI();
ror:
   ur = AdrAC_G(ua, ub);
   ub |= GetSR_C() << 7;
   SetSR_C(ur & 1);
   ur >>= 1;
   ur |= ub & 0x80;
   AdrAC_S(ua, ub, ur);
   ModSR_ZN(ur);
   JmpVI();
dec:
   ub = MemB1_G(ua) - 1;
   MemB1_S(ua, ub);
   ModSR_ZN(ub);
   JmpVI();
inc:
   ub = MemB1_G(ua) + 1;
   MemB1_S(ua, ub);
   ModSR_ZN(ub);
   JmpVI();
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
DEX_NP: ModSR_ZN(SetRX(GetRX() - 1)); JmpVI();
DEY_NP: ModSR_ZN(SetRY(GetRY() - 1)); JmpVI();
INX_NP: ModSR_ZN(SetRX(GetRX() + 1)); JmpVI();
INY_NP: ModSR_ZN(SetRY(GetRY() + 1)); JmpVI();
/* Trace */
TRR_NP:
   #ifndef NDEBUG
   ACS_BeginPrint();
   TraceReg();
   EndLogEx(_pri_critical|_pri_nonotify);
   #endif
   JmpVI();
TRS_NP:
   #ifndef NDEBUG
   ACS_BeginPrint();
   for(mem_size_t i = GetSP() + 1; i <= 0xFF; i++) {
      __nprintf("%02X: %02X", i, MemB1_G(STA_BEG + i));
   }
   EndLogEx(_pri_critical|_pri_nonotify);
   #endif
   JmpVI();
TRV_NP:
   #ifndef NDEBUG
   ACS_BeginPrint();
   for(i32 i = 0; i <= 0xFF; i++) {
      __nprintf("%02X: %02X", i, MemB1_G(VAR_BEG + i));
   }
   EndLogEx(_pri_critical|_pri_nonotify);
   #endif
   JmpVI();
halt:
   Dbg_Log(log_dlg, _l(_f), _l(": exited"));
   pl.modal = _gui_none;
}

script_str ext("ACS") addr(OBJ "RunProgram")
void Z_RunProgram(i32 num) {
   if(pl.modal == _gui_none) {
      pl.dlg.num = DNUM_PRG_BEG + num;
      pl.modal = _gui_dlg;
   }
}

script_str ext("ACS") addr(OBJ "RunDialogue")
void Z_RunDialogue(i32 num) {
   if(pl.modal == _gui_none) {
      pl.dlg.num = DNUM_DLG_BEG + num;
      pl.dlg.page = 0;
      pl.modal = _gui_dlg;
   }
}

script_str ext("ACS") addr(OBJ "RunTerminal")
void Z_RunTerminal(i32 num) {
   if(pl.modal == _gui_none) {
      switch(ml.mission) {
      case _mstat_unfinished: pl.dlg.page = DPAGE_UNFINISHED; break;
      case _mstat_finished:   pl.dlg.page = DPAGE_FINISHED;   break;
      case _mstat_failure:    pl.dlg.page = DPAGE_FAILURE;    break;
      }
      pl.dlg.num = DNUM_TRM_BEG + num;
      pl.modal = _gui_dlg;
   }
}

script static
bool chtf_run_prg(cheat_params_t const params) {
   if(!IsDigit(params[0]) || !IsDigit(params[1])) {
      return false;
   }
   i32 p0 = params[0] - '0';
   i32 p1 = params[1] - '0';
   Z_RunProgram(p0 * 10 + p1);
   return true;
}

script static
bool chtf_run_dlg(cheat_params_t const params) {
   if(!IsDigit(params[0]) || !IsDigit(params[1])) {
      return false;
   }
   i32 p0 = params[0] - '0';
   i32 p1 = params[1] - '0';
   Z_RunDialogue(p0 * 10 + p1);
   return true;
}

script static
bool chtf_run_trm(cheat_params_t const params) {
   if(!IsDigit(params[0]) || !IsDigit(params[1])) {
      return false;
   }
   i32 p0 = params[0] - '0';
   i32 p1 = params[1] - '0';
   Z_RunTerminal(p0 * 10 + p1);
   return true;
}

struct cheat cht_run_prg = cheat_s("pgrprg", 2, chtf_run_prg, "Program unleashed");
struct cheat cht_run_dlg = cheat_s("pgrdlg", 2, chtf_run_dlg, "Dialogue unleashed");
struct cheat cht_run_trm = cheat_s("pgrtrm", 2, chtf_run_trm, "Terminal unleashed");

/* EOF */
