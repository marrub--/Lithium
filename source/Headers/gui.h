// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// gui.h: GUI functions and data.

#ifndef gui_h
#define gui_h

#include "m_cps.h"

#define LineHash ((u32)__LINE__ * FileHash)

#define Pre(name) .preset = &gui_p.name

// Fixed ID
#define Lith_GUI_Button_FId(g, id, ...)      Lith_GUI_Button_Impl     (g, id, &(struct gui_arg_btn const){__VA_ARGS__})
#define Lith_GUI_Checkbox_FId(g, id, ...)    Lith_GUI_Checkbox_Impl   (g, id, &(struct gui_arg_cbx const){__VA_ARGS__})
#define Lith_GUI_ScrollBegin_FId(g, id, ...) Lith_GUI_ScrollBegin_Impl(g, id, &(struct gui_arg_scr const){__VA_ARGS__})
#define Lith_GUI_Slider_FId(g, id, ...)      Lith_GUI_Slider_Impl     (g, id, &(struct gui_arg_sld const){__VA_ARGS__})
#define Lith_GUI_TextBox_FId(g, id, ...)     Lith_GUI_TextBox_Impl    (g, id, &(struct gui_arg_txt const){__VA_ARGS__})

// Dynamic ID
#define Lith_GUI_Button_Id(g, id, ...)      Lith_GUI_Button_FId     (g, id + LineHash, __VA_ARGS__)
#define Lith_GUI_Checkbox_Id(g, id, ...)    Lith_GUI_Checkbox_FId   (g, id + LineHash, __VA_ARGS__)
#define Lith_GUI_ScrollBegin_Id(g, id, ...) Lith_GUI_ScrollBegin_FId(g, id + LineHash, __VA_ARGS__)
#define Lith_GUI_Slider_Id(g, id, ...)      Lith_GUI_Slider_FId     (g, id + LineHash, __VA_ARGS__)
#define Lith_GUI_TextBox_Id(g, id, ...)     Lith_GUI_TextBox_FId    (g, id + LineHash, __VA_ARGS__)

// Dynamic
#define Lith_GUI_Button(g, ...)             Lith_GUI_Button_Id     (g, 0, __VA_ARGS__)
#define Lith_GUI_Checkbox(g, ...)           Lith_GUI_Checkbox_Id   (g, 0, __VA_ARGS__)
#define Lith_GUI_ScrollBegin(g, ...)        Lith_GUI_ScrollBegin_Id(g, 0, __VA_ARGS__)
#define Lith_GUI_Slider(g, ...)             Lith_GUI_Slider_Id     (g, 0, __VA_ARGS__)
#define Lith_GUI_TextBox(g, ...)            Lith_GUI_TextBox_Id    (g, 0, __VA_ARGS__)

#define Lith_GUI_GenPreset(type, def) \
   type pre; \
   if(a->preset) pre = *a->preset; \
   else          pre = def

#define Lith_GUI_Prefix(set) \
   if(set) { \
      ACS_BeginPrint(); \
      PrintChrSt(g->gfxprefix); \
      ACS_PrintString(set); \
      set = ACS_EndStrParam(); \
   } else (void)0
#define Lith_GUI_Prefix1(g, pre, mem) \
   (!(pre)->external \
      ? ((pre)->mem ? (ACS_BeginPrint(), \
                       PrintChrSt((g)->gfxprefix), \
                       ACS_PrintString((pre)->mem), \
                       ACS_EndStrParam()) : nil) \
      : (pre)->mem)
#define Lith_GUI_Prefix2(g, gfx, pre, mem) \
   do { \
      if(!(pre)->mem) \
         (gfx)[0] = '\0'; \
      else if((pre)->external) \
         strncpy(gfx, (pre)->mem, sizeof(gfx)); \
      else \
      { \
         strcpy(gfx, (g)->gfxprefix); \
         strcat(gfx, (pre)->mem); \
      } \
   } while(0)

#define Lith_GUI_ScrollReset(g, st) \
   (*(st) = (struct gui_scr){})

#define Lith_GUI_Tabs(g, st, names, x, y, yy) \
   do { \
      for(i32 _i = 0; _i < countof(names); _i++) \
      { \
         if(Lith_GUI_Button_Id(g, _i, names[_i], gui_p.btntab.w * _i + (x), \
            gui_p.btntab.h * (yy) + (y), _i == *(st), .preset = &gui_p.btntab)) \
         { \
            *(st) = _i; \
         } \
      } \
   } while(0)

