/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * GUI functions and data.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef gui_h
#define gui_h

#include "m_cps.h"

#define Pre(name) .preset = &gui_p.name

/* Fixed ID */
#define G_Button_FId(g, id, ...)      G_Button_Impl     (g, id, &(struct gui_arg_btn const){__VA_ARGS__})
#define G_Checkbox_FId(g, id, ...)    G_Checkbox_Impl   (g, id, &(struct gui_arg_cbx const){__VA_ARGS__})
#define G_ScrollBegin_FId(g, id, ...) G_ScrollBegin_Impl(g, id, &(struct gui_arg_scr const){__VA_ARGS__})
#define G_Slider_FId(g, id, ...)      G_Slider_Impl     (g, id, &(struct gui_arg_sld const){__VA_ARGS__})
#define G_TextBox_FId(g, id, ...)     G_TextBox_Impl    (g, id, &(struct gui_arg_txt const){__VA_ARGS__})

/* Dynamic ID */
#define G_Button_Id(g, id, ...)      G_Button_FId     (g, id + LineHash, __VA_ARGS__)
#define G_Checkbox_Id(g, id, ...)    G_Checkbox_FId   (g, id + LineHash, __VA_ARGS__)
#define G_ScrollBegin_Id(g, id, ...) G_ScrollBegin_FId(g, id + LineHash, __VA_ARGS__)
#define G_Slider_Id(g, id, ...)      G_Slider_FId     (g, id + LineHash, __VA_ARGS__)
#define G_TextBox_Id(g, id, ...)     G_TextBox_FId    (g, id + LineHash, __VA_ARGS__)

/* Dynamic */
#define G_Button(g, ...)             G_Button_Id     (g, 0, __VA_ARGS__)
#define G_Checkbox(g, ...)           G_Checkbox_Id   (g, 0, __VA_ARGS__)
#define G_ScrollBegin(g, ...)        G_ScrollBegin_Id(g, 0, __VA_ARGS__)
#define G_Slider(g, ...)             G_Slider_Id     (g, 0, __VA_ARGS__)
#define G_TextBox(g, ...)            G_TextBox_Id    (g, 0, __VA_ARGS__)

#define G_GenPreset(type, def) \
   type pre; \
   if(a->preset) pre = *a->preset; \
   else          pre = def

#define G_Prefix(g, gfx, pre, mem) \
   do { \
           if(!(pre)->mem)     (gfx)[0] = '\0'; \
      else if((pre)->external) strcpy(gfx, (pre)->mem); \
      else                     lstrcpy2(gfx, (g)->gfxprefix, (pre)->mem); \
   } while(0)

#define G_ScrollReset(g, st) \
   (*(st) = (struct gui_scr){})

#define G_Tabs(g, st, names, x, y, yy) \
   do { \
      for(i32 _i = 0; _i < countof(names); _i++) \
      { \
         if(G_Button_Id(g, _i, names[_i], gui_p.btntab.w * _i + (x), \
            gui_p.btntab.h * (yy) + (y), _i == *(st), .preset = &gui_p.btntab)) \
         { \
            *(st) = _i; \
         } \
      } \
   } while(0)

#define G_TextBox_Reset(st) ((st)->tbptr = 0)

#define G_TextBox_OnTextEntered(st) \
   __with(cstr txt_buf = Cps_Expand(st->txtbuf, 0, st->tbptr);) \
      ifauto(cstr, _c, strchr(txt_buf, '\n')) \
         __with(size_t txt_len = _c - txt_buf; G_TextBox_Reset(st);)

/* Types ------------------------------------------------------------------- */

struct gui_scr
{
   i32  ox;
   i32  oy;
   i32  occludeS;
   i32  occludeE;
   k64  y;
   k64  grabpos;
   bool grabbed;
   i32  nextsnd;
};

struct gui_typ
{
   str txt;
   i32 len;
   i32 pos;
};

struct gui_txt
{
   Cps_Decl(txtbuf, 128);
   i32 tbptr;
};

