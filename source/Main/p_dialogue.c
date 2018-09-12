// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_world.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_dialogue.h"

#define Next_I (*++codeptr)
#define Next_S ((__str)Next_I)

#define DoCurCode  goto *cases[*codeptr]
#define DoNextCode goto *cases[Next_I]
#define Done       goto done
#define Op(name) opcase_##name

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

typedef struct dlgoption_s
{
   __str name;
   int  *ptr;
} dlgoption_t;

typedef struct dlgcurstate_s
{
   __str trmPict;
   int   trmActi;
   int   trmTime;
} dlgcurstate_t;

typedef struct dlgvmstate_s
{
   __str text;

   int stk[16];
   int *sptr;

   __str sreg[4];
   int rega;
   int regb;
   int regc;
   int regd;

   int action;

   dlgoption_t option[8];
   int         optNum;
   int         optSel;
   int         concat;

   anonymous
   dlgcurstate_t cur;
   dlgcurstate_t next;
} dlgvmstate_t;

// Extern Objects ------------------------------------------------------------|

struct dlgdef *lmvar dlgdefs;

// Static Functions ----------------------------------------------------------|

//
// Lith_TerminalGUI
//
script
static void Lith_TerminalGUI(gui_state_t *g, struct player *p, dlgvmstate_t *vmstate)
{
   enum {
      // background
      sizex = 480, sizey = 360,
      right  = sizex,
      top    = sizey*.08,
      bottom = sizey*.75,

      midx = right/2, midy = bottom/2,

      // text
      tsizex = 640, tsizey = 400,
      ttop  = tsizey*.08,
      tleft = tsizex/2-32
   };

   __str remote = vmstate->sreg[DSTR_REMOTE] ?
      vmstate->sreg[DSTR_REMOTE] : "<unknown>@raddr.4E19";

   Lith_GUI_Begin(g, sizex, sizey);
   Lith_GUI_UpdateState(g, p);

   // Background
   SetSize(480, 300);
   PrintSprite("lgfx/Terminal/Back.png",   midx,0, 0,1);
   PrintSprite("lgfx/Terminal/Border.png", midx,0, 0,1);
   SetSize(g->w, g->h);

   // Top-left text
   PrintTextStr("SGXLine r4205");
   PrintText("LTRMFONT", CR_RED, 0,1, 0,1);

   // Top-right text
   switch(vmstate->trmActi)
   {
   case TACT_LOGON:
      PrintTextFmt("Opening Connection to %S", remote); break;
   case TACT_LOGOFF:
      PrintTextStr("Disconnecting...");                 break;
   default:
      PrintTextFmt("Remote: %S",               remote); break;
   }
   PrintText("LTRMFONT", CR_RED, right,2, 0,1);

   // Bottom-left text
   PrintTextFmt("<55.883.115.7>");
   PrintText("LTRMFONT", CR_RED, 0,1, bottom,2);

   // Bottom-right text
   switch(vmstate->trmActi)
   {
   case TACT_LOGON:
   case TACT_LOGOFF: PrintTextStr(world.canondate);      break;
   default:          PrintTextStr("Use To Acknowledge"); break;
   }
   PrintText("LTRMFONT", CR_RED, right,2, bottom,2);

   // Contents
   __str pict;

   if(vmstate->trmPict)
      pict = StrParam("lgfx/Terminal/%S.png", vmstate->trmPict);

   switch(vmstate->trmActi)
   {
   case TACT_LOGON:
   case TACT_LOGOFF:
      __with(int y = midy;)
      {
         if(vmstate->text != "")
         {
            PrintTextStr(vmstate->text);
            PrintText("LTRMFONT", CR_WHITE, midx,0, midy + 35,0);
            y -= 10;
         }

         PrintSprite(pict, midx,0, y,0);
      }
      break;

   case TACT_PICT:
      PrintSprite(pict, midx/2,0, midy,0);

      SetSize(tsizex, tsizey);
      SetClipW(tleft, ttop, 300, 300, 300);
      PrintTextStr(vmstate->text);
      PrintText("LTRMFONT", CR_WHITE, tleft,1, ttop,1);
      SetSize(g->w, g->h);
      ClearClip();
      break;

   case TACT_INFO:
      PrintTextStr(vmstate->text);
      PrintText("LTRMFONT", CR_WHITE, midx,0, midy,0);
      break;
   }

   Lith_GUI_End(g, gui_curs_outlineinv);

   if(p->buttons & BT_USE && !(p->old.buttons & BT_USE) && p->old.indialogue)
   {
      ACS_LocalAmbientSound("player/trmswitch", 127);
      vmstate->action = ACT_ACKNOWLEDGE;
      return;
   }
}

