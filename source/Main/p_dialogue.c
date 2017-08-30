// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_world.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_dialogue.h"

#define NextCode    (*++codeptr)
#define NextCodeStr ((__str)NextCode)

#ifndef Lith_NoDynamicGoto
   #define DoCurCode  goto *cases[*codeptr]
   #define DoNextCode goto *cases[NextCode]
   #define Done       goto done
   #define Op(name) opcase_##name
#else
   #define DoCurCode  goto docurcode
   #define DoNextCode codeptr++; goto docurcode
   #define Done       goto done
   #define Op(name) case name
#endif

// Types ---------------------------------------------------------------------|

enum
{
   GACT_NONE,
   GACT_ACKNOWLEDGE,
   GACT_SELOPTION,
   GACT_EXIT,
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
   __str trmpict;
   int trmaction;
   int trmtimer;
} dlgcurstate_t;

typedef struct dlgvmstate_s
{
   int stk[32], *sptr;
   __str text;
   __str strings[4];
   dlgoption_t options[8];
   int guiaction;
   int numoptions;
   int seloption;

   [[__anonymous]] dlgcurstate_t cur;
   dlgcurstate_t next;
} dlgvmstate_t;

// Extern Objects ------------------------------------------------------------|

dlgdef_t *lmvar dlgdefs;

// Static Functions ----------------------------------------------------------|

//
// Lith_TerminalGUI
//
[[__call("ScriptS")]]
static void Lith_TerminalGUI(gui_state_t *g, player_t *p, dlgvmstate_t *vmstate)
{
   int const sizetx  = 640;
   int const sizety  = 400;
   int const sizex   = 480;
   int const sizey   = 300;
   int const right   = sizex;
   int const bottom  = sizey*.75;
   int const top     = sizey*.08;
   int const middlex = right/2;
   int const middley = bottom/2;
   int const middleleftx = middlex/2;
   int const topt    = sizety*.08;
   int const leftt   = sizetx/2-32;

   __str remote = vmstate->strings[DSTR_REMOTE] ? vmstate->strings[DSTR_REMOTE] : "<unknown>@raddr.4E19";

   Lith_GUI_Begin(g, hid_end_dialogue, sizex, sizey);
   Lith_GUI_UpdateState(g, p);

   // Background
   DrawSpritePlain("lgfx/Terminal/Back.png",   g->hid--, middlex, 0.1, TICSECOND);
   DrawSpritePlain("lgfx/Terminal/Border.png", g->hid--, middlex, 0.1, TICSECOND);

   // Top-left text
   HudMessageF("LTRMFONT", "SGXLine r4205");
   HudMessageParams(0, g->hid--, CR_RED, 0.1, 0.1, TICSECOND);

   // Top-right text
   switch(vmstate->trmaction)
   {
   case TACT_LOGON:  HudMessageF("LTRMFONT", "Opening Connection to %S", remote); break;
   case TACT_LOGOFF: HudMessageF("LTRMFONT", "Disconnecting...");                 break;
   default:          HudMessageF("LTRMFONT", "Remote: %S",               remote); break;
   }

   HudMessageParams(0, g->hid--, CR_RED, right+.2, 0.1, TICSECOND);

   // Bottom-left text
   HudMessageF("LTRMFONT", "<55.883.115.7>");
   HudMessageParams(0, g->hid--, CR_RED, 0.1, bottom+.2, TICSECOND);

   // Bottom-right text
   switch(vmstate->trmaction)
   {
   case TACT_LOGON:
   case TACT_LOGOFF: HudMessageF("LTRMFONT", "%S", world.canondate); break;
   default:          HudMessageF("LTRMFONT", "Use To Acknowledge");  break;
   }

   HudMessageParams(0, g->hid--, CR_RED, right+.2, bottom+.2, TICSECOND);

   // Contents
   switch(vmstate->trmaction)
   {
   case TACT_LOGON:
   case TACT_LOGOFF:
      __with(int y = middley;)
      {
         if(vmstate->text != "")
         {
            HudMessageF("LTRMFONT", "%S", vmstate->text);
            HudMessagePlain(g->hid--, middlex, middley + 35, TICSECOND);
            y -= 10;
         }

         DrawSpritePlain(StrParam("lgfx/Terminal/%S.png", vmstate->trmpict), g->hid--, middlex, y, TICSECOND);
      }
      break;
   case TACT_PICT:
      DrawSpritePlain(StrParam("lgfx/Terminal/%S.png", vmstate->trmpict), g->hid--, middleleftx, middley, TICSECOND);
      ACS_SetHudSize(sizetx, sizety, false);
      ACS_SetHudClipRect(leftt, topt, 300, 300, 300);
      HudMessageF("LTRMFONT", "%S", vmstate->text);
      HudMessagePlain(g->hid--, leftt+.1, topt+.1, TICSECOND);
      ACS_SetHudSize(g->w, g->h, false);
      ACS_SetHudClipRect(0, 0, 0, 0);
      break;
   case TACT_INFO:
      HudMessageF("LTRMFONT", "%S", vmstate->text);
      HudMessagePlain(g->hid--, middlex, middley, TICSECOND);
      break;
   }

   Lith_GUI_End(g);

   if(p->buttons & BT_USE && !(p->old.buttons & BT_USE) && p->old.indialogue)
   {
      ACS_LocalAmbientSound("player/trmswitch", 127);
      vmstate->guiaction = GACT_ACKNOWLEDGE;
      return;
   }
}

