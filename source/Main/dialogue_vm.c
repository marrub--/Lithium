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
#include "dialogue.h"

#define IMM def->codeV[++vm.pc]
#define CUR def->codeV[vm.pc]
#define STR &def->stabV[IMM]
#define JCC goto *cases[CUR]
#define JNC goto *cases[IMM]
#define HLT goto done

/* Types ------------------------------------------------------------------- */

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

enum
{
   SR_N_BIT,
   SR_V_BIT,
   SR_Z_BIT,
   SR_C_BIT,
};

enum
{
   SR_N = 1 << SR_N_BIT,
   SR_V = 1 << SR_V_BIT,
   SR_Z = 1 << SR_Z_BIT,
   SR_C = 1 << SR_C_BIT,
};

struct vmopt
{
   char const *name;
   i32         ptr;
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

   char const *st[4];

   i32 pc; /* program counter */
   i32 ac; /* accumulator */
   i32 x;  /* x register */
   i32 y;  /* y register */
   i32 sr; /* status register */
   i32 sp; /* stack pointer */

   i32 action;

   struct vmopt option[8];
   i32          optNum;
   i32          optSel;
   i32          concat;

   anonymous
   struct vmstate cur;
   struct vmstate next;
};

/* Extern Objects ---------------------------------------------------------- */

struct dlgdef *lmvar dlgdefs;

/* Static Functions -------------------------------------------------------- */

script
static void TerminalGUI(struct gui_state *g, struct player *p, struct vm *vm)
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

   char const *remote = vm->st[DSTR_REMOTE] ?
                        vm->st[DSTR_REMOTE] :
                        "<unknown>@raddr.4E19";

   G_Begin(g, sizex, sizey);
   G_UpdateState(g, p);

   /* Background */
   SetSize(480, 300);
   PrintSprite(sp_Terminal_Back,   480/2,0, 0,1);
   PrintSprite(sp_Terminal_Border, 480/2,0, 0,1);

   /* Top-left text */
   SetSize(tsizex, tsizey);
   PrintText_str(st_term_sgxline, s_smallfnt, CR_RED, 0,1, 0,1);

   /* Top-right text */
   str tpright;
   switch(vm->trmActi) {
   default:          tpright = StrParam("Remote: %s",               remote); break;
   case TACT_LOGON:  tpright = StrParam("Opening Connection to %s", remote); break;
   case TACT_LOGOFF: tpright = st_term_disconnecting;                        break;
   }
   PrintText_str(tpright, s_smallfnt, CR_RED, tright,2, 0,1);

   /* Bottom-left text */
   PrintText_str(st_term_ip, s_smallfnt, CR_RED, 0,1, tbottom,2);

   /* Bottom-right text */
   str btright;
   switch(vm->trmActi)
   {
   case TACT_LOGON:
   case TACT_LOGOFF: btright = l_strdup(CanonTime(ct_date)); break;
   default:          btright = st_term_use_to_ack;           break;
   }

   PrintText_str(btright, s_smallfnt, CR_RED, tright,2, tbottom,2);

   /* Contents */
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

   G_End(g, gui_curs_outlineinv);

   if(p->buttons & BT_USE && !(p->old.buttons & BT_USE) && p->old.indialogue)
   {
      ACS_LocalAmbientSound(ss_player_trmswitch, 127);
      vm->action = ACT_ACKNOWLEDGE;
      return;
   }
}