//
// Lith_DialogueGUI
//
script
static void Lith_DialogueGUI(gui_state_t *g, struct player *p, dlgvmstate_t *vmstate)
{
   enum {left = 37, top = 75};
   __str icon = StrParam("lgfx/Dialogue/Icon%S.png", vmstate->sreg[DSTR_ICON]);
   __str name = vmstate->sreg[DSTR_NAME];
   __str remo = vmstate->sreg[DSTR_REMOTE];

   Lith_GUI_Begin(g, 320, 240);
   Lith_GUI_UpdateState(g, p);

   PrintSpriteA("lgfx/Dialogue/Back.png", 0,1, 0,1, 0.7);
   PrintSpriteA(icon,                     0,1, 0,1, 0.7);

   PrintTextStr(name);
   PrintText("LHUDFONT", CR_GREEN, 30,1, 35,1);

   SetClipW(left, top, 263, 157, 263);
   PrintTextFmt("\Cd> Remote: %S\n\Cd> Date: %S\n\n\C-%S", remo, world.canontime,
      vmstate->text);
   PrintText("CBIFONT", CR_WHITE, left,1, top,1);
   ClearClip();

   if(vmstate->optNum)
   {
      int y = 220 - (14 * vmstate->optNum);
      for(int i = 0; i < vmstate->optNum; i++, y += 14)
      {
         if(Lith_GUI_Button_Id(g, i, vmstate->option[i].name, 45, y, Pre(btndlgsel)))
         {
            vmstate->action = ACT_SELOPTION;
            vmstate->optSel = i;
         }
      }
   }

   Lith_GUI_End(g, gui_curs_outlineinv);
}