#define Lith_GUI_BasicCheckbox(g, st, ...) \
   if(Lith_GUI_Checkbox((g), *(st), __VA_ARGS__)) \
      *(st) = !*(st); \
   else \
      (void)0

#define Lith_GUI_TextBox_Reset(st) ((st)->tbptr = 0)

#define Lith_GUI_TextBox_OnTextEntered(st) \
   __with(char const *txt_buf = Cps_Print(st->txtbuf, st->tbptr);) \
      ifauto(char const *, _c, strchr(txt_buf, '\n')) \
         __with(size_t txt_len = _c - txt_buf; Lith_GUI_TextBox_Reset(st);)

// Types ---------------------------------------------------------------------|

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

   u32 active, hot, slide;
   i32 slidecount, slidetime;

   bool useclip;
   i32 clpxS, clpyS, clpxE, clpyE;

   void *state;

   char const *gfxprefix;
};

struct gui_pre_btn
{
   char const *gfx;
   char const *hot;
   char const *snd;
   char const *cdef;
   char const *cact;
   char const *chot;
   char const *cdis;
   char const *font;
   i32 ax;
   i32 ay;
   i32 w;
   i32 h;
   bool external;
};

struct gui_arg_btn
{
   char const *label;
   i32         x, y;
   bool        disabled;
   char const *color;
   bool        slide;
   struct gui_pre_btn const *preset;
};

struct gui_pre_cbx
{
   str  gfx;
   str  hot;
   str  dis;
   str  sndup;
   str  snddn;
   str  chkgfx;
   str  chkhot;
   str  chkact;
   str  chkdis;
   bool external;
   i32  w;
   i32  h;
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
   str  capS;
   str  capE;
   str  scrl;
   str  notchgfx;
   str  notchhot;
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
   str  gfx;
   str  snd;
   str  notch;
   str  notchhot;
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
   char const *suf;
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

// Extern Objects ------------------------------------------------------------|

extern struct gui_presets const gui_p;

// Extern Functions ----------------------------------------------------------|

stkcall optargs(1)
void Lith_GUI_Auto(struct gui_state *g, u32 id, i32 x, i32 y, i32 w, i32 h, bool slide);

stkcall optargs(1)
void Lith_GUI_Init(struct gui_state *g, void *state);

stkcall
void Lith_GUI_UpdateState(struct gui_state *g, struct player *p);

stkcall optargs(2)
void Lith_GUI_Begin(struct gui_state *g, i32 w, i32 h);
stkcall
void Lith_GUI_End(struct gui_state *g, enum cursor curs);

stkcall optargs(1)
void Lith_GUI_Clip(struct gui_state *g, i32 x, i32 y, i32 w, i32 h, i32 ww);
stkcall
void Lith_GUI_ClipRelease(struct gui_state *g);

stkcall char const *RemoveTextColors_str(char __str_ars const *s, i32 size);
stkcall char const *RemoveTextColors    (char           const *s, i32 size);

stkcall
void Lith_GUI_TypeOn(struct gui_state *g, struct gui_typ *typeon, str text);
stkcall
struct gui_typ const *Lith_GUI_TypeOnUpdate(struct gui_state *g, struct gui_typ *typeon);

void Lith_GUI_ScrollEnd(struct gui_state *g, struct gui_scr *scr);
optargs(1)
bool Lith_GUI_ScrollOcclude(struct gui_state *g, struct gui_scr const *scr, i32 y, i32 h);

bool            Lith_GUI_Button_Impl     (struct gui_state *g, u32 id, struct gui_arg_btn const *a);
bool            Lith_GUI_Checkbox_Impl   (struct gui_state *g, u32 id, struct gui_arg_cbx const *a);
void            Lith_GUI_ScrollBegin_Impl(struct gui_state *g, u32 id, struct gui_arg_scr const *a);
k64             Lith_GUI_Slider_Impl     (struct gui_state *g, u32 id, struct gui_arg_sld const *a);
struct gui_txt *Lith_GUI_TextBox_Impl    (struct gui_state *g, u32 id, struct gui_arg_txt const *a);

#endif
