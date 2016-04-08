#ifndef LITH_CBI_GUI_H
#define LITH_CBI_GUI_H

#define UI_BUTTON_W 48
#define UI_BUTTON_H 16

#define UI_TAB_W 48
#define UI_TAB_H 11

#define UI_LIST_W 64
#define UI_LIST_H 8

#define UI_LISTBTN_W 64
#define UI_LISTBTN_H 12

#define UI_LISTSCR_W 4
#define UI_LISTSCR_H 8

#define UI_LISTCAP_W 4
#define UI_LISTCAP_H 4

// ---------------------------------------------------------------------------
// Node Types.
//

typedef int (*ui_drawfunc_t)(struct ui_node_s *, int);
typedef void (*ui_updatefunc_t)(struct ui_node_s *, struct player_s *, cursor_t);
typedef bool (*ui_clickfunc_t)(struct ui_node_s *, struct player_s *, cursor_t, bool);
typedef ui_clickfunc_t ui_holdfunc_t;

typedef struct ui_nodefuncs_s
{
   ui_drawfunc_t PreDraw;
   ui_drawfunc_t Draw;
   ui_drawfunc_t PostDraw;
   ui_updatefunc_t Update;
   ui_clickfunc_t Click;
   ui_holdfunc_t Hold;
} ui_nodefuncs_t;

typedef struct ui_node_s
{
   int x, y;
   int id;
   bool visible;
   
   ui_nodefuncs_t basefuncs;
   ui_nodefuncs_t userfuncs;
   
   struct ui_node_s *parent;
   struct dlist_s *children;
} ui_node_t;

typedef struct ui_text_s
{
   ui_node_t node;
   
   __str text;
   __str font;
   bool rainbows;
   fixed alignx;
   fixed aligny;
} ui_text_t;

typedef struct ui_sprite_s
{
   ui_node_t node;
   
   __str name;
   fixed alpha;
   fixed alignx;
   fixed aligny;
} ui_sprite_t;

typedef struct ui_button_s
{
   ui_node_t node;
   
   __str font;
   __str label;
   int clicked;
   int respond;
} ui_button_t;

typedef struct ui_tab_s
{
   ui_node_t node;
   
   __str *names;
   int curtab;
   int hover;
   int clicked;
   int ntabs;
} ui_tab_t;

typedef struct ui_list_s
{
   ui_node_t node;
   
   __str *labels;
   int nlabels;
   int hover;
   int height;
   int selected;
   int clicked;
} ui_list_t;

//
// ---------------------------------------------------------------------------

enum
{
   // Bits 0 - 4 reserved for generic flags.
   NODEAF_NOTVISIBLE    = 1 << 0,
   NODEAF_ALLOCCHILDREN = 1 << 1,
};

enum
{
   TXTAF_RAINBOWS      = 1 << 5,
   TXTAF_ALIGNX_CENTER = 1 << 6,
   TXTAF_ALIGNX_RIGHT  = 1 << 7,
   TXTAF_ALIGNY_CENTER = 1 << 8,
   TXTAF_ALIGNY_BOTTOM = 1 << 9,
};

enum
{
   SPRAF_ALPHA         = 1 << 5,
   SPRAF_ALIGNX_CENTER = 1 << 6,
   SPRAF_ALIGNX_RIGHT  = 1 << 7,
   SPRAF_ALIGNY_CENTER = 1 << 8,
   SPRAF_ALIGNY_BOTTOM = 1 << 9,
};

enum
{
   BTNAF_RESPOND_LEFT  = 1 << 5,
   BTNAF_RESPOND_RIGHT = 1 << 6,
   
   BTNAF_RESPOND_BOTH = BTNAF_RESPOND_LEFT | BTNAF_RESPOND_RIGHT,
};

// ---------------------------------------------------------------------------
// Node Functions.
//

void UI_InsertNode(ui_node_t *parent, ui_node_t *child);

int UI_NodeListDraw(struct dlist_s *list, int id);
void UI_NodeListUpdate(struct dlist_s *list, player_t *p, cursor_t cur);
bool UI_NodeListClick(struct dlist_s *list, player_t *p, cursor_t cur, bool left);
bool UI_NodeListHold(struct dlist_s *list, player_t *p, cursor_t cur, bool left);
ui_node_t *UI_NodeListGetByID(struct dlist_s *list, int id);

//
// ui_node_t

int UI_NodeDraw(ui_node_t *node, int id);
void UI_NodeUpdate(ui_node_t *node, player_t *p, cursor_t cur);
bool UI_NodeClick(ui_node_t *node, player_t *p, cursor_t cur, bool left);
bool UI_NodeHold(ui_node_t *node, player_t *p, cursor_t cur, bool left);
void UI_NodeReset(ui_node_t *node, int flags, int id, int x, int y, ui_nodefuncs_t *userfuncs);

[[__optional_args(5)]]
ui_node_t *UI_NodeAlloc(int flags, int id, int x, int y, ui_nodefuncs_t *userfuncs);

//
// ui_text_t

[[__optional_args(2)]]
ui_node_t *UI_TextAlloc(int flags, int id, int x, int y, ui_nodefuncs_t *userfuncs, __str text, __str font);

//
// ui_sprite_t

[[__optional_args(2)]]
ui_node_t *UI_SpriteAlloc(int flags, int id, int x, int y, ui_nodefuncs_t *userfuncs, __str name, fixed alpha);

//
// ui_button_t

[[__optional_args(2)]]
ui_node_t *UI_ButtonAlloc(int flags, int id, int x, int y, ui_nodefuncs_t *userfuncs, __str label, __str font);

//
// ui_tab_t

[[__optional_args(1)]]
ui_node_t *UI_TabAlloc(int flags, int id, int x, int y, ui_nodefuncs_t *userfuncs, __str *names);

//
// ui_list_t

[[__optional_args(2)]]
ui_node_t *UI_ListAlloc(int flags, int id, int x, int y, ui_nodefuncs_t *userfuncs, __str *labels, int height);

//
// ---------------------------------------------------------------------------

#endif

