// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ GUI functions and data.                                                  │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef gui_h
#define gui_h

#include "m_cps.h"

#define Pre(name) .preset = &gui_p.name

/* Fixed ID */
#define G_Args(ty, ...) &(struct gui_arg_##ty){__VA_ARGS__}
#define G_Button_FId(g, id, ...) G_Button_Imp(g, id, G_Args(btn, __VA_ARGS__))
#define G_ChkBox_FId(g, id, ...) G_ChkBox_Imp(g, id, G_Args(cbx, __VA_ARGS__))
#define G_ScrBeg_FId(g, id, ...) G_ScrBeg_Imp(g, id, G_Args(scr, __VA_ARGS__))
#define G_Slider_FId(g, id, ...) G_Slider_Imp(g, id, G_Args(sld, __VA_ARGS__))
#define G_TxtBox_FId(g, id, ...) G_TxtBox_Imp(g, id, G_Args(txt, __VA_ARGS__))
#define G_WinBeg_FId(g, id, ...) G_WinBeg_Imp(g, id, G_Args(win, __VA_ARGS__))

/* Hashed ID */
#define G_HId(id) (id) + LineHash
#define G_Button_HId(g, id, ...) G_Button_FId(g, G_HId(id), __VA_ARGS__)
#define G_ChkBox_HId(g, id, ...) G_ChkBox_FId(g, G_HId(id), __VA_ARGS__)
#define G_ScrBeg_HId(g, id, ...) G_ScrBeg_FId(g, G_HId(id), __VA_ARGS__)
#define G_Slider_HId(g, id, ...) G_Slider_FId(g, G_HId(id), __VA_ARGS__)
#define G_TxtBox_HId(g, id, ...) G_TxtBox_FId(g, G_HId(id), __VA_ARGS__)
#define G_WinBeg_HId(g, id, ...) G_WinBeg_FId(g, G_HId(id), __VA_ARGS__)

/* Automatic ID */
#define G_Button(g, ...) G_Button_HId(g, 0, __VA_ARGS__)
#define G_ChkBox(g, ...) G_ChkBox_HId(g, 0, __VA_ARGS__)
#define G_ScrBeg(g, ...) G_ScrBeg_HId(g, 0, __VA_ARGS__)
#define G_Slider(g, ...) G_Slider_HId(g, 0, __VA_ARGS__)
#define G_TxtBox(g, ...) G_TxtBox_HId(g, 0, __VA_ARGS__)
#define G_WinBeg(g, ...) G_WinBeg_HId(g, 0, __VA_ARGS__)

#define G_Prefix(g, gfx, pre, mem) \
   statement({ \
           if(!(pre)->mem)     (gfx)[0] = '\0'; \
      else if((pre)->external) faststrcpy(gfx, (pre)->mem); \
      else                     faststrcpy2(gfx, (g)->gfxprefix, (pre)->mem); \
   })

#define G_ScrollReset(g, st) fastmemset(st, 0, sizeof(struct gui_scr))

#define G_TxtBoxRes(st) ((st)->tbptr = 0)

typedef i32  gid_t;
typedef char gtab_t[32];

struct gui_fil {
   i32 cur;
   i32 tic;
};

struct gui_scr {
   i32  ox, oy;
   i32  occludeS;
   i32  occludeE;
   k64  y;
   k64  grabpos;
   bool grabbed;
   i32  nextsnd;
};

struct gui_txt {
   cps_t txtbuf[Cps_Size(128)];
   i32   tbptr;
   bool  changed;
};

struct gui_win {
   bool init, grabbed;
   i32 x, y;
   i32 ox, oy;
   i32 nextsnd;
};

struct gui_delta {
   k64 cx, cy;

   bool clicklft;
   bool clickrgt;
   bool clickany;
};

struct gui_clip {
   i32 x, y, w, h, ww;
};

struct gui_state {
   anonymous
   struct gui_delta cur;
   struct gui_delta old;

   i32 defcr;

   i32 ox, oy;
   i32 w, h;

   gid_t active, hot;

   gid_t slide;
   i32 slidecount, slidetime;

   i32 clip;
   struct gui_clip clips[16];

   cstr gfxprefix;
   str  tooltip;
};

struct gui_pre_btn {
   cstr gfx;
   cstr hot;
   cstr cdef;
   cstr cact;
   cstr chot;
   cstr cdis;
   str  font;
   str  snd;
   i32  ax;
   i32  ay;
   i32  w;
   i32  h;
   bool external;
};

struct gui_arg_btn {
   cstr label;
   i32  x, y;
   bool disabled;
   cstr color;
   bool slide;
   struct gui_fil *fill;
   struct gui_pre_btn const *preset;
};

