#ifndef LITH_GUI_H
#define LITH_GUI_H

#define LineHash ((id_t)__LINE__ * StrHash(__FILE__))

// fug
#define Lith_GUI_Button(g, ...)             Lith_GUI_Button_Id(g, 0, __VA_ARGS__)
#define Lith_GUI_Button_Id(g, id, ...)      Lith_GUI_Button_Impl(g, id + LineHash, &(gui_button_args_t){__VA_ARGS__})
#define Lith_GUI_Checkbox(g, ...)           Lith_GUI_Checkbox_Id(g, 0, __VA_ARGS__)
#define Lith_GUI_Checkbox_Id(g, id, ...)    Lith_GUI_Checkbox_Impl(g, id + LineHash, &(gui_checkb_args_t){__VA_ARGS__})
#define Lith_GUI_ScrollBegin(g, ...)        Lith_GUI_ScrollBegin_Id(g, 0, __VA_ARGS__)
#define Lith_GUI_ScrollBegin_Id(g, id, ...) Lith_GUI_ScrollBegin_Impl(g, id + LineHash, &(gui_scroll_args_t){__VA_ARGS__})
#define Lith_GUI_Slider(g, ...)             Lith_GUI_Slider_Id(g, 0, __VA_ARGS__)
#define Lith_GUI_Slider_Id(g, id, ...)      Lith_GUI_Slider_Impl(g, id + LineHash, &(gui_slider_args_t){__VA_ARGS__})
#define Lith_GUI_TextBox(g, ...)            Lith_GUI_TextBox_Id(g, 0, __VA_ARGS__)
#define Lith_GUI_TextBox_Id(g, id, ...)     Lith_GUI_TextBox_Impl(g, id + LineHash, &(gui_textbox_args_t){__VA_ARGS__})

#define Lith_GUI_GenPreset(type, def) \
   type pre; \
   if(a->preset) pre = *a->preset; \
   else          pre = def

#define Lith_GUI_Prefix(set) if(set) set = StrParam("%S%S", g->gfxprefix, set)

#define Lith_GUI_ScrollReset(g, stn) \
   memset(&g->st[stn].scrl, 0, sizeof(gui_scroll_state_t));

#define BtnDefault \
   .snd  = "player/cbi/buttonpress", \
   .cdef = "j", \
   .cact = "g", \
   .chot = "k", \
   .font = "CBIFONT", \
   .gfx  = "Button.png", \
   .hot  = "ButtonHot.png", \
   .cdis = "m", \
   .w = 48, \
   .h = 16

#define CbxDefault \
   .sndup  = "player/cbi/clickon", \
   .snddn  = "player/cbi/clickoff", \
   .w = 10, \
   .h = 10, \
   .font = "CNFONT", \
   .chkgfx = "CheckboxX.png", \
   .chkhot = "CheckboxXHot.png", \
   .chkact = "CheckboxXActive.png", \
   .chkdis = "CheckboxXDisabled.png", \
   .gfx    = "Checkbox.png", \
   .hot    = "CheckboxHot.png", \
   .dis    = "CheckboxDisabled.png"


//----------------------------------------------------------------------------
// Type Definitions
//

typedef unsigned id_t;

typedef struct gui_scroll_state_s
{
   int ox;
   int oy;
   int occludeS;
   int occludeE;
   double y;
   double grabpos;
   bool grabbed;
} gui_scroll_state_t;

typedef struct gui_typeon_state_s
{
   __str txt;
   int   len;
   int   pos;
} gui_typeon_state_t;

typedef struct gui_textbox_state_s
{
   char txtbuf[32];
   int tbptr;
} gui_textbox_state_t;

typedef union gui_stateitem_s
{
   int i;
   void *vp;
   gui_scroll_state_t scrl;
   gui_typeon_state_t type;
   gui_textbox_state_t tb;
} gui_stateitem_t;

typedef struct gui_delta_s
{
   float cx, cy;
   bool clicklft : 1;
   bool clickrgt : 1;
   bool clickany : 1;
} gui_delta_t;

typedef struct gui_state_s
{
   [[__anonymous]] gui_delta_t cur;
   gui_delta_t old;
   
   int hid;
   int ox, oy;
   int w, h;
   
   id_t active, hot;
   
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
   bool  external;
   int   w;
   int   h;
} gui_button_preset_t;

