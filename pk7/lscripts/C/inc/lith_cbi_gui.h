#ifndef LITH_CBI_GUI_H
#define LITH_CBI_GUI_H

#define LineHash ((id_t)__LINE__ * StrHash(__FILE__))
#define Lith_GUI_Button(g, ...) Lith_GUI_Button_Id(g, 0, __VA_ARGS__)
#define Lith_GUI_Button_Id(g, id, ...) Lith_GUI_Button_Impl(g, id + LineHash, &(gui_button_args_t){__VA_ARGS__})
#define Lith_GUI_ScrollBegin(g, ...) Lith_GUI_ScrollBegin_Id(g, 0, __VA_ARGS__)
#define Lith_GUI_ScrollBegin_Id(g, id, ...) Lith_GUI_ScrollBegin_Impl(g, id + LineHash, &(gui_scroll_args_t){__VA_ARGS__})
#define Lith_GUI_Slider(g, ...) Lith_GUI_Slider_Id(g, 0, __VA_ARGS__)
#define Lith_GUI_Slider_Id(g, id, ...) Lith_GUI_Slider_Impl(g, id + LineHash, &(gui_slider_args_t){__VA_ARGS__})


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
   float y;
} gui_scroll_state_t;

typedef union gui_stateitem_s
{
   int i;
   gui_scroll_state_t scrl;
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
   
   id_t active, hot;
   
   bool useclip;
   int clpxS, clpyS, clpxE, clpyE;
   
   gui_stateitem_t *st;
} gui_state_t;

typedef struct gui_button_preset_s
{
   __str  gfx;
   __str  hot;
   __str  snd;
   char  cdef;
   char  cact;
   char  chot;
   char  cdis;
   __str font;
   int   w;
   int   h;
} gui_button_preset_t;

typedef struct gui_button_args_s
{
   __str label;
   int x, y;
   bool disabled;
   gui_button_preset_t const *preset;
} gui_button_args_t;

typedef struct gui_scroll_preset_s
{
   __str capS;
   __str capE;
   __str scrl;
   int scrlw;
   int scrlh;
   __str notchgfx;
   __str notchhot;
   int notchofs;
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
   float minima;
   float maxima;
   float val;
   gui_slider_preset_t const *preset;
} gui_slider_args_t;


//----------------------------------------------------------------------------
// Extern Objects
//

extern gui_button_preset_t const btndefault;
extern gui_button_preset_t const btntab;
extern gui_button_preset_t const btnexit;
extern gui_button_preset_t const btnlist;
extern gui_button_preset_t const btnbipmain;
extern gui_button_preset_t const btnbipback;
extern gui_scroll_preset_t const scrdefault;
extern gui_slider_preset_t const slddefault;


//----------------------------------------------------------------------------
// Extern Functions
//

void Lith_GUI_Init(gui_state_t *g, size_t maxst);
void Lith_GUI_UpdateState(gui_state_t *g, struct player_s *p);
void Lith_GUI_Begin(gui_state_t *g);
void Lith_GUI_End(gui_state_t *g);
void Lith_GUI_Clip(gui_state_t *g, int x, int y, int w, int h);
void Lith_GUI_ClipRelease(gui_state_t *g);
bool Lith_GUI_Button_Impl(gui_state_t *g, id_t id, gui_button_args_t *a);
void Lith_GUI_ScrollBegin_Impl(gui_state_t *g, id_t id, gui_scroll_args_t *a);
void Lith_GUI_ScrollEnd(gui_state_t *g, size_t st);
[[__optional_args(1)]] bool Lith_GUI_ScrollOcclude(gui_state_t *g, size_t st, int y, int h);
float Lith_GUI_Slider_Impl(gui_state_t *g, id_t id, gui_slider_args_t *a);

#endif

