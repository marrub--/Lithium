// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Compressed strings.                                                      │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef m_cps_h
#define m_cps_h

#define Cps_Size(len)     ((len) / 4 + (((len) % 4) != 0))
#define Cps_CountOf(name) (countof(name) * 4)

#define Cps_Expand_str(cps, s, l) fast_strdup(Cps_Expand(cps, s, l))
#define Cps_ExpandNT_str(cps, s)  fast_strdup(Cps_ExpandNT(cps, s))

typedef u32 cps_t;

stkcall void Cps_SetC(cps_t *cps, mem_size_t p, i32 c);
stkcall mem_byte_t Cps_GetC(cps_t const *cps, mem_size_t p);
stkcall cstr Cps_Expand(cps_t const *cps, mem_size_t s, mem_size_t l);
stkcall cstr Cps_ExpandNT(cps_t const *cps, mem_size_t s);
void Dbg_PrintMemC(cps_t const *data, mem_size_t size);

#endif
