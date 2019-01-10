// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// dialogue_vm.c: Main dialogue VM.

#if LITHIUM
#include "common.h"
#include "w_world.h"
#include "p_player.h"
#include "p_hudid.h"
#include "dialogue.h"

#define IMM *++vm.pc
#define STR &def->stabV[IMM]
#define JCC goto *cases[*vm.pc]
#define JNC goto *cases[IMM]
#define HLT goto done

// Types ---------------------------------------------------------------------|

enum
{
   ACT_NONE,
   ACT_ACKNOWLEDGE,
   ACT_SELOPTION,
   ACT_EXIT,
};

enum
{
   TACT_NONE,
   TACT_LOGON,
   TACT_LOGOFF,
   TACT_INFO,
   TACT_PICT,
};

struct vmopt
{
   char const *name;
   i32        *ptr;
};

struct vmstate
{
   char const *trmPict;
   i32         trmActi;
   i32         trmTime;
};

struct vm
{
   Vec_Decl(char, text);

   i32 stk[16];
   i32 *sp, *pc;

   char const *sr[4]; // String Registers
   i32         ra;    // Integer Registers
   i32         rb;
   i32         rc;
   i32         rd;

   i32 action;

   struct vmopt option[8];
   i32          optNum;
   i32          optSel;
   i32          concat;

   anonymous
   struct vmstate cur;
   struct vmstate next;
};

// Extern Objects ------------------------------------------------------------|

struct dlgdef *lmvar dlgdefs;

// Static Functions ----------------------------------------------------------|

script
static void Lith_TerminalGUI(struct gui_state *g, struct player *p, struct vm *vm)
{
   enum {
      // background
      sizex = 480, sizey = 360,
      right  = sizex,
      top    = sizey*.08,
      bottom = sizey*.75,

      midx = right/2, midy = bottom/2,

      // text
      tsizex  = 320, tsizey = 240,
      tright  = tsizex,
      ttop    = tsizey*.08,
      tbottom = tsizey*.75,
      tleft   = tsizex/2 - 10,

      tmidx = tright/2, tmidy = tbottom/2,
   };

   char const *remote = vm->sr[DSTR_REMOTE] ?
                        vm->sr[DSTR_REMOTE] :
                        "<unknown>@raddr.4E19";

   Lith_GUI_Begin(g, sizex, sizey);
   Lith_GUI_UpdateState(g, p);

   // Background
   SetSize(480, 300);
   PrintSprite(sp_Terminal_Back,   480/2,0, 0,1);
   PrintSprite(sp_Terminal_Border, 480/2,0, 0,1);

   // Top-left text
   SetSize(tsizex, tsizey);
   PrintText_str(st_term_sgxline, s_smallfnt, CR_RED, 0,1, 0,1);

   // Top-right text
   str tpright;
   switch(vm->trmActi) {
   default:          tpright = StrParam("Remote: %s",               remote); break;
   case TACT_LOGON:  tpright = StrParam("Opening Connection to %s", remote); break;
   case TACT_LOGOFF: tpright = st_term_disconnecting;                        break;
   }
   PrintText_str(tpright, s_smallfnt, CR_RED, tright,2, 0,1);

   // Bottom-left text
   PrintText_str(st_term_ip, s_smallfnt, CR_RED, 0,1, tbottom,2);

   // Bottom-right text
   str btright;
   switch(vm->trmActi)
   {
   case TACT_LOGON:
   case TACT_LOGOFF: btright = l_strdup(world.canondate); break;
   default:          btright = st_term_use_to_ack;        break;
   }

   PrintText_str(btright, s_smallfnt, CR_RED, tright,2, tbottom,2);

   // Contents
   SetSize(g->w, g->h);

   char pict[32] = ":Terminal:"; strcat(pict, vm->trmPict);

   switch(vm->trmActi)
   {
   case TACT_LOGON:
   case TACT_LOGOFF:
      __with(i32 y = midy;)
      {
         if(vm->textV)
         {
            SetSize(tsizex, tsizey);
            PrintTextChr(vm->textV, vm->textC);
            PrintText(s_smallfnt, CR_WHITE, tmidx,0, tmidy + 35,0);
            SetSize(g->w, g->h);

            y -= 10;
         }

         PrintSprite(l_strdup(pict), midx,0, y,0);
      }
      break;
   case TACT_PICT:
      PrintSprite(l_strdup(pict), midx/2,0, midy,0);

      SetSize(tsizex, tsizey);
      SetClipW(tleft, ttop, 150, 300, 150);

      PrintTextChr(vm->textV, vm->textC);
      PrintText(s_smallfnt, CR_WHITE, tleft,1, ttop,1);

      SetSize(g->w, g->h);
      ClearClip();
      break;
   case TACT_INFO:
      SetSize(tsizex, tsizey);

      PrintTextChr(vm->textV, vm->textC);
      PrintText(s_smallfnt, CR_WHITE, 2,1, ttop+2,1);

      SetSize(g->w, g->h);
      break;
   }

   Lith_GUI_End(g, gui_curs_outlineinv);

   if(p->buttons & BT_USE && !(p->old.buttons & BT_USE) && p->old.indialogue)
   {
      ACS_LocalAmbientSound(ss_player_trmswitch, 127);
      vm->action = ACT_ACKNOWLEDGE;
      return;
   }
}

