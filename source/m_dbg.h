// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Debugging functions.                                                     │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef NDEBUG
#define trace(n) (ACS_BeginPrint(), _c('\C'), _c(ACS_Timer() & 1 ? 'i' : 'g'), _l(_f), _c(':'), _c(' '), _l(#n), _c(' '), _c('='), _c(' '), _p(n), EndLogEx(_pri_bold|_pri_nolog))

#define Dbg_Log(level, ...) \
   (dbglevel(level) ? \
    (ACS_BeginPrint(), \
     PrintStrL(#level), \
     ACS_PrintChar(':'), \
     ACS_PrintChar(' '), \
     (__VA_ARGS__), \
     EndLogEx(_pri_critical|_pri_nonotify)) : \
    (void)0)
#define PrintErrLevel _pri_bold
#else
#define Dbg_Stat(...)
#define Dbg_Note(...)
#define Dbg_Log(...)
#define PrintErrLevel (_pri_bold | _pri_nonotify)
#endif

#ifndef NDEBUG
enum {
   log_dev,    /* general debug info */
   log_devh,   /* prints live stats to the HUD (position, angles, etc) */
   log_boss,   /* debug info for the boss system */
   log_dmon,   /* debug info for the monster tracker */
   log_gsinfo, /* static info compilers' informational output */
   log_dlg,    /* debug info for the dialogue/terminal VM */
   log_bip,    /* debug info for the BIP */
   log_dpl,    /* dynamic stack usage */
   log_save,   /* save data */
};

enum {
   dbgf_ammo,
   dbgf_bip,
   dbgf_gui,
   dbgf_items,
   dbgf_score,
   dbgf_upgr,
   dbgf_xtime,
};
#endif

#ifndef NDEBUG
void Dbg_PrintMem(void const *data, mem_size_t size);
#endif

#ifndef NDEBUG
#define dbglevel(level) get_bit(cv.debug_level, level)
#define dbgflags(flags) get_bit(cv.debug_flags, flags)
#define dbglevel_any()  !!cv.debug_level
#define dbgflags_any()  !!cv.debug_flags
#else
#define dbglevel(level) false
#define dbgflags(flags) false
#define dbglevel_any()  false
#define dbgflags_any()  false
#endif
