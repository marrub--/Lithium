#ifndef LITH_CBI_GUI_H
#define LITH_CBI_GUI_H

// ---------------------------------------------------------------------------
// Node Types.
//

typedef int (*cbi_drawfunc_t)(struct cbi_node_s *, int);
typedef void (*cbi_updatefunc_t)(struct cbi_node_s *, struct player_s *, struct cursor_s);
typedef bool (*cbi_clickfunc_t)(struct cbi_node_s *, struct player_s *, struct cursor_s, bool);
typedef void (*cbi_holdfunc_t)(struct cbi_node_s *, struct player_s *, struct cursor_s, bool);
typedef void (*cbi_buttonevent_t)(struct cbi_button_s *, struct player_s *, bool, bool *) [[__call("ScriptI")]];

typedef struct cbi_node_s
{
   int x, y;
   int id;
   bool visible;
   
   cbi_drawfunc_t Draw;
   cbi_updatefunc_t Update;
   cbi_clickfunc_t Click;
   cbi_holdfunc_t Hold;
   
   struct dlist_s *children;
} cbi_node_t;

typedef struct cbi_text_s
{
   cbi_node_t node;
   __str text;
   __str font;
   bool rainbows;
} cbi_text_t;

typedef struct cbi_sprite_s
{
   cbi_node_t node;
   __str name;
   fixed alpha;
} cbi_sprite_t;

typedef struct cbi_button_s
{
   cbi_node_t node;
   __str label;
   bool hover;
   int clicked;
   
   cbi_buttonevent_t Event;
} cbi_button_t;

typedef struct cbi_tab_s
{
   cbi_node_t node;
   int ntabs;
   int curtab;
   int hover;
   int clicked;
   __str *names;
} cbi_tab_t;

//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Node Functions.
//

#define CBI_InsertNode(list, node) \
   DList_InsertBack(list, (listdata_t){ (node) })

//

enum
{
   NODEAF_NOTVISIBLE = 1 << 0,
};

enum
{
   TXTAF_NOTVISIBLE = 1 << 0,
   TXTAF_RAINBOWS   = 1 << 1,
};

enum
{
   SPRAF_NOTVISIBLE = 1 << 0,
   SPRAF_ALPHA      = 1 << 1,
};

enum
{
   BTNAF_NOTVISIBLE = 1 << 0,
};

enum
{
   TABAF_NOTVISIBLE = 1 << 0,
};

int CBI_NodeDrawList(struct dlist_s *list, int id);
void CBI_NodeUpdateList(struct dlist_s *list, player_t *p, struct cursor_s cur);
bool CBI_NodeClickList(struct dlist_s *list, player_t *p, struct cursor_s cur, bool left);

//
// cbi_node_t

[[__optional_args(4)]]
cbi_node_t *CBI_NodeAlloc(int flags, int id, int x, int y);

//
// cbi_text_t

[[__optional_args(1)]]
cbi_node_t *CBI_TextAlloc(int flags, int id, int x, int y, __str text, __str font);

//
// cbi_sprite_t

[[__optional_args(1)]]
cbi_node_t *CBI_SpriteAlloc(int flags, int id, int x, int y, __str name, fixed alpha);

//
// cbi_button_t

[[__optional_args(1)]]
cbi_node_t *CBI_ButtonAlloc(int flags, int id, int x, int y, cbi_buttonevent_t event, __str label);

//
// cbi_tab_t

cbi_node_t *CBI_TabAlloc(int flags, int id, int x, int y, __str *names);

//
// ---------------------------------------------------------------------------

#endif

