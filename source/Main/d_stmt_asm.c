/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Dialogue compiler assembly statements.
 *
 * ---------------------------------------------------------------------------|
 */

#if LITHIUM
#include "d_compile.h"

/* Static Functions -------------------------------------------------------- */

static i32 CodeABS(struct compiler *d, cstr reg)
{
   struct token *tok = d->tb.get();

   if(tok->type == tok_number) {
      i32 n = strtoi(tok->textV, nil, 0);

      if(n <= 0xFFFF) {
         if(!reg) {
            return n;
         } else if(d->tb.drop(tok_comma)) {
            tok = d->tb.get();

            if(CheckKw(tok, reg)) return n;

            d->tb.unget();
         }
      }
   }

   d->tb.unget();

   return false;
}

static i32 CodeZPG(struct compiler *d, cstr reg)
{
   struct token *tok = d->tb.get();

   if(tok->type == tok_number) {
      i32 n = strtoi(tok->textV, nil, 0);

      if(n <= 0xFF) {
         if(!reg) {
            return n;
         } else if(d->tb.drop(tok_comma)) {
            tok = d->tb.get();

            if(CheckKw(tok, reg)) return n;

            d->tb.unget();
         }
      }
   }

   d->tb.unget();

   return -1;
}

static bool CodeAI(struct compiler *d, u32 code)
{
   ifw(i32 n = CodeABS(d, nil), n > 0) {
      Dlg_PushB1(d, code);
      Dlg_PushB2(d, n);
      return true;
   }

   return false;
}

static bool CodeAX(struct compiler *d, u32 code)
{
   ifw(i32 n = CodeABS(d, "X"), n > 0) {
      Dlg_PushB1(d, code);
      Dlg_PushB2(d, n);
      return true;
   }

   return false;
}

static bool CodeAY(struct compiler *d, u32 code)
{
   ifw(i32 n = CodeABS(d, "Y"), n > 0) {
      Dlg_PushB1(d, code);
      Dlg_PushB2(d, n);
      return true;
   }

   return false;
}

static bool CodeII(struct compiler *d, u32 code)
{
   if(d->tb.drop(tok_pareno)) {
      struct token *tok = d->tb.get();

      if(tok->type == tok_number) {
         i32 n = strtoi(tok->textV, nil, 0);

         if(n <= 0xFFFF && d->tb.drop(tok_parenc)) {
            Dlg_PushB1(d, code);
            Dlg_PushB2(d, n);
            return true;
         }
      }

      d->tb.unget();
   }

   return false;
}

static bool CodeIX(struct compiler *d, u32 code)
{
   if(d->tb.drop(tok_pareno)) {
      struct token *tok = d->tb.get();

      if(tok->type == tok_number) {
         i32 n = strtoi(tok->textV, nil, 0);

         if(n <= 0xFF && d->tb.drop(tok_comma)) {
            tok = d->tb.get();

            if(CheckKw(tok, "X") && d->tb.drop(tok_parenc)) {
               Dlg_PushB1(d, code);
               Dlg_PushB1(d, n);
               return true;
            }

            d->tb.unget();
         }
      }

      d->tb.unget();
   }

   return false;
}

static bool CodeIY(struct compiler *d, u32 code)
{
   if(d->tb.drop(tok_pareno)) {
      struct token *tok = d->tb.get();

      if(tok->type == tok_number) {
         i32 n = strtoi(tok->textV, nil, 0);

         if(n <= 0xFF && d->tb.drop(tok_parenc) && d->tb.drop(tok_comma)) {
            tok = d->tb.get();

            if(CheckKw(tok, "Y")) {
               Dlg_PushB1(d, code);
               Dlg_PushB1(d, n);
               return true;
            }

            d->tb.unget();
         }
      }

      d->tb.unget();
   }

   return false;
}

#define CodeNP(d, code) (Dlg_PushB1(d, code), true)
#define CodeRI CodeZI

static bool CodeVI(struct compiler *d, u32 code)
{
   if(d->tb.drop(tok_hash)) {
      struct token *tok = d->tb.get();

      if(tok->type == tok_number) {
         i32 n = strtoi(tok->textV, nil, 0);

         if(n <= 0xFF) {
            Dlg_PushB1(d, code);
            Dlg_PushB1(d, n);
            return true;
         }
      }

      d->tb.unget();
   }

   return false;
}

static bool CodeZI(struct compiler *d, u32 code)
{
   ifw(i32 n = CodeZPG(d, nil), n > 0) {
      Dlg_PushB1(d, code);
      Dlg_PushB1(d, n);
      return true;
   }

   return false;
}

static bool CodeZX(struct compiler *d, u32 code)
{
   ifw(i32 n = CodeZPG(d, "X"), n > 0) {
      Dlg_PushB1(d, code);
      Dlg_PushB1(d, n);
      return true;
   }

   return false;
}

static bool CodeZY(struct compiler *d, u32 code)
{
   ifw(i32 n = CodeZPG(d, "Y"), n > 0) {
      Dlg_PushB1(d, code);
      Dlg_PushB1(d, n);
      return true;
   }

   return false;
}

/* Extern Functions -------------------------------------------------------- */

void Dlg_GetStmt_Asm(struct compiler *d)
{
   struct token *tok;

   #define DCD(n, op, ty) \
      tok = d->tb.reget(); \
      if(faststrcasecmp(tok->textV, #op) == 0) \
         if(Code##ty(d, DCD_##op##_##ty)) return;
   #include "d_vm.h"

   tok = d->tb.reget();
   ErrT(d, tok, "no function found with this syntax");
}

#endif

/* EOF */