script
static void Lith_DialogueGUI(struct gui_state *g, struct player *p, struct vm *vm)
{
   enum {left = 37, top = 75};

   char const *name = vm->sr[DSTR_NAME];
   char const *remo = vm->sr[DSTR_REMOTE];
   char icon[32] = ":Dialogue:Icon"; strcat(icon, vm->sr[DSTR_ICON]);

   Lith_GUI_Begin(g, 320, 240);
   Lith_GUI_UpdateState(g, p);

   PrintSpriteA(sp_Dialogue_Back, 0,1, 0,1, 0.7);
   PrintSpriteA(l_strdup(icon),   0,1, 0,1, 0.7);

   ACS_BeginPrint();
   PrintChrSt(name);
   PrintText(s_lhudfont, CR_GREEN, 30,1, 35,1);

   SetClipW(left, top, 263, 157, 263);
   PrintTextFmt("\Cd> Remote: %s\n\Cd> Date: %s\n\n\C-%.*s", remo, world.canontime, vm->textC, vm->textV);
   PrintText(s_cbifont, CR_WHITE, left,1, top,1);
   ClearClip();

   if(vm->optNum)
   {
      i32 y = 220 - (14 * vm->optNum);
      for(i32 i = 0; i < vm->optNum; i++, y += 14)
         if(Lith_GUI_Button_Id(g, i, vm->option[i].name, 45, y, Pre(btndlgsel)))
      {
         vm->action = ACT_SELOPTION;
         vm->optSel = i;
      }
   }

   Lith_GUI_End(g, gui_curs_outlineinv);
}

static void SetText(struct vm *vm, char const *s)
{
   i32 l = strlen(s) + 1;
   Vec_Resize(vm->text, l);
   memmove(vm->textV, s, l);
}

static void AddText(struct vm *vm, char const *s)
{
   if(s[0])
   {
      i32 l = strlen(s);

      Vec_Grow(vm->text, l + 1);
      vm->textC--;
      Vec_MoveEnd(vm->text, s, l);
      Vec_Next(vm->text) = vm->concat ? ' ' : '\n';
      Vec_Next(vm->text) = '\0';
   }
   else
   {
      Vec_Grow(vm->text, 2);
      vm->textC--;
      Vec_Next(vm->text) = '\n';
      Vec_Next(vm->text) = '\n';
      Vec_Next(vm->text) = '\0';
   }
}

static void PutText(struct vm *vm, char const *s)
{
   i32 l = strlen(s);

   Vec_Grow(vm->text, l);
   vm->textC--;
   Vec_MoveEnd(vm->text, s, l + 1);
}

static void ResetText(struct vm *vm)
{
   Vec_Resize(vm->text, 1);
   vm->textV[0] = '\0';
}

script
static void TeleportOutEffect(struct player *p)
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

// Extern Functions ----------------------------------------------------------|

