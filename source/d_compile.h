// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Dialogue compiler data.                                                  │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef d_compile_h
#define d_compile_h

#include "common.h"
#include "w_world.h"
#include "p_player.h"
#include "d_vm.h"
#include "m_file.h"
#include "m_tokbuf.h"
#include "m_trie.h"

enum {
   _name_pool_program,
   _name_pool_dialogue,
   _name_pool_terminal,
   _name_pool_variables,
   _name_pool_pages,
   _name_pool_max,
};

struct name_pool {
   char     **names;
   mem_size_t num_names;
};

struct compiler {
   struct tokbuf    tb;
   struct tbuf_err  res;
   struct dlg_def   def;
   struct name_pool nam[_name_pool_max];
   i32              pool;
   i32              num;
   i32              page;
   i32              name;
};

u32 Dlg_WriteCode(struct dlg_def const *def, u32 c, u32 i);

void Dlg_GetStmt_Asm(struct compiler *d);
script void Dlg_GetStmt(struct compiler *d);

void Dlg_PushB1(struct compiler *d, u32 b);
void Dlg_PushB2(struct compiler *d, u32 word);
void Dlg_PushLdVA(struct compiler *d, u32 action);
struct ptr2 Dlg_PushLdAdr(struct compiler *d, u32 at, u32 set);
void Dlg_SetB1(struct compiler *d, u32 ptr, u32 b);
void Dlg_SetB2(struct compiler *d, u32 ptr, u32 word);
u32 Dlg_PushStr(struct compiler *d, cstr s, u32 l);

void Dlg_GetNamePool(struct compiler *d, i32 which);
i32 Dlg_CheckNamePool(struct compiler *d, i32 which, cstr check);
void Dlg_ClearNamePool(struct compiler *d, i32 which);

#endif