script
static void DialogueGUI(struct gui_state *g, struct player *p, struct vm *vm)
{
   enum {left = 37, top = 75};

   char const *name = vm->st[DSTR_NAME];
   char const *remo = vm->st[DSTR_REMOTE];
   char icon[32] = ":Dialogue:Icon"; strcat(icon, vm->st[DSTR_ICON]);

   G_Begin(g, 320, 240);
   G_UpdateState(g, p);

   PrintSpriteA(sp_Dialogue_Back, 0,1, 0,1, 0.7);
   PrintSpriteA(l_strdup(icon),   0,1, 0,1, 0.7);

   ACS_BeginPrint();
   PrintChrSt(name);
   PrintText(s_lhudfont, CR_GREEN, 30,1, 35,1);

   SetClipW(left, top, 263, 157, 263);
   PrintTextFmt("\Cd> Remote: %s\n\Cd> Date: %s\n\n\C-%.*s", remo, CanonTime(ct_full), vm->textC, vm->textV);
   PrintText(s_cbifont, CR_WHITE, left,1, top,1);
   ClearClip();

   if(vm->optNum)
   {
      i32 y = 220 - (14 * vm->optNum);
      for(i32 i = 0; i < vm->optNum; i++, y += 14)
         if(G_Button_Id(g, i, vm->option[i].name, 45, y, Pre(btndlgsel)))
      {
         vm->action = ACT_SELOPTION;
         vm->optSel = i;
      }
   }

   G_End(g, gui_curs_outlineinv);
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

/* Extern Functions -------------------------------------------------------- */

/* Main dialogue VM. */
script
void Dlg_Run(struct player *p, i32 num)
{
   if(p->dead || p->indialogue > 1)
      return;

   /* Get the dialogue by number. */
   struct dlgdef *def;

   for(def = dlgdefs; def && def->num != num; def = def->next);
   if(!def) return;

   p->indialogue++;

   /* GUI state */
   struct gui_state gst = {};
   gst.gfxprefix = ":UI_Green:";
   G_Init(&gst);

   /* VM state */
   struct vm vm = {};
   ResetText(&vm);

   /* terminals are numbered negative */
   /* TODO: determine mission status */
   if(num < 0) {gst.cx = 320;     gst.cy = 200;    }
   else        {gst.cx = 320 / 2; gst.cy = 200 / 2;}

   static __label *const cases[] = {
      #define DCD(name) &&opDCD_##name,
      #include "dialogue.h"
   };

   vm.pc = def->pages[0];
   JCC;

opDCD_NOP: JNC;
opDCD_HLT: HLT;

opDCD_PHA: vm.stk[vm.sp++] = vm.ac; JNC;
opDCD_PLA: vm.ac = vm.stk[--vm.sp]; JNC;

opDCD_LDA: vm.ac = IMM; JNC;
opDCD_LDX: vm.x  = IMM; JNC;
opDCD_LDY: vm.y  = IMM; JNC;

opDCD_TAX: vm.x  = vm.ac; JNC;
opDCD_TAY: vm.y  = vm.ac; JNC;
opDCD_TSX: vm.x  = vm.sp; JNC;
opDCD_TXA: vm.ac = vm.x;  JNC;
opDCD_TXS: vm.sp = vm.x;  JNC;
opDCD_TYA: vm.ac = vm.y;  JNC;

#define ArithSet(sfx, r) \
   opDCD_ADD##sfx: vm.ac +=  r; JNC; \
   opDCD_SUB##sfx: vm.ac -=  r; JNC; \
   opDCD_MUL##sfx: vm.ac *=  r; JNC; \
   opDCD_DIV##sfx: vm.ac /=  r; JNC; \
   opDCD_MOD##sfx: vm.ac %=  r; JNC; \
   opDCD_IOR##sfx: vm.ac |=  r; JNC; \
   opDCD_AND##sfx: vm.ac &=  r; JNC; \
   opDCD_XOR##sfx: vm.ac ^=  r; JNC; \
   opDCD_LSH##sfx: vm.ac <<= r; JNC; \
   opDCD_RSH##sfx: vm.ac >>= r; JNC
ArithSet(I, IMM);
#undef ArithSet

opDCD_JPAGE: vm.pc = def->pages[IMM]; JCC;
opDCD_JMP:   vm.pc = IMM;             JCC;

#define GenJump(t) \
   __with(i32 j = IMM;) { \
      if(t) { \
         vm.pc = j; \
         JCC; \
      } \
   } \
   JNC
opDCD_JNZ:     GenJump(vm.stk[vm.sp - 1] != 0);
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

opDCD_TRA: Log("%.8X", vm.ac); JNC;
opDCD_TRX: Log("%.8X", vm.x);  JNC;
opDCD_TRY: Log("%.8X", vm.y);  JNC;
opDCD_TRZ: Log("%s",   STR);   JNC;

opDCD_SETSTRING:
   __with(i32 num = IMM; char const *s = STR;)
      vm.st[num] = s;
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
   __with(i32 j = IMM;)
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
   P_SetVel(p, 0, 0, 0);

   if(vm.textV[0]) HudMessageLog("%.*s", vm.textC, vm.textV);

   do {
      DialogueGUI(&gst, p, &vm);
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
      P_SetVel(p, 0, 0, 0);

      if(vm.textV[0]) HudMessageLog("%.*s", vm.textC, vm.textV);

      do {
         TerminalGUI(&gst, p, &vm);
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
         vm.pc     = vm.option[vm.optSel].ptr;
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

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr("Lith_TeleportOutEffect")
void Sc_TeleportOutEffect(struct player *p)
{
   with_player(LocalPlayer) TeleportOutEffect(p);
}

script_str ext("ACS") addr("Lith_RunDialogue")
void Sc_RunDialogue(i32 num)
{
   with_player(LocalPlayer) if(!p->indialogue)
   {
      p->dlgnum = num;
      p->indialogue++;
   }
}

script_str ext("ACS") addr("Lith_RunTerminal")
void Sc_RunTerminal(i32 num)
{
   with_player(LocalPlayer) if(!p->indialogue)
   {
      p->dlgnum = -num;
      p->indialogue++;
   }
}
#endif

/* EOF */