typedef struct gui_button_args_s
{
   __str label;
   int   x, y;
   bool  disabled;
   __str color;
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
   __str font;
   int w;
   int h;
} gui_checkb_preset_t;

typedef struct gui_checkb_args_s
{
   bool on;
   int x, y;
   __str label;
   bool disabled;
   gui_checkb_preset_t const *preset;
} gui_checkb_args_t;

typedef struct gui_scroll_preset_s
{
   __str capS;
   __str capE;
   __str scrl;
   int scrlw;
   int scrlh;
   __str notchgfx;
   __str notchhot;
} gui_scroll_preset_t;

typedef struct gui_scroll_args_s
{
   size_t st;
   int x;
   int y;
   int w;
   int h;
   int contenth;
   gui_scroll_preset_t const *preset;
} gui_scroll_args_t;

typedef struct gui_slider_preset_s
{
   __str gfx;
   __str snd;
   __str notch;
   __str notchhot;
   int pad;
   int w;
   int h;
} gui_slider_preset_t;

typedef struct gui_slider_args_s
{
   int x;
   int y;
   double minima;
   double maxima;
   double val;
   bool integ;
   gui_slider_preset_t const *preset;
} gui_slider_args_t;

typedef struct gui_textbox_args_s
{
   size_t st;
   int x;
   int y;
   int pnum;
   char *inbuf;
} gui_textbox_args_t;


//----------------------------------------------------------------------------
// Extern Objects
//

extern gui_button_preset_t const btndefault;
extern gui_button_preset_t const btntab;
extern gui_button_preset_t const btnexit;
extern gui_button_preset_t const btnnext;
extern gui_button_preset_t const btnprev;
extern gui_button_preset_t const btnnexts;
extern gui_button_preset_t const btnprevs;
extern gui_button_preset_t const btnlist;
extern gui_button_preset_t const btnlistsel;
extern gui_button_preset_t const btnlistactive;
extern gui_button_preset_t const btnlistactivated;
extern gui_button_preset_t const btnbipmain;
extern gui_button_preset_t const btnbipback;
extern gui_button_preset_t const btndlgsel;
extern gui_checkb_preset_t const cbxdefault;
extern gui_checkb_preset_t const cbxsmall;
extern gui_scroll_preset_t const scrdefault;
extern gui_slider_preset_t const slddefault;


//----------------------------------------------------------------------------
// Extern Functions
//

void Lith_GUI_Auto(gui_state_t *g, id_t id, int x, int y, int w, int h);
void Lith_GUI_Init(gui_state_t *g, size_t maxst);
void Lith_GUI_UpdateState(gui_state_t *g, struct player *p);
[[__optional_args(2)]] void Lith_GUI_Begin(gui_state_t *g, int basehid, int w, int h);
void Lith_GUI_End(gui_state_t *g);
void Lith_GUI_Clip(gui_state_t *g, int x, int y, int w, int h);
void Lith_GUI_ClipRelease(gui_state_t *g);
void Lith_GUI_TypeOn(gui_state_t *g, size_t st, __str text);
__str Lith_RemoveTextColors(__str str, int size);
gui_typeon_state_t const *Lith_GUI_TypeOnUpdate(gui_state_t *g, size_t st);

bool Lith_GUI_Button_Impl(gui_state_t *g, id_t id, gui_button_args_t *a);
bool Lith_GUI_Checkbox_Impl(gui_state_t *g, id_t id, gui_checkb_args_t *a);
void Lith_GUI_ScrollBegin_Impl(gui_state_t *g, id_t id, gui_scroll_args_t *a);
void Lith_GUI_ScrollEnd(gui_state_t *g, size_t st);
[[__optional_args(1)]] bool Lith_GUI_ScrollOcclude(gui_state_t *g, size_t st, int y, int h);
double Lith_GUI_Slider_Impl(gui_state_t *g, id_t id, gui_slider_args_t *a);
gui_textbox_state_t *Lith_GUI_TextBox_Impl(gui_state_t *g, id_t id, gui_textbox_args_t *a);

#endif