struct gui_pre_cbx {
   cstr gfx;
   cstr hot;
   cstr dis;
   cstr chkgfx;
   cstr chkhot;
   cstr chkact;
   cstr chkdis;
   str  sndup;
   str  snddn;
   bool external;
   i32  w;
   i32  h;
   i32  cx;
   i32  cy;
};

struct gui_arg_cbx {
   bool on;
   i32  x, y;
   bool disabled;
   struct gui_pre_cbx const *preset;
};

struct gui_pre_scr {
   cstr capS;
   cstr capE;
   cstr scrl;
   cstr notchgfx;
   cstr notchhot;
   bool external;
   i32  scrlw;
   i32  scrlh;
};

struct gui_arg_scr {
   struct gui_scr *st;
   i32 x;
   i32 y;
   i32 w;
   i32 h;
   i32 contenth;
   i32 ww;
   struct gui_pre_scr const *preset;
};

struct gui_pre_sld {
   cstr gfx;
   cstr notch;
   cstr notchhot;
   str  snd;
   str  font;
   bool external;
   i32  pad;
   i32  w;
   i32  h;
};

struct gui_arg_sld {
   i32  x;
   i32  y;
   k64  minima;
   k64  maxima;
   k64  step;
   k64  val;
   cstr suf;
   struct gui_pre_sld const *preset;
};

struct gui_arg_txt {
   struct gui_txt *st;
   i32 x, y;
};

struct gui_pre_win {
   cstr bg;
   i32  w;
   i32  h;
   k32  a;
   i32  bx;
   i32  by;
   bool external;
};

struct gui_arg_win {
   struct gui_win *st;
   struct gui_pre_win const *preset;
};

struct gui_presets {
   struct gui_pre_btn btndef;
   struct gui_pre_btn btntab;
   struct gui_pre_btn btnexit;
   struct gui_pre_btn btnnext;
   struct gui_pre_btn btnprev;
   struct gui_pre_btn btnnexts;
   struct gui_pre_btn btnprevs;
   struct gui_pre_btn btnlist;
   struct gui_pre_btn btnlistsel;
   struct gui_pre_btn btnlistactive;
   struct gui_pre_btn btnlistactivated;
   struct gui_pre_btn btnbipmain;
   struct gui_pre_btn btnbipback;
   struct gui_pre_btn btndlgsel;
   struct gui_pre_btn btnclear;
   struct gui_pre_btn btnnote;
   struct gui_pre_cbx cbxdef;
   struct gui_pre_cbx cbxsmall;
   struct gui_pre_scr scrdef;
   struct gui_pre_sld slddef;
   struct gui_pre_win windef;
};

struct slide_ret {
   bool different;
   k64  value;
};

enum cursor {
   gui_curs_green,
   gui_curs_pink,
   gui_curs_blue,
   gui_curs_orange,
   gui_curs_red,
   gui_curs_white,
   gui_curs_outline,
   gui_curs_outline2,
   gui_curs_outlineinv,
   gui_curs_outline2inv,
   gui_curs_max,
};

extern struct gui_presets const gui_p;

optargs(1)
void G_Auto(struct gui_state *g, gid_t id, i32 x, i32 y, i32 w, i32 h, bool slide);

void G_UpdateState(struct gui_state *g);

optargs(2)
void G_Begin(struct gui_state *g, i32 w, i32 h);
void G_End(struct gui_state *g, i32 curs);

optargs(1)
void G_Clip(struct gui_state *g, i32 x, i32 y, i32 w, i32 h, i32 ww);
void G_ClipRelease(struct gui_state *g);

stkcall
bool G_Filler(i32 x, i32 y, struct gui_fil *fil, bool held);

stkcall
void G_Tooltip(struct gui_state *g, i32 x, i32 y, i32 w, i32 h, cstr id);

i32 G_Tabs(struct gui_state *g, mem_size_t *st, gtab_t const *tn, mem_size_t num, i32 yp);

void G_ScrEnd(struct gui_state *g, struct gui_scr *scr);
optargs(1)
bool G_ScrOcc(struct gui_state *g, struct gui_scr const *scr, i32 y, i32 h);

void G_WinEnd(struct gui_state *g, struct gui_win *win);

#define G_ImpArgs(ty) struct gui_state *g, gid_t id, struct gui_arg_##ty *a
bool             G_Button_Imp(G_ImpArgs(btn));
bool             G_ChkBox_Imp(G_ImpArgs(cbx));
void             G_ScrBeg_Imp(G_ImpArgs(scr));
struct slide_ret G_Slider_Imp(G_ImpArgs(sld));
bool             G_TxtBox_Imp(G_ImpArgs(txt));
void             G_WinBeg_Imp(G_ImpArgs(win));

#endif
