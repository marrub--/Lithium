#include "lith_common.h"
#include "lith_world.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_dialogue.h"


//----------------------------------------------------------------------------
// Types
//

enum
{
   GACT_NONE,
   GACT_SELOPTION,
   GACT_EXIT,
};

typedef struct dlgoption_s
{
   __str name;
   int  *ptr;
} dlgoption_t;

typedef struct dlgvmstate_s
{
   int stk[32], *sptr;
   __str name, text, icon, remote;
   dlgoption_t options[8];
   int guiaction;
   int numoptions;
   int seloption;
} dlgvmstate_t;


//----------------------------------------------------------------------------
// Extern Objects
//

dlgdef_t *Lith_MapVariable dlgdefs;


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_DialogueGUI
//
[[__call("ScriptS")]]
static void Lith_DialogueGUI(gui_state_t *g, player_t *p, dlgvmstate_t *vmstate)
{
   Lith_GUI_Begin(g, hid_end_dialogue, 320, 240);
   Lith_GUI_UpdateState(g, p);
   
   DrawSpriteAlpha("lgfx/Dialogue/Back.png", g->hid--, 0.1, 0.1, TICSECOND, 0.7);
   DrawSpriteAlpha(StrParam("lgfx/Dialogue/Icon%S.png", vmstate->icon), g->hid--, 0.1, 0.1, TICSECOND, 0.7);
   
   HudMessageF("LHUDFONT", "%S", vmstate->name);
   HudMessagePlain(g->hid--, 30.1, 35.1, TICSECOND);
   
   HudMessageF("CBIFONT",
      StrParam("\Cd> Remote: %%S\n\Cd> Date: %%S\n\n%S", vmstate->text[0] == '$' ? "%LS" : "%S"),
      vmstate->remote, world.canontime, vmstate->text);
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

//
// Lith_RunDialogue
//
// Main dialogue VM.
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_RunDialogue(int num)
{
   // external state
   player_t *p = Lith_LocalPlayer;
   
   if(p->indialogue)
      return;
   
   // Get the dialogue by number.
   dlgdef_t *def;
   
   for(def = dlgdefs; def && def->num != num; def = def->next);
   if(!def) return;
   
   p->indialogue = true;
   
   // GUI state
   gui_state_t gst = {};
   gst.cx = 320 / 2;
   gst.cy = 200 / 2;
   gst.gfxprefix = "lgfx/UI_Green/";
   Lith_GUI_Init(&gst, 0);
   
   // VM state
   dlgvmstate_t vmstate = {};
   vmstate.sptr = vmstate.stk;
   vmstate.name = "Default";
   vmstate.text = "";
   vmstate.icon = "None";
   vmstate.remote = "<unknown>@<unknown>";
   
   #define DoCurCode  goto docurcode
   #define DoNextCode continue
   #define Done       goto done
   #define Op(name) case name
   #define GenArith(op) vmstate.sptr[-2] op##= vmstate.sptr[-1]; vmstate.sptr--
   for(int *codeptr = def->code + def->pages[0];; codeptr++)
   {
   docurcode:
      switch(*codeptr)
      {
      Op(DCD_NOP): DoNextCode;
      Op(DCD_DIE): Done;
      Op(DCD_PUSH): *vmstate.sptr++ = *++codeptr; DoNextCode;
      Op(DCD_POP):   vmstate.sptr--;              DoNextCode;
      Op(DCD_ADD): GenArith(+); DoNextCode;
      Op(DCD_SUB): GenArith(-); DoNextCode;
      Op(DCD_MUL): GenArith(*); DoNextCode;
      Op(DCD_DIV): GenArith(/); DoNextCode;
      Op(DCD_MOD): GenArith(%); DoNextCode;
      Op(DCD_JPAGE): codeptr = def->code + def->pages[*++codeptr]; DoCurCode;
      Op(DCD_JMP):   codeptr = def->code +            *++codeptr;  DoCurCode;
      Op(DCD_JNZ):
         __with(void *jmpto = def->code + *++codeptr;)
            if(vmstate.sptr[-1] != 0)
               {codeptr = jmpto; DoCurCode;}
         DoNextCode;
      Op(DCD_JNITEM):
         __with(void *jmpto = def->code + *++codeptr;)
            if(!ACS_CheckInventory((__str)*++codeptr))
               {codeptr = jmpto; DoCurCode;}
         DoNextCode;
      Op(DCD_JNCLASS):
         __with(void *jmpto = def->code + *++codeptr;)
            if(p->pclass != *++codeptr)
               {codeptr = jmpto; DoCurCode;}
         DoNextCode;
      Op(DCD_NAME):   vmstate.name   = (__str)*++codeptr; DoNextCode;
      Op(DCD_TEXT):   vmstate.text   = (__str)*++codeptr; DoNextCode;
      Op(DCD_ICON):   vmstate.icon   = (__str)*++codeptr; DoNextCode;
      Op(DCD_REMOTE): vmstate.remote = (__str)*++codeptr; DoNextCode;
      Op(DCD_PUTOPT):
         __with(void *jmpto = def->code + *++codeptr;)
         {
            dlgoption_t *option = vmstate.options + vmstate.numoptions++;
            
            option->name = (__str)*++codeptr;
            option->ptr  = ++codeptr;
            
            codeptr = jmpto;
         }
         DoCurCode;
      Op(DCD_WAIT):
         ACS_LocalAmbientSound("misc/chat", 127);
         
         p->frozen++;
         do
         {
            Lith_DialogueGUI(&gst, p, &vmstate);
            ACS_Delay(1);
         }
         while(vmstate.guiaction == GACT_NONE);
         p->frozen--;
         
         __with(int action = vmstate.guiaction;)
         {
            vmstate.guiaction = 0;
            switch(action)
            {
            case GACT_SELOPTION:
               codeptr = vmstate.options[vmstate.seloption].ptr;
               vmstate.seloption  = 0;
               vmstate.numoptions = 0;
               DoCurCode;
            case GACT_EXIT:
               Done;
            }
         }
         DoNextCode;
      }
   }
   #undef DoCurCode
   #undef DoNextCode
   #undef Done
   #undef Op
   #undef GenArith
   
done:
   p->indialogue = false;
}

// EOF

