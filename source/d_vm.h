// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Dialogue VM data. A 6502 emulator with extensions.                       │
// │                                                                          │
// │ Registers:                                                               │
// │    AC - accumulator                                                      │
// │    PC - program counter                                                  │
// │    RX - x                                                                │
// │    RY - y                                                                │
// │    SP - stack pointer                                                    │
// │    SR - processor state                                                  │
// │                                                                          │
// │ Extended registers:                                                      │
// │    VA - vm action                                                        │
// │                                                                          │
// │ Status bits:                                                             │
// │    B - break command (ignored)                                           │
// │    C - carry                                                             │
// │    D - decimal mode (ignored)                                            │
// │    I - interrupt disable (ignored)                                       │
// │    N - negative                                                          │
// │    V - overflow                                                          │
// │    Z - zero                                                              │
// │                                                                          │
// │ Base instructions:                                                       │
// │    ADC AND ASL BCC BCS BEQ BIT BMI BNE BPL BRK BVC BVS CLC               │
// │    CLD CLI CLV CMP CPX CPY DEC DEX DEY EOR INC INX INY JMP               │
// │    JSR LDA LDX LDY LSR NOP ORA PHA PHP PLA PLP ROL ROR RTI               │
// │    RTS SBC SEC SED SEI STA STX STY TAX TAY TSX TXA TXS TYA               │
// │                                                                          │
// │ Extended instructions:                                                   │
// │    JPG - jump page                                                       │
// │    LDV - load vm action and execute                                      │
// │    TRR - trace registers                                                 │
// │    TRS - trace stack                                                     │
// │    TRV - trace system variables                                          │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#if defined(ACT)
ACT(END_GAME)
ACT(LD_ITEM)
ACT(LD_OPT)
ACT(SCRIPT_I)
ACT(SCRIPT_S)
ACT(TELEPORT_INTERLEVEL)
ACT(TELEPORT_INTRALEVEL)
ACT(WAIT)
#undef ACT
#elif defined(DCD)
DCD(0xEA, NOP, NP)
DCD(0x00, BRK, NP)
DCD(0x10, BPL, RI)
DCD(0x20, JSR, AI)
DCD(0x30, BMI, RI)
DCD(0x40, RTI, NP)
DCD(0x50, BVC, RI)
DCD(0x60, RTS, NP)
DCD(0x70, BVS, RI)
DCD(0x90, BCC, RI)
DCD(0xB0, BCS, RI)
DCD(0xD0, BNE, RI)
DCD(0xF0, BEQ, RI)
DCD(0x4C, JMP, AI)
DCD(0x6C, JMP, II)
DCD(0x22, JPG, VI) /* extension */
DCD(0xC0, CPY, VI)
DCD(0xE0, CPX, VI)
DCD(0xC1, CMP, IX)
DCD(0xD1, CMP, IY)
DCD(0x24, BIT, ZI)
DCD(0xC4, CPY, ZI)
DCD(0xE4, CPX, ZI)
DCD(0xC5, CMP, ZI)
DCD(0xD5, CMP, ZX)
DCD(0xC9, CMP, VI)
DCD(0xD9, CMP, AY)
DCD(0x2C, BIT, AI)
DCD(0xCC, CPY, AI)
DCD(0xEC, CPX, AI)
DCD(0xCD, CMP, AI)
DCD(0xDD, CMP, AX)
DCD(0x08, PHP, NP)
DCD(0x28, PLP, NP)
DCD(0x48, PHA, NP)
DCD(0x68, PLA, NP)
DCD(0x18, CLC, NP)
DCD(0x38, SEC, NP)
DCD(0x58, CLI, NP)
DCD(0x78, SEI, NP)
DCD(0xB8, CLV, NP)
DCD(0xD8, CLD, NP)
DCD(0xF8, SED, NP)
DCD(0xA0, LDY, VI)
DCD(0xA1, LDA, IX)
DCD(0xB1, LDA, IY)
DCD(0xA2, LDX, VI)
DCD(0xA4, LDY, ZI)
DCD(0xB4, LDY, ZX)
DCD(0xA5, LDA, ZI)
DCD(0xB5, LDA, ZX)
DCD(0xA6, LDX, ZI)
DCD(0xB6, LDX, ZY)
DCD(0xA9, LDA, VI)
DCD(0xB9, LDA, AY)
DCD(0xAC, LDY, AI)
DCD(0xBC, LDY, AX)
DCD(0xAD, LDA, AI)
DCD(0xBD, LDA, AX)
DCD(0xAE, LDX, AI)
DCD(0xBE, LDX, AY)
DCD(0x03, LDV, VI) /* extension */
DCD(0x13, LDV, ZI) /* extension */
DCD(0x23, LDV, ZX) /* extension */
DCD(0x33, LDV, AI) /* extension */
DCD(0x43, LDV, AX) /* extension */
DCD(0x98, TYA, NP)
DCD(0xA8, TAY, NP)
DCD(0x8A, TXA, NP)
DCD(0x9A, TXS, NP)
DCD(0xAA, TAX, NP)
DCD(0xBA, TSX, NP)
DCD(0x81, STA, IX)
DCD(0x91, STA, IY)
DCD(0x84, STY, ZI)
DCD(0x94, STY, ZX)
DCD(0x85, STA, ZI)
DCD(0x95, STA, ZX)
DCD(0x86, STX, ZI)
DCD(0x96, STX, ZY)
DCD(0x99, STA, AY)
DCD(0x8C, STY, AI)
DCD(0x8D, STA, AI)
DCD(0x9D, STA, AX)
DCD(0x8E, STX, AI)
DCD(0x01, ORA, IX)
DCD(0x11, ORA, IY)
DCD(0x21, AND, IX)
DCD(0x31, AND, IY)
DCD(0x41, EOR, IX)
DCD(0x51, EOR, IY)
DCD(0x61, ADC, IX)
DCD(0x71, ADC, IY)
DCD(0xE1, SBC, IX)
DCD(0xF1, SBC, IY)
DCD(0x05, ORA, ZI)
DCD(0x15, ORA, ZX)
DCD(0x25, AND, ZI)
DCD(0x35, AND, ZX)
DCD(0x45, EOR, ZI)
DCD(0x55, EOR, ZX)
DCD(0x65, ADC, ZI)
DCD(0x75, ADC, ZX)
DCD(0xE5, SBC, ZI)
DCD(0xF5, SBC, ZX)
DCD(0x06, ASL, ZI)
DCD(0x16, ASL, ZX)
DCD(0x26, ROL, ZI)
DCD(0x36, ROL, ZX)
DCD(0x46, LSR, ZI)
DCD(0x56, LSR, ZX)
DCD(0x66, ROR, ZI)
DCD(0x76, ROR, ZX)
DCD(0xC6, DEC, ZI)
DCD(0xD6, DEC, ZX)
DCD(0xE6, INC, ZI)
DCD(0xF6, INC, ZX)
DCD(0x88, DEY, NP)
DCD(0xC8, INY, NP)
DCD(0xE8, INX, NP)
DCD(0x09, ORA, VI)
DCD(0x19, ORA, AY)
DCD(0x29, AND, VI)
DCD(0x39, AND, AY)
DCD(0x49, EOR, VI)
DCD(0x59, EOR, AY)
DCD(0x69, ADC, VI)
DCD(0x79, ADC, AY)
DCD(0xE9, SBC, VI)
DCD(0xF9, SBC, AY)
DCD(0x0A, ASL, NP)
DCD(0x2A, ROL, NP)
DCD(0x4A, LSR, NP)
DCD(0x6A, ROR, NP)
DCD(0xCA, DEX, NP)
DCD(0x0D, ORA, AI)
DCD(0x1D, ORA, AX)
DCD(0x2D, AND, AI)
DCD(0x3D, AND, AX)
DCD(0x4D, EOR, AI)
DCD(0x5D, EOR, AX)
DCD(0x6D, ADC, AI)
DCD(0x7D, ADC, AX)
DCD(0xED, SBC, AI)
DCD(0xFD, SBC, AX)
DCD(0x0E, ASL, AI)
DCD(0x1E, ASL, AX)
DCD(0x2E, ROL, AI)
DCD(0x3E, ROL, AX)
DCD(0x4E, LSR, AI)
DCD(0x5E, LSR, AX)
DCD(0x6E, ROR, AI)
DCD(0x7E, ROR, AX)
DCD(0xCE, DEC, AI)
DCD(0xDE, DEC, AX)
DCD(0xEE, INC, AI)
DCD(0xFE, INC, AX)
DCD(0x02, TRR, NP) /* extension */
DCD(0x12, TRS, NP) /* extension */
DCD(0x32, TRV, NP) /* extension */
#undef DCD
#elif !defined(d_vm_h)
#define d_vm_h

