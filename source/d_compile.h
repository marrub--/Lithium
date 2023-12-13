/* Dialogue compiler data. */
#ifndef d_compile_h
#define d_compile_h

#include "m_engine.h"
#include "w_world.h"
#include "p_player.h"
#include "d_vm.h"
#include "m_file.h"
#include "m_tokbuf.h"
#include "m_trie.h"

enum {
   _name_pool_variables,
   _name_pool_max,
};

struct name_pool {
   char     **names;
   mem_size_t num_names;
};

struct compiler {
   struct tokbuf    tb;
   struct err       res;
   struct dlg_def   def;
   struct name_pool nam[_name_pool_max];
   char             name[32];
   i32              pool;
   i32              num;
   i32              page;
};

mem_size_t Dlg_WriteCode(struct dlg_def const *def, mem_size_t c, mem_size_t i);
void Dlg_GetStmt_Asm(struct compiler *d);
script void Dlg_GetStmt(struct compiler *d);
void Dlg_PushB1(struct compiler *d, i32 b);
void Dlg_PushB2(struct compiler *d, i32 word);
void Dlg_PushLdVA(struct compiler *d, i32 action);
struct ptr2 Dlg_PushLdAdr(struct compiler *d, mem_size_t at, i32 set);
void Dlg_SetB1(struct compiler *d, mem_size_t ptr, i32 b);
void Dlg_SetB2(struct compiler *d, mem_size_t ptr, i32 word);
mem_size_t Dlg_PushStr(struct compiler *d, cstr s, mem_size_t l);
void Dlg_GetNamePool(struct compiler *d, mem_size_t which);
i32 Dlg_CheckNamePool(struct compiler *d, mem_size_t which, cstr check);
void Dlg_ClearNamePool(struct compiler *d, mem_size_t which);

#endif