// Main dialogue VM.
script
void Lith_DialogueVM(struct player *p, i32 num)
{
   if(p->dead || p->indialogue > 1)
      return;

   // Get the dialogue by number.
   struct dlgdef *def;

   for(def = dlgdefs; def && def->num != num; def = def->next);
   if(!def) return;

   p->indialogue++;

   // GUI state
   struct gui_state gst = {};
   gst.gfxprefix = ":UI_Green:";
   Lith_GUI_Init(&gst);

   // VM state
   struct vm vm = {};
   vm.sp = &vm.stk[0];
   ResetText(&vm);

   // terminals are numbered negative
   // TODO: determine mission status
   if(num < 0) {gst.cx = 320;     gst.cy = 200;    }
   else        {gst.cx = 320 / 2; gst.cy = 200 / 2;}

   static __label *const cases[] = {
      #define DCD(name) &&opDCD_##name,
      #include "dialogue.h"
   };

   vm.pc = &def->codeV[def->pages[0]];
   JCC;

opDCD_NOP: JNC;
opDCD_DIE: HLT;

opDCD_PUSH_I: *vm.sp++ = IMM;   JNC;
opDCD_PUSH_A: *vm.sp++ = vm.ra; JNC;
opDCD_PUSH_B: *vm.sp++ = vm.rb; JNC;
opDCD_PUSH_C: *vm.sp++ = vm.rc; JNC;
opDCD_PUSH_D: *vm.sp++ = vm.rd; JNC;

opDCD_POP:            --vm.sp; JNC;
opDCD_POP_A: vm.ra = *--vm.sp; JNC;
opDCD_POP_B: vm.rb = *--vm.sp; JNC;
opDCD_POP_C: vm.rc = *--vm.sp; JNC;
opDCD_POP_D: vm.rd = *--vm.sp; JNC;

#define Arith(op, r) vm.ra op##= r
#define ArithSet(sfx, r) \
   opDCD_ADD_##sfx: Arith(+,  r); JNC; \
   opDCD_SUB_##sfx: Arith(-,  r); JNC; \
   opDCD_MUL_##sfx: Arith(*,  r); JNC; \
   opDCD_DIV_##sfx: Arith(/,  r); JNC; \
   opDCD_MOD_##sfx: Arith(%,  r); JNC; \
   opDCD_IOR_##sfx: Arith(|,  r); JNC; \
   opDCD_AND_##sfx: Arith(&,  r); JNC; \
   opDCD_XOR_##sfx: Arith(^,  r); JNC; \
   opDCD_LSH_##sfx: Arith(<<, r); JNC; \
   opDCD_RSH_##sfx: Arith(>>, r); JNC
ArithSet(I, IMM);
ArithSet(A, vm.ra);
ArithSet(B, vm.rb);
ArithSet(C, vm.rc);
ArithSet(D, vm.rd);
#undef Arith
#undef ArithSet

opDCD_JPAGE: vm.pc = &def->codeV[def->pages[IMM]]; JCC;
opDCD_JMP:   vm.pc = &def->codeV[IMM];             JCC;

#define GenJump(t) \
   __with(i32 *j = &def->codeV[IMM];) if(t) {vm.pc = j; JCC;} JNC
opDCD_JNZ:     GenJump(vm.sp[-1] != 0);
opDCD_JNITEM:  GenJump(!InvNum(l_strdup(STR)));
opDCD_JNCLASS: GenJump(p->pclass != IMM);
#undef GenJump

opDCD_SCRIPTI:
   __with(i32 s = IMM, a = IMM, b = IMM, c = IMM, d = IMM;)
      ACS_ExecuteWithResult(s, a, b, c, d);
   JNC;
opDCD_SCRIPTS:
   __with(char const *s = STR; i32 a = IMM, b = IMM, c = IMM, d = IMM;)
      ACS_NamedExecuteWithResult(l_strdup(s), a, b, c, d);
   JNC;

opDCD_TELEPORT_INTRALEVEL:
   ACS_Delay(5);
   ACS_Teleport(0, IMM, false);
   HLT;
opDCD_TELEPORT_INTERLEVEL:
   ACS_Delay(5);
   TeleportOutEffect(p);
   ACS_Delay(34);
   ACS_Teleport_NewMap(IMM, 0, 0);
   HLT;

opDCD_TRACE_S: Log("%s",   STR);   JNC;
opDCD_TRACE_A: Log("%.8X", vm.ra); JNC;
opDCD_TRACE_B: Log("%.8X", vm.rb); JNC;
opDCD_TRACE_C: Log("%.8X", vm.rc); JNC;
opDCD_TRACE_D: Log("%.8X", vm.rd); JNC;

opDCD_SETSTRING:
   __with(i32 num = IMM; char const *s = STR;)
      vm.sr[num] = s;
   JNC;

opDCD_SETTEXT:      SetText(&vm,    STR ); JNC;
opDCD_SETTEXTLOCAL: SetText(&vm, LC(STR)); JNC;
opDCD_ADDTEXT:      AddText(&vm,    STR ); JNC;
opDCD_ADDTEXTLOCAL: AddText(&vm, LC(STR)); JNC;

opDCD_CONCAT:
   vm.concat++;
   JNC;

opDCD_CONCATEND:
   PutText(&vm, "\n");
   vm.concat--;
   JNC;

opDCD_PUTOPT:
   __with(i32 *j = &def->codeV[IMM];)
   {
      struct vmopt *option = vm.option + vm.optNum++;

      option->name = STR;
      option->ptr  = vm.pc + 1;

      vm.pc = j;
   }
   JCC;

opDCD_DLGWAIT:
   ACS_LocalAmbientSound(ss_player_cbi_dlgopen, 127);

   p->frozen++;
   p->setVel(0, 0, 0);

   if(vm.textV[0]) HudMessageLog("%.*s", vm.textC, vm.textV);

   do {
      Lith_DialogueGUI(&gst, p, &vm);
      ACS_Delay(1);
   } while(vm.action == ACT_NONE);

   p->frozen--;
   goto guiact;

opDCD_LOGON:   vm.next.trmActi = TACT_LOGON;  goto login;
opDCD_LOGOFF:  vm.next.trmActi = TACT_LOGOFF; goto login;
opDCD_INFO:    vm.next.trmActi = TACT_INFO;   goto terminal;
opDCD_PICT:    vm.next.trmActi = TACT_PICT;   goto pict;
opDCD_TRMWAIT: goto terminal;

login:
   vm.next.trmTime = 42;
pict:
   vm.next.trmPict = STR;
terminal:
   if(vm.trmActi != TACT_NONE)
   {
      if(vm.trmActi == TACT_LOGON || vm.trmActi == TACT_LOGOFF)
         ACS_LocalAmbientSound(ss_player_trmopen, 127);

      bool timer = vm.trmTime != 0;
      p->frozen++;
      p->setVel(0, 0, 0);

      if(vm.textV[0]) HudMessageLog("%.*s", vm.textC, vm.textV);

      do {
         Lith_TerminalGUI(&gst, p, &vm);
         ACS_Delay(1);
      } while(vm.action == ACT_NONE && (!timer || --vm.trmTime >= 0));

      p->frozen--;
   }

   vm.cur = vm.next;
   vm.next.trmActi = TACT_NONE;
   vm.next.trmTime = 0;
guiact:
   __with(i32 action = vm.action;)
   {
      vm.action = ACT_NONE;
      ResetText(&vm);

      switch(action)
      {
      case ACT_ACKNOWLEDGE: break;
      case ACT_SELOPTION:
         vm.pc = vm.option[vm.optSel].ptr;
         vm.optSel = 0;
         vm.optNum = 0;
         JCC;
      case ACT_EXIT: HLT;
      }
   }
   JNC;

done:
   Vec_Clear(vm.text);

   p->indialogue -= 2;
}

// Scripts -------------------------------------------------------------------|

script_str ext("ACS") addr("Lith_TeleportOutEffect")
void Sc_TeleportOutEffect(struct player *p)
{
   withplayer(LocalPlayer) TeleportOutEffect(p);
}

script_str ext("ACS") addr("Lith_RunDialogue")
void Sc_RunDialogue(i32 num)
{
   withplayer(LocalPlayer) if(!p->indialogue)
   {
      p->dlgnum = num;
      p->indialogue++;
   }
}

script_str ext("ACS") addr("Lith_RunTerminal")
void Sc_RunTerminal(i32 num)
{
   withplayer(LocalPlayer) if(!p->indialogue)
   {
      p->dlgnum = -num;
      p->indialogue++;
   }
}
#endif

// EOF
