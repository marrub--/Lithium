// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifndef LITH_GUI_H
#define LITH_GUI_H

#include "lith_cps.h"

#define LineHash ((id_t)__LINE__ * FileHash)

#define Pre(name) .preset = &guipre.name

// Fixed ID
#define Lith_GUI_Button_FId(g, id, ...)      Lith_GUI_Button_Impl     (g, id, &(gui_button_args_t const){__VA_ARGS__})
#define Lith_GUI_Checkbox_FId(g, id, ...)    Lith_GUI_Checkbox_Impl   (g, id, &(gui_checkb_args_t const){__VA_ARGS__})
#define Lith_GUI_ScrollBegin_FId(g, id, ...) Lith_GUI_ScrollBegin_Impl(g, id, &(gui_scroll_args_t const){__VA_ARGS__})
#define Lith_GUI_Slider_FId(g, id, ...)      Lith_GUI_Slider_Impl     (g, id, &(gui_slider_args_t const){__VA_ARGS__})
#define Lith_GUI_TextBox_FId(g, id, ...)     Lith_GUI_TextBox_Impl    (g, id, &(gui_txtbox_args_t const){__VA_ARGS__})

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
      ACS_PrintString(g->gfxprefix); \
      ACS_PrintString(set); \
      set = ACS_EndStrParam(); \
   } else (void)0
#define Lith_GUI_Prefix1(g, pre, mem) \
   (!(pre)->external \
      ? ((pre)->mem ? (ACS_BeginPrint(), ACS_PrintString((g)->gfxprefix), ACS_PrintString((pre)->mem), ACS_EndStrParam()) : null) \
      : (pre)->mem)

#define Lith_GUI_ScrollReset(g, st) \
   (*(st) = (gui_scroll_state_t){})