#include "w_world.h"
#include "m_cps.h"

#define StructOfs(nam, mem, sel) \
   (VAR_##nam##_BEG + S_##nam##_##mem + S_##nam##_SIZE * (sel))

#define StructOfsHead(nam, sel) (VAR_##nam##_BEG + S_##nam##_SIZE * (sel))

enum {
   #define DCD(n, op, ty) DCD_##op##_##ty = n,
   #include "d_vm.h"
};

enum {
   /* program numbers */
   DNUM_PRG_BEG,
   DNUM_PRG_END = DNUM_PRG_BEG + 15,
   DNUM_DLG_BEG,
   DNUM_DLG_END = DNUM_DLG_BEG + 15,
   DNUM_TRM_BEG,
   DNUM_TRM_END = DNUM_TRM_BEG + 15,
   DNUM_MAX,
};

enum {
   /* page numbers */
   DPAGE_NORMAL_MAX = 250,
   DPAGE_UNFINISHED,
   DPAGE_FINISHED,
   DPAGE_FAILURE,
   DPAGE_MAX,
};

enum {
   /* memory regions */
   STA_BEG = 0x0100,
   STA_END = 0x01FF,
   SYS_BEG = 0x0200,
   SYS_END = 0x02FF,
   VAR_BEG = 0x0300,
   VAR_END = 0x9FFF,
   PRG_BEG = 0xA000,
   PRG_END = 0xBFFF,
   STR_BEG = 0xC000,
   STR_END = 0xFEFF,
   IRQ_BEG = 0xFF00,
   STA_BEG_C = STA_BEG / 4,
   PRG_BEG_C = PRG_BEG / 4,
   STR_BEG_C = STR_BEG / 4,
};

enum {
   ADRM_AI, /* absolute-immediate */
   ADRM_AX, /* absolute-X         */
   ADRM_AY, /* absolute-Y         */
   ADRM_II, /* indirect-immediate */
   ADRM_IX, /* indirect-X         */
   ADRM_IY, /* indirect-Y         */
   ADRM_NP, /* no-parameter       */
   ADRM_RI, /* relative-immediate */
   ADRM_VI, /* value-immediate    */
   ADRM_ZI, /* zeropage-immediate */
   ADRM_ZX, /* zeropage-X         */
   ADRM_ZY, /* zeropage-Y         */
};

enum {
   /* option structure */
   S_OPT_PTRL,
   S_OPT_PTRH,
   S_OPT_NAML,
   S_OPT_NAMH,
   S_OPT_SIZE,
};

enum {
   /* firmware modes */
   FIRM_NONE,
   FIRM_DIALOGUE,
   FIRM_TERMINAL,
   FIRM_FINALE,
};

enum {
   VAR_FRMMD = SYS_BEG, /* firmware mode */
   VAR_FRMAC,           /* firmware action */
   VAR_ADRL,  VAR_ADRH,  /* vm action address */
   VAR_RADRL, VAR_RADRH, /* vm action address 2 */
   VAR_SCP0, VAR_SCP1, VAR_SCP2, VAR_SCP3, VAR_SCP4, /* script arguments */
   VAR_NAMEL,   VAR_NAMEH,   /* name string */
   VAR_ICONL,   VAR_ICONH,   /* icon string */
   VAR_REMOTEL, VAR_REMOTEH, /* remote string */
   VAR_PICTL,   VAR_PICTH,   /* picture string */
   VAR_TEXTL,   VAR_TEXTH,   /* text string */
   VAR_MUSICL,  VAR_MUSICH,  /* music string */
   VAR_OPT_CNT, /* option count */
   VAR_OPT_SEL, /* option selection */
   VAR_OPT_BEG, /* option structure data */
   VAR_OPT_END = VAR_OPT_BEG + S_OPT_SIZE * 16,
   VAR_PCLASS, /* player class */
};

enum {
   #define ACT(name) ACT_##name,
   #include "d_vm.h"
   ACT_MAX,
   /* response actions */
   ACT_NONE,
   ACT_HALT,
   ACT_JUMP,
};

enum {
   TACT_NONE,
   TACT_LOGON,
   TACT_LOGOFF,
   TACT_INFO,
   TACT_PICT,
};

enum {
   FACT_NONE,
   FACT_FADE_IN,
   FACT_FADE_OUT,
   FACT_WAIT,
   FACT_MUS_FADE,
   FACT_CRAWL,
};

enum {
   UACT_NONE,
   UACT_ACKNOWLEDGE,
   UACT_SELOPTION,
   UACT_EXIT,
};

struct dlg_def {
   mem_size_t pages[DPAGE_MAX];
   u32 *codeV; mem_size_t codeC, codeP;
   u32 *stabV; mem_size_t stabC, stabP;
};

struct dlg_start_info {
   mem_size_t active;
   mem_size_t page;
   mem_size_t num;
   i32        firm_mode;
};

struct dcd_info {
   char       name[7];
   mem_size_t adrm;
};

void Dlg_MInit(void);
script void Dlg_Run(void);

extern struct dlg_def        dlgdefs[DNUM_MAX];
extern struct dcd_info const dcdinfo[0xFF];

#endif
