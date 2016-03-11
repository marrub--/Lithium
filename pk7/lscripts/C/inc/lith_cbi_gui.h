#ifndef LITH_CBI_GUI_H
#define LITH_CBI_GUI_H

#define CBI_BUTTON_W 48
#define CBI_BUTTON_H 16

// ---------------------------------------------------------------------------
// Node Types.
//

typedef int (*cbi_drawfunc_t)(struct cbi_node_s *, int);
typedef void (*cbi_updatefunc_t)(struct cbi_node_s *, struct player_s *, cursor_t);
typedef bool (*cbi_clickfunc_t)(struct cbi_node_s *, struct player_s *, cursor_t, bool);
typedef cbi_clickfunc_t cbi_holdfunc_t;

typedef struct cbi_nodefuncs_s
{
   cbi_drawfunc_t Draw;
   cbi_updatefunc_t Update;
   cbi_clickfunc_t Click;
   cbi_holdfunc_t Hold;
} cbi_nodefuncs_t;

typedef struct cbi_node_s
{
   int x, y;
   int id;
   bool visible;
   
   cbi_nodefuncs_t basefuncs;
   cbi_nodefuncs_t userfuncs;
   
   struct dlist_s *children;
} cbi_node_t;

typedef struct cbi_text_s
{
   cbi_node_t node;
   
   __str text;
   __str font;
   bool rainbows;
   fixed alignx;
   fixed aligny;
} cbi_text_t;

typedef struct cbi_sprite_s
{
   cbi_node_t node;
   
   __str name;
   fixed alpha;
   fixed alignx;
   fixed aligny;
} cbi_sprite_t;

typedef struct cbi_button_s
{
   cbi_node_t node;
   
   __str font;
   __str label;
   int clicked;
   int respond;
} cbi_button_t;

//
// ---------------------------------------------------------------------------

enum
{
   // Bits 0 - 4 reserved for generic flags.
   NODEAF_NOTVISIBLE = 1 << 0,
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

#define CBI_InsertNode(list, node) \
   DList_InsertBack(list, (listdata_t){ (node) })

//

int CBI_NodeListDraw(struct dlist_s *list, int id);
void CBI_NodeListUpdate(struct dlist_s *list, player_t *p, cursor_t cur);
bool CBI_NodeListClick(struct dlist_s *list, player_t *p, cursor_t cur, bool left);
bool CBI_NodeListHold(struct dlist_s *list, player_t *p, cursor_t cur, bool left);
cbi_node_t *CBI_NodeListGetByID(struct dlist_s *list, int id);

//
// cbi_node_t

[[__optional_args(5)]]
cbi_node_t *CBI_NodeAlloc(int flags, int id, int x, int y, cbi_nodefuncs_t *userfuncs);

//
// cbi_text_t

[[__optional_args(2)]]
cbi_node_t *CBI_TextAlloc(int flags, int id, int x, int y, cbi_nodefuncs_t *userfuncs, __str text, __str font);

//
// cbi_sprite_t

[[__optional_args(2)]]
cbi_node_t *CBI_SpriteAlloc(int flags, int id, int x, int y, cbi_nodefuncs_t *userfuncs, __str name, fixed alpha);

//
// cbi_button_t

[[__optional_args(2)]]
cbi_node_t *CBI_ButtonAlloc(int flags, int id, int x, int y, cbi_nodefuncs_t *userfuncs, __str label, __str font);

//
// ---------------------------------------------------------------------------

#endif