struct gui_delta
{
   k64 cx, cy;

   bool clicklft;
   bool clickrgt;
   bool clickany;
};

struct gui_state
{
   anonymous
   struct gui_delta cur;
   struct gui_delta old;

   i32 ox, oy;
   i32 w, h;

   u32 active, hot;

   u32 slide;
   i32 slidecount, slidetime;

   u32 dbl;
   i32 dbltime;

   bool useclip;
   i32 clpxS, clpyS, clpxE, clpyE;

   void *state;

   cstr gfxprefix;
};

struct gui_pre_btn
{
   cstr gfx;
   cstr hot;
   cstr cdef;
   cstr cact;
   cstr chot;
   cstr cdis;
   str font;
   str snd;
   i32 ax;
   i32 ay;
   i32 w;
   i32 h;
   bool external;
};

struct gui_arg_btn
{
   cstr label;
   i32  x, y;
   bool disabled;
   cstr color;
   bool slide;
   struct gui_pre_btn const *preset;
};

struct gui_pre_cbx
{
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

struct gui_arg_cbx
{
   bool on;
   i32  x, y;
   bool disabled;
   struct gui_pre_cbx const *preset;
};

struct gui_pre_scr
{
   cstr capS;
   cstr capE;
   cstr scrl;
   cstr notchgfx;
   cstr notchhot;
   bool external;
   i32  scrlw;
   i32  scrlh;
};

struct gui_arg_scr
{
   struct gui_scr *st;
   i32 x;
   i32 y;
   i32 w;
   i32 h;
   i32 contenth;
   i32 ww;
   struct gui_pre_scr const *preset;
};

struct gui_pre_sld
{
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

struct gui_arg_sld
{
   i32  x;
   i32  y;
   k64  minima;
   k64  maxima;
   k64  val;
   bool integ;
   cstr suf;
   struct gui_pre_sld const *preset;
};

struct gui_arg_txt
{
   struct gui_txt *st;
   i32 x, y;
   struct player *p;
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
};

enum cursor
{
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

/* Extern Objects ---------------------------------------------------------- */

extern struct gui_presets const gui_p;

/* Extern Functions -------------------------------------------------------- */

stkcall optargs(1)
void G_Auto(struct gui_state *g, u32 id, i32 x, i32 y, i32 w, i32 h, bool slide);

stkcall optargs(1)
void G_Init(struct gui_state *g, void *state);

stkcall
void G_UpdateState(struct gui_state *g, struct player *p);

stkcall optargs(2)
void G_Begin(struct gui_state *g, i32 w, i32 h);
stkcall
void G_End(struct gui_state *g, enum cursor curs);

stkcall optargs(1)
void G_Clip(struct gui_state *g, i32 x, i32 y, i32 w, i32 h, i32 ww);
stkcall
void G_ClipRelease(struct gui_state *g);

stkcall cstr RemoveTextColors_str(astr s, i32 size);
stkcall cstr RemoveTextColors    (cstr s, i32 size);

stkcall
void G_TypeOn(struct gui_state *g, struct gui_typ *typeon, str text);
stkcall
struct gui_typ const *G_TypeOnUpdate(struct gui_state *g, struct gui_typ *typeon);

void G_ScrollEnd(struct gui_state *g, struct gui_scr *scr);
optargs(1)
bool G_ScrollOcclude(struct gui_state *g, struct gui_scr const *scr, i32 y, i32 h);

bool            G_Button_Impl     (struct gui_state *g, u32 id, struct gui_arg_btn const *a);
bool            G_Checkbox_Impl   (struct gui_state *g, u32 id, struct gui_arg_cbx const *a);
void            G_ScrollBegin_Impl(struct gui_state *g, u32 id, struct gui_arg_scr const *a);
k64             G_Slider_Impl     (struct gui_state *g, u32 id, struct gui_arg_sld const *a);
struct gui_txt *G_TextBox_Impl    (struct gui_state *g, u32 id, struct gui_arg_txt const *a);

#endif