//
// AddText
//
static __str AddText(dlgvmstate_t *vmstate, __str s, bool local)
{
   if(!vmstate->concat) {
      if(local) return StrParam("%S%LS\n", vmstate->text, s);
      else      return StrParam("%S%S\n",  vmstate->text, s);
   } else if(s != "") {
      if(local) return StrParam("%S%LS ", vmstate->text, s);
      else      return StrParam("%S%S ",  vmstate->text, s);
   } else {
      return StrParam("%S\n\n", vmstate->text);
   }
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_TeleportOutEffect
//
script ext("ACS")
void Lith_TeleportOutEffect(struct player *p)
{
   if(!p) p = LocalPlayer;

   ACS_AmbientSound("misc/teleout", 127);

   ACS_SetHudSize(320, 200);
   ACS_SetCameraToTexture(p->tid, "LITHCAM3", 90);

   DrawSpritePlain("lgfx/Terminal/TeleportOut.png", hid_teleportback,
      160.0, 100.0, 1);

   for(int j = 1; j <= 25; j++)
   {
      fixed e = j / 25.f * 30;
      ACS_SetHudSize(320 * e, 240);
      DrawSpriteFade("LITHCAM3", hid_teleport, (int)(160 * e), 120, TS, 0.2);
      ACS_Delay(1);
   }
}

//
// Lith_DialogueVM
//
// Main dialogue VM.
//
script
void Lith_DialogueVM(struct player *p, int num)
{
   if(p->dead || p->indialogue > 1)
      return;

   // Get the dialogue by number.
   struct dlgdef *def;

   for(def = dlgdefs; def && def->num != num; def = def->next);
   if(!def) return;

   p->indialogue++;

   // GUI state
   gui_state_t gst = {};
   gst.gfxprefix = "lgfx/UI_Green/";
   Lith_GUI_Init(&gst);

   // VM state
   dlgvmstate_t vmstate = {};
   vmstate.text = "";
   for(int i = 0; i < 4; i++)
      vmstate.sreg[i] = "";
   vmstate.sptr = &vmstate.stk[0];

   // terminals are numbered negative
   if(num < 0)
   {
      gst.cx = 320;
      gst.cy = 200;
      // TODO: determine mission status
   }
   else
   {
      gst.cx = 320 / 2;
      gst.cy = 200 / 2;
   }

   for(int *codeptr = def->codeV + def->pages[0];;)
   {
      static __label *const cases[] = {
         #define DCD(name) &&opcase_DCD_##name,
         #include "lith_dialogue.h"
      };

      DoCurCode;
   Op(DCD_NOP): DoNextCode;
   Op(DCD_DIE): Done;

   Op(DCD_PUSH_I): *(vmstate.sptr++) = Next_I;       DoNextCode;
   Op(DCD_PUSH_A): *(vmstate.sptr++) = vmstate.rega; DoNextCode;
   Op(DCD_PUSH_B): *(vmstate.sptr++) = vmstate.regb; DoNextCode;
   Op(DCD_PUSH_C): *(vmstate.sptr++) = vmstate.regc; DoNextCode;
   Op(DCD_PUSH_D): *(vmstate.sptr++) = vmstate.regd; DoNextCode;

   Op(DCD_POP):                    --vmstate.sptr ; DoNextCode;
   Op(DCD_POP_A): vmstate.rega = *(--vmstate.sptr); DoNextCode;
   Op(DCD_POP_B): vmstate.regb = *(--vmstate.sptr); DoNextCode;
   Op(DCD_POP_C): vmstate.regc = *(--vmstate.sptr); DoNextCode;
   Op(DCD_POP_D): vmstate.regd = *(--vmstate.sptr); DoNextCode;

   #define Arith(op, r) \
      vmstate.rega op##= r
   #define ArithSet(sfx, r) \
      Op(DCD_ADD_##sfx): Arith(+,  r); DoNextCode; \
      Op(DCD_SUB_##sfx): Arith(-,  r); DoNextCode; \
      Op(DCD_MUL_##sfx): Arith(*,  r); DoNextCode; \
      Op(DCD_DIV_##sfx): Arith(/,  r); DoNextCode; \
      Op(DCD_MOD_##sfx): Arith(%,  r); DoNextCode; \
      Op(DCD_IOR_##sfx): Arith(|,  r); DoNextCode; \
      Op(DCD_AND_##sfx): Arith(&,  r); DoNextCode; \
      Op(DCD_XOR_##sfx): Arith(^,  r); DoNextCode; \
      Op(DCD_LSH_##sfx): Arith(<<, r); DoNextCode; \
      Op(DCD_RSH_##sfx): Arith(>>, r); DoNextCode
   ArithSet(I, Next_I);
   ArithSet(A, vmstate.rega);
   ArithSet(B, vmstate.regb);
   ArithSet(C, vmstate.regc);
   ArithSet(D, vmstate.regd);
   #undef Arith
   #undef ArithSet

   Op(DCD_JPAGE): codeptr = def->codeV + def->pages[Next_I]; DoCurCode;
   Op(DCD_JMP):   codeptr = def->codeV +            Next_I;  DoCurCode;

   #define GenJump(check) \
      __with(void *jmpto = def->codeV + Next_I;) \
         if(check) {codeptr = jmpto; DoCurCode;} \
      DoNextCode
   Op(DCD_JNZ):     GenJump(vmstate.sptr[-1] != 0);
   Op(DCD_JNITEM):  GenJump(!InvNum(Next_S));
   Op(DCD_JNCLASS): GenJump(p->pclass != Next_I);
   #undef GenJump

   Op(DCD_SCRIPTI):
      __with(int s = Next_I, a = Next_I, b = Next_I, c = Next_I, d = Next_I;)
         ACS_ExecuteWithResult(s, a, b, c, d);
      DoNextCode;
   Op(DCD_SCRIPTS):
      __with(__str s = Next_S; int a = Next_I, b = Next_I, c = Next_I, d = Next_I;)
         ACS_NamedExecuteWithResult(s, a, b, c, d);
      DoNextCode;

   Op(DCD_TELEPORT_INTRALEVEL):
      ACS_Delay(5);
      ACS_Teleport(0, Next_I, false);
      Done;
   Op(DCD_TELEPORT_INTERLEVEL):
      ACS_Delay(5);
      Lith_TeleportOutEffect(p);
      ACS_Delay(34);
      ACS_Teleport_NewMap(Next_I, 0, 0);
      Done;

   Op(DCD_TRACE_S): Log("%S",   Next_S      ); DoNextCode;
   Op(DCD_TRACE_A): Log("%.8X", vmstate.rega); DoNextCode;
   Op(DCD_TRACE_B): Log("%.8X", vmstate.regb); DoNextCode;
   Op(DCD_TRACE_C): Log("%.8X", vmstate.regc); DoNextCode;
   Op(DCD_TRACE_D): Log("%.8X", vmstate.regd); DoNextCode;

   Op(DCD_SETSTRING):
      __with(int num = Next_I; __str str = Next_S;)
         vmstate.sreg[num] = str;
      DoNextCode;

   Op(DCD_SETTEXT):
      vmstate.text =                   Next_S;         DoNextCode;
   Op(DCD_SETTEXTLOCAL):
      vmstate.text = StrParam("%LS",   Next_S);        DoNextCode;
   Op(DCD_ADDTEXT):
      vmstate.text = AddText(&vmstate, Next_S, false); DoNextCode;
   Op(DCD_ADDTEXTLOCAL):
      vmstate.text = AddText(&vmstate, Next_S, true);  DoNextCode;

   Op(DCD_CONCAT):
      vmstate.concat++;
      DoNextCode;

   Op(DCD_CONCATEND):
      vmstate.text = StrParam("%S\n", vmstate.text);
      vmstate.concat--;
      DoNextCode;

   Op(DCD_PUTOPT):
      __with(void *jmpto = def->codeV + Next_I;)
      {
         dlgoption_t *option = vmstate.option + vmstate.optNum++;

         option->name = Next_S;
         option->ptr  = ++codeptr;

         codeptr = jmpto;
      }
      DoCurCode;

   Op(DCD_DLGWAIT):
      ACS_LocalAmbientSound("player/cbi/dlgopen", 127);

      p->frozen++;
      p->setVel(0, 0, 0);

      if(vmstate.text != "")
         HudMessageLog("%S", vmstate.text);

      do {
         Lith_DialogueGUI(&gst, p, &vmstate);
         ACS_Delay(1);
      }
      while(vmstate.action == ACT_NONE);

      p->frozen--;
      goto guiact;

   Op(DCD_LOGON):   vmstate.next.trmActi = TACT_LOGON;  goto login;
   Op(DCD_LOGOFF):  vmstate.next.trmActi = TACT_LOGOFF; goto login;
   Op(DCD_INFO):    vmstate.next.trmActi = TACT_INFO;   goto terminal;
   Op(DCD_PICT):    vmstate.next.trmActi = TACT_PICT;   goto pict;
   Op(DCD_TRMWAIT): goto terminal;

   login:
      vmstate.next.trmTime = 42;
   pict:
      vmstate.next.trmPict = Next_S;
   terminal:
      if(vmstate.trmActi != TACT_NONE)
      {
         if(vmstate.trmActi == TACT_LOGON || vmstate.trmActi == TACT_LOGOFF)
            ACS_LocalAmbientSound("player/trmopen", 127);

         bool timer = vmstate.trmTime != 0;
         p->frozen++;
         p->setVel(0, 0, 0);

         if(vmstate.text != "")
            HudMessageLog("%S", vmstate.text);

         do {
            Lith_TerminalGUI(&gst, p, &vmstate);
            ACS_Delay(1);
         }
         while(vmstate.action == ACT_NONE &&
            (!timer || --vmstate.trmTime >= 0));

         p->frozen--;
      }

      vmstate.cur = vmstate.next;
      vmstate.next.trmActi = TACT_NONE;
      vmstate.next.trmTime = 0;
   guiact:
      __with(int action = vmstate.action;)
      {
         vmstate.action = ACT_NONE;
         vmstate.text = "";

         switch(action)
         {
         case ACT_ACKNOWLEDGE: break;
         case ACT_SELOPTION:
            codeptr = vmstate.option[vmstate.optSel].ptr;
            vmstate.optSel = 0;
            vmstate.optNum = 0;
            DoCurCode;
         case ACT_EXIT: Done;
         }
      }
      DoNextCode;
   }

done:
   p->indialogue -= 2;
}

//
// Lith_RunDialogue
//
script ext("ACS")
void Lith_RunDialogue(int num)
{
   withplayer(LocalPlayer) if(!p->indialogue)
   {
      p->dlgnum = num;
      p->indialogue++;
   }
}

//
// Lith_RunTerminal
//
// Runs a numbered terminal.
//
script ext("ACS")
void Lith_RunTerminal(int num)
{
   Lith_RunDialogue(-num);
}

//
// Lith_RunDialogueInt
//
scripti addr(24244) ext("ACS")
void Lith_RunDialogueInt(int num)
{
   Lith_RunDialogue(num);
}

// EOF