//
// Lith_DialogueGUI
//
[[__call("ScriptS")]]
static void Lith_DialogueGUI(gui_state_t *g, player_t *p, dlgvmstate_t *vmstate)
{
   Lith_GUI_Begin(g, hid_end_dialogue, 320, 240);
   Lith_GUI_UpdateState(g, p);

   DrawSpriteAlpha("lgfx/Dialogue/Back.png", g->hid--, 0.1, 0.1, TICSECOND, 0.7);
   DrawSpriteAlpha(StrParam("lgfx/Dialogue/Icon%S.png", vmstate->strings[DSTR_ICON]), g->hid--, 0.1, 0.1, TICSECOND, 0.7);

   HudMessageF("LHUDFONT", "%S", vmstate->strings[DSTR_NAME]);
   HudMessagePlain(g->hid--, 30.1, 35.1, TICSECOND);

   HudMessageF("CBIFONT", "\Cd> Remote: %S\n\Cd> Date: %S\n\n%S",
      vmstate->strings[DSTR_REMOTE], world.canontime, vmstate->text);
   HudMessageParams(0, g->hid--, CR_WHITE, 37.1, 75.1, TICSECOND);

   if(vmstate->numoptions)
   {
      int y = 220 - (14 * vmstate->numoptions);
      for(int i = 0; i < vmstate->numoptions; i++, y += 14)
      {
         if(Lith_GUI_Button_Id(g, i, vmstate->options[i].name, 45, y, .preset = &btndlgsel))
         {
            vmstate->guiaction = GACT_SELOPTION;
            vmstate->seloption = i;
         }
      }
   }

   Lith_GUI_End(g);
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_RunDialogue
//
// Main dialogue VM.
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_RunDialogue(int num)
{
   // external state
   player_t *p = LocalPlayer;
   if(NoPlayer(p) || p->dead || p->indialogue)
      return;

   // Get the dialogue by number.
   dlgdef_t *def;

   for(def = dlgdefs; def && def->num != num; def = def->next);
   if(!def) return;

   p->indialogue = true;

   // GUI state
   gui_state_t gst = {};
   gst.gfxprefix = "lgfx/UI_Green/";
   Lith_GUI_Init(&gst, 0);

   // VM state
   dlgvmstate_t vmstate = {};
   vmstate.sptr = vmstate.stk;
   vmstate.text = "";
   for(int i = 0; i < 4; i++)
      vmstate.strings[i] = "";

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

   for(int *codeptr = def->code + def->pages[0];;)
   {
      #ifndef Lith_NoDynamicGoto
      static __label *const cases[] = {
         #define DCD(name) &&opcase_DCD_##name,
         #include "lith_dialogue.h"
      };

      DoCurCode;
      #else
   docurcode:
      switch(*codeptr)
      #endif
      {
      Op(DCD_NOP): DoNextCode;
      Op(DCD_DIE): Done;

      Op(DCD_PUSH): *vmstate.sptr++ = NextCode; DoNextCode;
      Op(DCD_POP):   vmstate.sptr--;            DoNextCode;

      #define GenArith(op) \
         vmstate.sptr[-2] op##= vmstate.sptr[-1]; vmstate.sptr--
      Op(DCD_ADD): GenArith(+); DoNextCode;
      Op(DCD_SUB): GenArith(-); DoNextCode;
      Op(DCD_MUL): GenArith(*); DoNextCode;
      Op(DCD_DIV): GenArith(/); DoNextCode;
      Op(DCD_MOD): GenArith(%); DoNextCode;
      #undef GenArith

      Op(DCD_JPAGE): codeptr = def->code + def->pages[NextCode]; DoCurCode;
      Op(DCD_JMP):   codeptr = def->code +            NextCode;  DoCurCode;

      #define GenJump(check) \
         __with(void *jmpto = def->code + NextCode;) \
            if(check) {codeptr = jmpto; DoCurCode;} \
         DoNextCode
      Op(DCD_JNZ):     GenJump(vmstate.sptr[-1] != 0);
      Op(DCD_JNITEM):  GenJump(!ACS_CheckInventory(NextCodeStr));
      Op(DCD_JNCLASS): GenJump(p->pclass != NextCode);
      #undef GenJump

      Op(DCD_SCRIPTI):
         __with(int sc = NextCode, a1 = NextCode, a2 = NextCode, a3 = NextCode, a4 = NextCode;)
            ACS_ExecuteWithResult(sc, a1, a2, a3, a4);
         DoNextCode;
      Op(DCD_SCRIPTS):
         __with(__str sc = NextCodeStr; int a1 = NextCode, a2 = NextCode, a3 = NextCode, a4 = NextCode;)
            ACS_NamedExecuteWithResult(sc, a1, a2, a3, a4);
         DoNextCode;
      Op(DCD_TRACE): Log("%S", NextCodeStr); DoNextCode;
      Op(DCD_TELEPORT_INTRALEVEL): ACS_Teleport(0, NextCode, false);    DoNextCode;
      Op(DCD_TELEPORT_INTERLEVEL): ACS_Teleport_NewMap(NextCode, 0, 0); DoNextCode;

      Op(DCD_SETSTRING): __with(int num = NextCode; __str str = NextCodeStr;) vmstate.strings[num] = str; DoNextCode;
      Op(DCD_SETTEXT):      vmstate.text =                                   NextCodeStr;  DoNextCode;
      Op(DCD_SETTEXTLOCAL): vmstate.text = StrParam("%LS",                   NextCodeStr); DoNextCode;
      Op(DCD_ADDTEXT):      vmstate.text = StrParam("%S%S\n",  vmstate.text, NextCodeStr); DoNextCode;
      Op(DCD_ADDTEXTLOCAL): vmstate.text = StrParam("%S%LS\n", vmstate.text, NextCodeStr); DoNextCode;

      Op(DCD_PUTOPT):
         __with(void *jmpto = def->code + NextCode;)
         {
            dlgoption_t *option = vmstate.options + vmstate.numoptions++;

            option->name = NextCodeStr;
            option->ptr  = ++codeptr;

            codeptr = jmpto;
         }
         DoCurCode;
      Op(DCD_DLGWAIT):
         ACS_LocalAmbientSound("misc/chat", 127);

         p->frozen++;
         p->setVel(0, 0, 0, false, true);
         do
         {
            Lith_DialogueGUI(&gst, p, &vmstate);
            ACS_Delay(1);
         }
         while(vmstate.guiaction == GACT_NONE);
         p->frozen--;
         goto guiaction;

      Op(DCD_LOGON):   vmstate.next.trmaction = TACT_LOGON;  goto logincommon;
      Op(DCD_LOGOFF):  vmstate.next.trmaction = TACT_LOGOFF; goto logincommon;
      Op(DCD_INFO):    vmstate.next.trmaction = TACT_INFO;   goto terminalcommon;
      Op(DCD_PICT):    vmstate.next.trmaction = TACT_PICT;   goto pictcommon;
      Op(DCD_TRMWAIT): goto terminalcommon;

      logincommon:
         vmstate.next.trmtimer = 42;
      pictcommon:
         vmstate.next.trmpict = NextCodeStr;
      terminalcommon:
         if(vmstate.trmaction != TACT_NONE)
         {
            switch(vmstate.trmaction)
            case TACT_LOGON:
            case TACT_LOGOFF:
               ACS_LocalAmbientSound("player/trmopen", 127);

            bool usetimer = vmstate.trmtimer != 0;
            p->frozen++;
            p->setVel(0, 0, 0, false, true);
            do
            {
               Lith_TerminalGUI(&gst, p, &vmstate);
               ACS_Delay(1);
            }
            while(vmstate.guiaction == GACT_NONE && (!usetimer || --vmstate.trmtimer >= 0));
            p->frozen--;
         }

         vmstate.cur = vmstate.next;
         vmstate.next.trmaction = TACT_NONE;
         vmstate.next.trmtimer = 0;
      guiaction:
         __with(int action = vmstate.guiaction;)
         {
            vmstate.guiaction = GACT_NONE;
            vmstate.text = "";
            switch(action)
            {
            case GACT_ACKNOWLEDGE: break;
            case GACT_SELOPTION:
               codeptr = vmstate.options[vmstate.seloption].ptr;
               vmstate.seloption  = 0;
               vmstate.numoptions = 0;
               DoCurCode;
            case GACT_EXIT: Done;
            }
         }
         DoNextCode;
      }
   }

done:
   p->indialogue = false;
}

//
// Lith_RunTerminal
//
// Runs a numbered terminal.
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_RunTerminal(int num)
{
   Lith_RunDialogue(-num);
}

//
// Lith_RunDialogueInt
//
[[__call("ScriptI"), __address(24244), __extern("ACS")]]
void Lith_RunDialogueInt(int num)
{
   Lith_RunDialogue(num);
}

// EOF

