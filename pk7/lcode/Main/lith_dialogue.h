#ifdef DCD
// VM state
DCD(NOP)
DCD(DIE)

// Stack
DCD(PUSH)
DCD(POP)

// Arithmetic
DCD(ADD)
DCD(SUB)
DCD(MUL)
DCD(DIV)
DCD(MOD)

// Jumps
DCD(JPAGE)
DCD(JMP)
DCD(JNZ)
DCD(JNITEM)
DCD(JNCLASS)

// Interaction
DCD(SCRIPTI)
DCD(SCRIPTS)
DCD(TRACE)
DCD(TELEPORT_INTRALEVEL)
DCD(TELEPORT_INTERLEVEL)

// Shared
DCD(SETSTRING)
DCD(SETTEXT)
DCD(SETTEXTLOCAL)
DCD(ADDTEXT)
DCD(ADDTEXTLOCAL)

// Dialogue
DCD(PUTOPT)
DCD(DLGWAIT)

// Terminal
DCD(LOGON)
DCD(LOGOFF)
DCD(INFO)
DCD(PICT)
DCD(TRMWAIT)
#undef DCD
#elif !defined(LITH_DIALOGUE_H)
#define LITH_DIALOGUE_H

#include "lith_world.h"


//----------------------------------------------------------------------------
// Types
//

enum
{
   #define DCD(name) DCD_##name,
   #include "lith_dialogue.h"
};

enum
{
   DSTR_NAME   = 0,
   DSTR_ICON   = 1,
   DSTR_PICT   = 1,
   DSTR_REMOTE = 2,
};

enum
{
   DTRMPAGE_UNFINISHED,
   DTRMPAGE_FINISHED,
   DTRMPAGE_FAILURE,
};

typedef struct dlgdef_s
{
   int  num;
   int *code;
   int  pages[48];
   
   struct dlgdef_s *next;
} dlgdef_t;


//----------------------------------------------------------------------------
// Extern Objects
//

extern dlgdef_t *lmvar dlgdefs;

#endif

