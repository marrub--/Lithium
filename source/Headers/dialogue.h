/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Dialogue VM code data.
 *
 * ---------------------------------------------------------------------------|
 */

#ifdef DCD
/* VM state */
DCD(NOP)
DCD(HLT)

/* Stack */
DCD(PHA)
DCD(PLA)

/* Load */
DCD(LDA)
DCD(LDX)
DCD(LDY)

/* Transfer */
DCD(TAX)
DCD(TAY)
DCD(TSX)
DCD(TXA)
DCD(TXS)
DCD(TYA)

/* Arithmetic */
#define ArithSet(sfx) \
   DCD(ADD##sfx) \
   DCD(SUB##sfx) \
   DCD(MUL##sfx) \
   DCD(DIV##sfx) \
   DCD(MOD##sfx) \
   DCD(IOR##sfx) \
   DCD(AND##sfx) \
   DCD(XOR##sfx) \
   DCD(LSH##sfx) \
   DCD(RSH##sfx)
ArithSet(I)
#undef ArithSet

/* Jumps */
DCD(JPAGE)
DCD(JMP)
DCD(JNZ)
DCD(JNITEM)
DCD(JNCLASS)

/* Interaction */
DCD(SCRIPTI)
DCD(SCRIPTS)
DCD(TELEPORT_INTRALEVEL)
DCD(TELEPORT_INTERLEVEL)

/* Debugging */
DCD(TRA)
DCD(TRX)
DCD(TRY)
DCD(TRZ)

/* Shared */
DCD(SETSTRING)
DCD(SETTEXT)
DCD(SETTEXTLOCAL)
DCD(ADDTEXT)
DCD(ADDTEXTLOCAL)
DCD(CONCAT)
DCD(CONCATEND)

/* Dialogue */
DCD(PUTOPT)
DCD(DLGWAIT)

/* Terminal */
DCD(LOGON)
DCD(LOGOFF)
DCD(INFO)
DCD(PICT)
DCD(TRMWAIT)
#undef DCD
#elif !defined(dialogue_h)
#define dialogue_h

#include "w_world.h"
#include "m_vec.h"

/* Types ------------------------------------------------------------------- */

enum
{
   #define DCD(name) DCD_##name,
   #include "dialogue.h"
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
   i32 num;
   i32 pages[48];
   Vec_Decl(i32,  code);
   Vec_Decl(char, stab);

   struct dlgdef *next;
};

#endif
