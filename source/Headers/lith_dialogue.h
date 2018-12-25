// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifdef DCD
// VM state
DCD(NOP)
DCD(DIE)

// Stack
DCD(PUSH_I)
DCD(PUSH_A)
DCD(PUSH_B)
DCD(PUSH_C)
DCD(PUSH_D)

DCD(POP)
DCD(POP_A)
DCD(POP_B)
DCD(POP_C)
DCD(POP_D)

// Arithmetic
#define ArithSet(sfx) \
   DCD(ADD_##sfx) \
   DCD(SUB_##sfx) \
   DCD(MUL_##sfx) \
   DCD(DIV_##sfx) \
   DCD(MOD_##sfx) \
   DCD(IOR_##sfx) \
   DCD(AND_##sfx) \
   DCD(XOR_##sfx) \
   DCD(LSH_##sfx) \
   DCD(RSH_##sfx)
ArithSet(I)
ArithSet(A)
ArithSet(B)
ArithSet(C)
ArithSet(D)
#undef ArithSet

// Jumps
DCD(JPAGE)
DCD(JMP)
DCD(JNZ)
DCD(JNITEM)
DCD(JNCLASS)

// Interaction
DCD(SCRIPTI)
DCD(SCRIPTS)
DCD(TELEPORT_INTRALEVEL)
DCD(TELEPORT_INTERLEVEL)

// Debugging
DCD(TRACE_S)
DCD(TRACE_A)
DCD(TRACE_B)
DCD(TRACE_C)
DCD(TRACE_D)

// Shared
DCD(SETSTRING)
DCD(SETTEXT)
DCD(SETTEXTLOCAL)
DCD(ADDTEXT)
DCD(ADDTEXTLOCAL)
DCD(CONCAT)
DCD(CONCATEND)

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
#elif !defined(lith_dialogue_h)
#define lith_dialogue_h

#include "lith_world.h"
#include "lith_darray.h"

// Types ---------------------------------------------------------------------|

enum
{
   #define DCD(name) DCD_##name,
   #include "lith_dialogue.h"
};

enum
{
   DSTR_NAME,
   DSTR_ICON,
   DSTR_REMOTE,
};

enum
{
   DTRMPAGE_UNFINISHED = 0x7FFFFF0F,
   DTRMPAGE_FINISHED,
   DTRMPAGE_FAILURE,
};

struct dlgdef
{
   int num;
   int pages[48];
   Vec_Decl(int,  code);
   Vec_Decl(char, stab);

   struct dlgdef *next;
};

#endif
