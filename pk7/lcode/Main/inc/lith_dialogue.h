#ifndef LITH_DIALOGUE_H
#define LITH_DIALOGUE_H

#include "lith_world.h"


//----------------------------------------------------------------------------
// Types
//

enum
{
   // VM state
   DCD_NOP,
   DCD_DIE,
   
   // Stack
   DCD_PUSH,
   DCD_POP,
   
   // Arithmetic
   DCD_ADD,
   DCD_SUB,
   DCD_MUL,
   DCD_DIV,
   DCD_MOD,
   
   // Jumps
   DCD_JPAGE,
   DCD_JMP,
   DCD_JNZ,
   DCD_JNITEM,
   DCD_JNCLASS,
   
   // Dialogue
   DCD_NAME,
   DCD_TEXT,
   DCD_ICON,
   DCD_REMOTE,
   DCD_PUTOPT,
   DCD_WAIT,
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

extern dlgdef_t *Lith_MapVariable dlgdefs;

#endif