#define Lith_GUI_Tabs(g, st, names, x, y, yy) \
   do { \
      for(int _i = 0; _i < countof(names); _i++) \
      { \
         if(Lith_GUI_Button_Id(g, _i, names[_i], guipre.btntab.w * _i + (x), \
            guipre.btntab.h * (yy) + (y), _i == *(st), .preset = &guipre.btntab)) \
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
   __with(char const *txt_buf = Lith_CPS_Print(st->txtbuf, st->tbptr);) \
      ifauto(char const *, _c, strchr(txt_buf, '\n')) \
         __with(size_t txt_len = _c - txt_buf; Lith_GUI_TextBox_Reset(st);)

// Types ---------------------------------------------------------------------|

typedef u32 id_t;

typedef struct gui_scroll_state_s
{
   int     ox;
   int     oy;
   int     occludeS;
   int     occludeE;
   fixed64 y;
   fixed64 grabpos;
   bool    grabbed;
   int     nextsnd;
} gui_scroll_state_t;

typedef struct gui_typeon_state_s
{
   __str txt;
   int   len;
   int   pos;
} gui_typeon_state_t;

typedef struct gui_txtbox_state_s
{
   Lith_CPS_Decl(txtbuf, 128);
   int tbptr;
} gui_txtbox_state_t;

typedef struct gui_delta_s
{
   fixed64 cx, cy;

   bool clicklft;
   bool clickrgt;
   bool clickany;
} gui_delta_t;

typedef struct gui_state_s
{
   anonymous
   gui_delta_t cur;
   gui_delta_t old;

   int ox, oy;
   int w, h;

   id_t active, hot, slide;
   int slidecount, slidetime;

   bool useclip;
   int clpxS, clpyS, clpxE, clpyE;

   void *state;

   __str gfxprefix;
} gui_state_t;

typedef struct gui_button_preset_s
{
   __str gfx;
   __str hot;
   __str snd;
   __str cdef;
   __str cact;
   __str chot;
   __str cdis;
   __str font;
   int ax;
   int ay;
   int w;
   int h;
   bool external;
} gui_button_preset_t;

typedef struct gui_button_args_s
{
   __str label;
   int   x, y;
   bool  disabled;
   __str color;
   bool  slide;
   gui_button_preset_t const *preset;
} gui_button_args_t;

typedef struct gui_checkb_preset_s
{
   __str gfx;
   __str hot;
   __str dis;
   __str sndup;
   __str snddn;
   __str chkgfx;
   __str chkhot;
   __str chkact;
   __str chkdis;
   bool  external;
   int   w;
   int   h;
} gui_checkb_preset_t;

typedef struct gui_checkb_args_s
{
   bool on;
   int  x, y;
   bool disabled;
   gui_checkb_preset_t const *preset;
} gui_checkb_args_t;

typedef struct gui_scroll_preset_s
{
   __str capS;
   __str capE;
   __str scrl;
   __str notchgfx;
   __str notchhot;
   bool  external;
   int   scrlw;
   int   scrlh;
} gui_scroll_preset_t;

typedef struct gui_scroll_args_s
{
   gui_scroll_state_t *st;
   int x;
   int y;
   int w;
   int h;
   int contenth;
   int ww;
   gui_scroll_preset_t const *preset;
} gui_scroll_args_t;

typedef struct gui_slider_preset_s
{
   __str gfx;
   __str snd;
   __str notch;
   __str notchhot;
   __str font;
   bool  external;
   int   pad;
   int   w;
   int   h;
} gui_slider_preset_t;

typedef struct gui_slider_args_s
{
   int         x;
   int         y;
   fixed64     minima;
   fixed64     maxima;
   fixed64     val;
   bool        integ;
   char const *suf;
   gui_slider_preset_t const *preset;
} gui_slider_args_t;

typedef struct gui_txtbox_args_s
{
   gui_txtbox_state_t *st;
   int x, y;
   struct player *p;
} gui_txtbox_args_t;

struct gui_presets {
   gui_button_preset_t btndef;
   gui_button_preset_t btntab;
   gui_button_preset_t btnexit;
   gui_button_preset_t btnnext;
   gui_button_preset_t btnprev;
   gui_button_preset_t btnnexts;
   gui_button_preset_t btnprevs;
   gui_button_preset_t btnlist;
   gui_button_preset_t btnlistsel;
   gui_button_preset_t btnlistactive;
   gui_button_preset_t btnlistactivated;
   gui_button_preset_t btnbipmain;
   gui_button_preset_t btnbipback;
   gui_button_preset_t btndlgsel;
   gui_button_preset_t btnclear;
   gui_button_preset_t btnnote;
   gui_checkb_preset_t cbxdef;
   gui_checkb_preset_t cbxsmall;
   gui_scroll_preset_t scrdef;
   gui_slider_preset_t slddef;
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

#ifndef EXTERNAL_CODE
extern struct gui_presets const guipre;
#else
struct gui_presets const *Lith_GUIPreExtern(void);
#define guipre (*Lith_GUIPreExtern())
#endif

// Extern Functions ----------------------------------------------------------|

stkcall optargs(1)
void Lith_GUI_Auto(gui_state_t *g, id_t id, int x, int y, int w, int h, bool slide);

stkcall optargs(1)
void Lith_GUI_Init(gui_state_t *g, void *state);

stkcall
void Lith_GUI_UpdateState(gui_state_t *g, struct player *p);

stkcall optargs(2)
void Lith_GUI_Begin(gui_state_t *g, int w, int h);
stkcall
void Lith_GUI_End(gui_state_t *g, enum cursor curs);

stkcall optargs(1)
void Lith_GUI_Clip(gui_state_t *g, int x, int y, int w, int h, int ww);
stkcall
void Lith_GUI_ClipRelease(gui_state_t *g);

stkcall
__str Lith_RemoveTextColors(__str str, int size);

stkcall
void Lith_GUI_TypeOn(gui_state_t *g, gui_typeon_state_t *typeon, __str text);
stkcall
gui_typeon_state_t const *Lith_GUI_TypeOnUpdate(gui_state_t *g, gui_typeon_state_t *typeon);

void Lith_GUI_ScrollEnd(gui_state_t *g, gui_scroll_state_t *scr);
optargs(1)
bool Lith_GUI_ScrollOcclude(gui_state_t *g, gui_scroll_state_t const *scr, int y, int h);

bool                Lith_GUI_Button_Impl     (gui_state_t *g, id_t id, gui_button_args_t const *a);
bool                Lith_GUI_Checkbox_Impl   (gui_state_t *g, id_t id, gui_checkb_args_t const *a);
void                Lith_GUI_ScrollBegin_Impl(gui_state_t *g, id_t id, gui_scroll_args_t const *a);
fixed64             Lith_GUI_Slider_Impl     (gui_state_t *g, id_t id, gui_slider_args_t const *a);
gui_txtbox_state_t *Lith_GUI_TextBox_Impl    (gui_state_t *g, id_t id, gui_txtbox_args_t const *a);

#endif

