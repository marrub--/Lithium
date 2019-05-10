/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Dialogue compiler data.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef d_compile_h
#define d_compile_h

#include "common.h"
#include "w_world.h"
#include "p_player.h"
#include "d_vm.h"
#include "m_file.h"
#include "m_tokbuf.h"

#include <setjmp.h>

#define Error(d) longjmp((d)->env, 1)

#define ErrF(d, fmt, ...) \
   (Log("%s: " fmt, __func__, __VA_ARGS__), Error(d))

#define Err(d, fmt) \
   (Log("%s: " fmt, __func__), Error(d))

#define ErrT(d, tok, s) \
   (Log("(%i:%i) %s: " s " (%i:'%s')", tok->orig.line, tok->orig.colu, \
        __func__, tok->type, tok->textV ? tok->textV : "<no string>"), \
    Error(d))

#define Expect(d, tok, typ) \
   if(tok->type != typ) ErrT(d, tok, "expected " #typ)

#define Expect2(d, tok, typ1, typ2) \
   if(tok->type != typ1 && tok->type != typ2) \
      ErrT(d, tok, "expected " #typ1 " or " #typ2)

#define Expect3(d, tok, typ1, typ2, typ3) \
   if(tok->type != typ1 && tok->type != typ2 && tok->type != typ3) \
      ErrT(d, tok, "expected " #typ1 ", " #typ2 " or " #typ3)

#define ExpectDrop(d, typ) \
   if(!d->tb.drop(typ)) {\
      struct token *_tok = d->tb.reget(); \
      ErrT(d, _tok, "expected " #typ); \
   }

#define CheckKw(tok, kw) \
   (tok->type == tok_identi && faststrcmp(tok->textV, kw) == 0)

/* Types ------------------------------------------------------------------- */

struct compiler {
   struct tokbuf  tb;
   struct dlg_def def;
   u32            num;
   jmp_buf        env;
   bool           ok;
};

/* Extern Functions -------------------------------------------------------- */

void Dlg_GetStmt_Asm(struct compiler *d);
script void Dlg_GetStmt(struct compiler *d);

void Dlg_PushB1(struct compiler *d, u32 b);
void Dlg_PushB2(struct compiler *d, u32 word);
void Dlg_PushLdVA(struct compiler *d, u32 action);
struct ptr2 Dlg_PushLdAdr(struct compiler *d, u32 at, u32 set);
void Dlg_SetB1(struct compiler *d, u32 ptr, u32 b);
void Dlg_SetB2(struct compiler *d, u32 ptr, u32 word);
u32 Dlg_PushStr(struct compiler *d, cstr s, u32 l);

#endif
