/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Dialogue compiler utilities.
 *
 * ---------------------------------------------------------------------------|
 */

#include "d_compile.h"

/* Extern Functions -------------------------------------------------------- */

void Dlg_PushB1(struct compiler *d, u32 b)
{
   size_t pc = d->def.codeP++;

   if(pc + 1 > PRG_END - PRG_BEG) d->tb.err("PRG segment overflow");

   if(pc + 1 > d->def.codeC * 4) {
      Vec_Grow(d->def.code, 1);
      d->def.codeC++;
   }

   if(b > 0xFF) d->tb.err("byte error (overflow) %u", b);

   Cps_SetC(d->def.codeV, pc, b);
}

void Dlg_PushB2(struct compiler *d, u32 word)
{
   Dlg_PushB1(d, word & 0xFF);
   Dlg_PushB1(d, word >> 8);
}

void Dlg_PushLdVA(struct compiler *d, u32 action)
{
   Dlg_PushB1(d, DCD_LDV_VI);
   Dlg_PushB1(d, action);
}

struct ptr2 Dlg_PushLdAdr(struct compiler *d, u32 at, u32 set)
{
   struct ptr2 adr;

   Dlg_PushB1(d, DCD_LDA_VI);
   Dlg_PushB1(d, set & 0xFF);
   adr.l = d->def.codeP - 1;

   Dlg_PushB1(d, DCD_STA_AI);
   Dlg_PushB2(d, at);

   Dlg_PushB1(d, DCD_LDA_VI);
   Dlg_PushB1(d, set >> 8);
   adr.h = d->def.codeP - 1;

   Dlg_PushB1(d, DCD_STA_AI);
   Dlg_PushB2(d, at + 1);

   return adr;
}

void Dlg_SetB1(struct compiler *d, u32 ptr, u32 b)
{
   if(b > 0xFF) d->tb.err("byte error (overflow) %u", b);

   Cps_SetC(d->def.codeV, ptr, b);
}

void Dlg_SetB2(struct compiler *d, u32 ptr, u32 word)
{
   Dlg_SetB1(d, ptr + 0, word & 0xFF);
   Dlg_SetB1(d, ptr + 1, word >> 8);
}

u32 Dlg_PushStr(struct compiler *d, cstr s, u32 l)
{
   u32  p = d->def.stabP;
   u32 vl = Cps_Adjust(p + l) - d->def.stabC;

   if(p + l > STR_END - STR_BEG) d->tb.err("STR segment overflow");

   Dbg_Log(log_dlg, "%s: (%3u %3u) '%s'", __func__, l, vl, s);

   Vec_Grow(d->def.stab, vl);
   d->def.stabC += vl;
   d->def.stabP += l;

   for(u32 i = 0; i < l; i++) Cps_SetC(d->def.stabV, p + i, s[i]);

   return STR_BEG + p;
}

/* EOF */
