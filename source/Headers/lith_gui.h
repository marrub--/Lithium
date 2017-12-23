// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
// vim: columns=140
#ifndef LITH_GUI_H
#define LITH_GUI_H

#define LineHash ((id_t)__LINE__ * FileHash)

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

#define Lith_GUI_Prefix(set) if(set) set = StrParam("%S%S", g->gfxprefix, set)
#define Lith_GUI_Prefix1(g, pre, mem) \
   (!(pre)->external \
      ? ((pre)->mem ? StrParam("%S%S", (g)->gfxprefix, (pre)->mem) : null) \
      : (pre)->mem)

#define Lith_GUI_ScrollReset(g, stn) \
   ((g)->st[(stn)].scrl = (gui_scroll_state_t){})

#define Lith_GUI_Tabs(g, stname, names, x, y, yy) \
   do { \
      for(int _i = 0; _i < countof(names); _i++) \
      { \
         if(Lith_GUI_Button_Id(g, _i, names[_i], guipre.btntab.w * _i + (x), \
            guipre.btntab.h * (yy) + (y), _i == g->st[stname].i, .preset = &guipre.btntab)) \
         { \
            g->st[stname].i = _i; \
         } \
      } \
   } while(0)

// Types ---------------------------------------------------------------------|

typedef unsigned id_t;

typedef struct gui_scroll_state_s
{
   int    ox;
   int    oy;
   int    occludeS;
   int    occludeE;
   double y;
   double grabpos;
   bool   grabbed;
} gui_scroll_state_t;

typedef struct gui_typeon_state_s
{
   __str txt;
   int   len;
   int   pos;
} gui_typeon_state_t;

typedef struct gui_txtbox_state_s
{
   char txtbuf[32];
   int  tbptr;
} gui_txtbox_state_t;

typedef union gui_stateitem_s
{
   int   i;
   void *vp;
   gui_scroll_state_t scrl;
   gui_typeon_state_t type;
   gui_txtbox_state_t tb;
} gui_stateitem_t;

typedef struct gui_delta_s
{
   float cx, cy;
   bool  clicklft : 1;
   bool  clickrgt : 1;
   bool  clickany : 1;
} gui_delta_t;

typedef struct gui_state_s
{
   [[__anonymous]] gui_delta_t cur;
   gui_delta_t old;

   int hid;
   int ox, oy;
   int w, h;

   id_t active, hot, slide;
   int slidecount, slidetime;

   bool useclip;
   int clpxS, clpyS, clpxE, clpyE;

   gui_stateitem_t *st;

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
   int   w;
   int   h;
   bool  external;
} gui_button_preset_t;

typedef struct gui_button_args_s
{
   __str  label;
   int    x, y;
   bool   disabled;
   __str  color;
   bool   slide;
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
   size_t st;
   int    x;
   int    y;
   int    w;
   int    h;
   int    contenth;
   int    ww;
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
   int    x;
   int    y;
   double minima;
   double maxima;
   double val;
   bool   integ;
   __str  suf;
   gui_slider_preset_t const *preset;
} gui_slider_args_t;

typedef struct gui_txtbox_args_s
{
   size_t st;
   int    x;
   int    y;
   int    pnum;
   char  *inbuf;
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
   gui_checkb_preset_t cbxdef;
   gui_checkb_preset_t cbxsmall;
   gui_scroll_preset_t scrdef;
   gui_slider_preset_t slddef;
};

// Extern Objects ------------------------------------------------------------|

#ifndef EXTERNAL_CODE
extern struct gui_presets const guipre;
#else
struct gui_presets const *Lith_GUIPreExtern(void);
#define guipre (*Lith_GUIPreExtern())
#endif

// Extern Functions ----------------------------------------------------------|

[[__optional_args(1)]]
void Lith_GUI_Auto(gui_state_t *g, id_t id, int x, int y, int w, int h, bool slide);
void Lith_GUI_Init(gui_state_t *g, size_t maxst);
void Lith_GUI_UpdateState(gui_state_t *g, struct player *p);
[[__optional_args(2)]]
void Lith_GUI_Begin(gui_state_t *g, int basehid, int w, int h);
void Lith_GUI_End(gui_state_t *g);
[[__optional_args(1)]]
void Lith_GUI_Clip(gui_state_t *g, int x, int y, int w, int h, int ww);
void Lith_GUI_ClipRelease(gui_state_t *g);
void Lith_GUI_TypeOn(gui_state_t *g, size_t st, __str text);
__str Lith_RemoveTextColors(__str str, int size);
gui_typeon_state_t const *Lith_GUI_TypeOnUpdate(gui_state_t *g, size_t st);
void Lith_GUI_ScrollEnd(gui_state_t *g, size_t st);
[[__optional_args(1)]]
bool Lith_GUI_ScrollOcclude(gui_state_t *g, size_t st, int y, int h);

bool                Lith_GUI_Button_Impl     (gui_state_t *g, id_t id, gui_button_args_t const *a);
bool                Lith_GUI_Checkbox_Impl   (gui_state_t *g, id_t id, gui_checkb_args_t const *a);
void                Lith_GUI_ScrollBegin_Impl(gui_state_t *g, id_t id, gui_scroll_args_t const *a);
double              Lith_GUI_Slider_Impl     (gui_state_t *g, id_t id, gui_slider_args_t const *a);
gui_txtbox_state_t *Lith_GUI_TextBox_Impl    (gui_state_t *g, id_t id, gui_txtbox_args_t const *a);

#endif

