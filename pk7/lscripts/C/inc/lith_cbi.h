#ifndef LITH_CBI_H
#define LITH_CBI_H

// ---------------------------------------------------------------------------
// GUI
//

// IDs.
#define GUI_INVALID_ID (-1)
#define GUI_EMPTY_ID (0)
#define GUI_ID(s) ((s[0] << 24) | (s[1] << 16) | (s[2] << 8) | (s[3] << 0))

// Bounding sizes.
#define GUI_BUTTON_W (48)
#define GUI_BUTTON_H (16)

#define GUI_TAB_W (48)
#define GUI_TAB_H (11)

#define GUI_LISTSCR_W (4)
#define GUI_LISTSCR_H (8)

#define GUI_LISTBTN_W (64)
#define GUI_LISTBTN_H (12)

#define GUI_LISTCAP_W (4)
#define GUI_LISTCAP_H (4)

#define GUI_LISTSCR_NOTCH_OFS (3)

// Function macros.
#define GUI_LIST_OFFSETS(i, max, height, pos) \
   /* ＨＯＲＲＩＢＬＥ ＡＲＴ ＯＦ ＨＡＸ ＷＡＳ ＰＡＳＳＥＤ ＤＯＷＮ ＦＯＲ ２０００ ＹＥＡＲＳ 。 */ \
   int addy = (GUI_LISTBTN_H * (i)) - \
      (int)(kbits(pos) * ((GUI_LISTBTN_H * (max)) - (GUI_LISTBTN_H * ((height) / GUI_LISTBTN_H)))); \
   if(addy > GUI_LISTBTN_H * (max)) \
      break; \
   else if(addy + GUI_LISTBTN_H < 0) \
      continue;
//
#define GUI_BEGIN_LIST(id, gst, hid, x, y, h, st) \
   GUI_ScrollBar(id, gst, hid, x, y, h, st, &gui_listscrparm); \
   GUI_BeginOffset(gst, x + GUI_LISTSCR_W, y); \
   GUI_BeginClip(gst, x + GUI_LISTSCR_W, y, GUI_LISTBTN_W, GUI_LISTSCR_H * 19); \
//
#define GUI_END_LIST(gst) \
   GUI_EndClip(gst); \
   GUI_EndOffset(gst);
//

//
// gui_click_t
//

typedef enum gui_click_e
{
   GUI_CLICK_NONE,
   GUI_CLICK_LEFT  = 1 << 0,
   GUI_CLICK_RIGHT = 1 << 1,
   GUI_CLICK_BOTH  = GUI_CLICK_LEFT | GUI_CLICK_RIGHT
} gui_click_t;

//
// gui_cursor_t
//

typedef struct gui_cursor_s
{
   fixed x, y;
   gui_click_t click;
} gui_cursor_t;

//
// gui_state_t
//

typedef struct gui_state_s
{
   gui_cursor_t cur;
   int active, hot;
   int ofsx, ofsy;
   struct gui_clip_s {
      int x, y, w, h;
      bool on;
   } clip;
} gui_state_t;

//
// gui_button_parm_t
//
// GUI_Button structured parameters.
//

typedef struct gui_button_parm_s
{
   int clickmask;
   char c_default, c_active, c_hot, c_inactive;
   __str f_font, f_gfx_def, f_gfx_hot;
   __str s_clicked;
   int dim_x, dim_y;
} gui_button_parm_t;

//
// gui_scroll_parm_t
//
// GUI_ScrollBar structured parameters.
//

typedef struct gui_scroll_parm_s
{
   int clickmask;
   bool vertical;
   __str f_gfx_capS, f_gfx_capE, f_gfx_scrl;
   __str f_gfx_notch, f_gfx_notch_hot;
   int dim_cap_x, dim_cap_y, dim_scrl_x, dim_scrl_y;
   int ofs_notch;
} gui_scroll_parm_t;

//
// GUI presets.

static gui_button_parm_t const gui_tabparm = {
   .f_gfx_def = "lgfx/UI/Tab.png",
   .f_gfx_hot = "lgfx/UI/TabHot.png",
   .c_inactive = 'n',
   .dim_x = GUI_TAB_W,
   .dim_y = GUI_TAB_H
};

static gui_button_parm_t const gui_exitparm = {
   .f_gfx_def = "lgfx/UI/ExitButton.png",
   .f_gfx_hot = "lgfx/UI/ExitButtonHot.png",
   .dim_x = 11,
   .dim_y = 11
};

static gui_button_parm_t const gui_listbtnparm = {
   .f_gfx_def = "lgfx/UI/ListButton.png",
   .f_gfx_hot = "lgfx/UI/ListButtonHot.png",
   .dim_x = GUI_LISTBTN_W,
   .dim_y = GUI_LISTBTN_H
};

static gui_scroll_parm_t const gui_listscrparm = {
   .vertical = true,
   .f_gfx_capS = "lgfx/UI/ListCapTop.png",
   .f_gfx_capE = "lgfx/UI/ListCapBottom.png",
   .f_gfx_scrl = "lgfx/UI/ListScrollbar.png",
   .f_gfx_notch = "lgfx/UI/ListScrollNotch.png",
   .f_gfx_notch_hot = "lgfx/UI/ListScrollNotchHot.png",
   .dim_cap_x = GUI_LISTCAP_W,
   .dim_cap_y = GUI_LISTCAP_H,
   .dim_scrl_x = GUI_LISTSCR_W,
   .dim_scrl_y = GUI_LISTSCR_H,
   .ofs_notch = GUI_LISTSCR_NOTCH_OFS
};

//
// GUI functions.

[[__optional_args(3)]]
bool GUI_Button(int id, gui_state_t *gst, int *hid, int x, int y, __str text, bool inactive, gui_button_parm_t const *parm_);
[[__optional_args(1)]]
bool GUI_ScrollBar(int id, gui_state_t *gst, int *hid, int x, int y, int size, int *pos, gui_scroll_parm_t const *parm_);

void GUI_BeginOffset(gui_state_t *gst, int x, int y);
void GUI_EndOffset(gui_state_t *gst);
void GUI_BeginClip(gui_state_t *gst, int x, int y, int w, int h);
void GUI_EndClip(gui_state_t *gst);
void GUI_Begin(gui_state_t *gst);
void GUI_End(gui_state_t *gst);

// ---------------------------------------------------------------------------
// BIP - Biotic Information Panel
//

typedef struct bip_s
{
   void *no_data;
} bip_t;

// ---------------------------------------------------------------------------
// CBI - Computer/Brain Interface
//

enum
{
   TAB_UPGRADES,
   TAB_SHOP,
   TAB_BIP,
   TAB_STATISTICS,
   TAB_SETTINGS
};

enum
{
   CBI_TABST_MAIN,
   CBI_TABST_NUM
};

enum
{
   CBI_SCRLST_UPGRADES,
   CBI_SCRLST_SHOP,
   CBI_SCRLST_NUM
};

enum
{
   CBI_LSTST_UPGRADES,
   CBI_LSTST_SHOP,
   CBI_LSTST_NUM
};

typedef struct cbi_s
{
   struct {
      gui_state_t ggst;
      int tabst[CBI_TABST_NUM];
      int scrlst[CBI_SCRLST_NUM];
      int lstst[CBI_LSTST_NUM];
   } gst;
   bool open;
} cbi_t;

[[__call("ScriptS"), __extern("ACS"), __script("Net")]]
void Lith_KeyOpenCBI(void);

[[__call("ScriptS")]]
void Lith_PlayerUpdateCBI(struct player_s *p);

#endif

