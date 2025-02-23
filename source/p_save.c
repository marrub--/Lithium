// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Save file loading/saving.                                                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "m_trie.h"
#include <setjmp.h>

#define MAX_STRARRAY 1000
#define PLACEHOLDER '!'

struct input_array {
   astr strs[16];
   i32 ptr, strptr;
};

struct output_array {
   i32 ptr, strptr;
};

#define geti(inp) ((inp)->strs[(inp)->strptr][(inp)->ptr])
#define peepi(inp) \
   ((inp)->ptr + 1 >= MAX_STRARRAY ? \
    (inp)->strs[(inp)->strptr + 1][0] : \
    (inp)->strs[(inp)->strptr][(inp)->ptr + 1])
#define nexti(inp) \
   ((inp)->ptr + 1 >= MAX_STRARRAY ? \
    (inp)->strs[++(inp)->strptr][(inp)->ptr = 0] : \
    (inp)->strs[(inp)->strptr][++(inp)->ptr])

#define check_next(inp, chr) \
   statement( \
      if(geti(inp) != chr) { \
         Dbg_Log(log_save, _l("error at line "), _p(__LINE__)); \
         longjmp(data_env, _datajmp_fatal); \
      } \
      else (void)nexti(inp); \
   )

#define chunk_ver(ver) \
   statement( \
      if(!check_version(chunk_name, version, ver)) \
         longjmp(data_env, _datajmp_fatal); \
   )

enum {
   _bipu_new,
};

enum {
   _datajmp_fatal = 1,
};

noinit static jmp_buf data_env;

stkoff static
void writeco(struct output_array *out, i32 ch) {
   if(out->ptr + 1 >= MAX_STRARRAY) {
      out->ptr = 0;
      CVarSetS(sa_psave[out->strptr++], ACS_EndStrParam());
      ACS_BeginPrint();
   }
   ACS_PrintChar(ch);
   ++out->ptr;
}

stkoff static
void writeso(struct output_array *out, cstr s) {
   for(; *s; ++s) writeco(out, *s);
}

stkoff static
void writesno(struct output_array *out, cstr s, i32 len) {
   for(register i32 i = 0; i < len; ++i) writeco(out, s[i]);
}

stkoff static
void writeio(struct output_array *out, i32 n) {
   noinit static
   char buf[64], *p;
   p = buf + sizeof(buf) - 1;
   while(n) {
      register i32div div = __div(n, 92);
      *--p = div.rem + '"';
      n = div.quot;
   }
   writeso(out, p);
}

stkoff static
void chunk_out(struct output_array *out, cstr name, i32 version) {
   Dbg_Log(log_save, _l("Write chunk "), _p(name));
   writeso(out, name);
   writeio(out, version);
   writeco(out, PLACEHOLDER);
}

stkoff static
i32 readii(struct input_array *inp) {
   register i32 ret = 0, digit;
   for(; (digit = geti(inp) - '"') < 92 && digit > 0; nexti(inp)) {
      register i32 oldret = ret;
      ret = ret * 92 + digit;
      if(ret < oldret) {
         return oldret;
      }
   }
   return ret;
}

stkoff static
void readsi(struct input_array *inp, char *out, mem_size_t max_len, char delim) {
   register i32 i, c;
   for(i = 0; i < max_len && (c = byte(geti(inp))) != delim; ++i, nexti(inp)) {
      out[i] = c;
   }
   out[i] = '\0';
}

static
bool check_version(cstr name, i32 version, i32 expected) {
   if(version != expected) {
      Dbg_Log(log_save,
              _l("version "), _p(expected),
              _l(" expected for chunk "), _p(name),
              _l(" but got "), _p(version));
      return false;
   } else {
      return true;
   }
}

script
void P_Data_Save(void) {
   Dbg_Log(log_save, _l("Saving data..."));
   ACS_BeginPrint();
   struct output_array out = {};
   chunk_out(&out, "Lith", 8);
   if(pl.done_intro) {
      chunk_out(&out, "intr", 2);
      writeio(&out, pl.done_intro);
      writeco(&out, PLACEHOLDER);
   }
   chunk_out(&out, "agrp", 2);
   i32 agrps = 0;
   for(i32 i = 0; i < UPGR_MAX; ++i) {
      if(pl.upgrades[i].agroups) ++agrps;
   }
   writeio(&out, agrps);
   writeco(&out, PLACEHOLDER);
   for(i32 i = 0; i < UPGR_MAX; ++i) {
      i32 groups = pl.upgrades[i].agroups;
      if(groups) {
         writeso(&out, Upgr_EnumToStr(i));
         writeco(&out, PLACEHOLDER);
         writeio(&out, groups);
         writeco(&out, PLACEHOLDER);
      }
   }
   chunk_out(&out, "note", 1);
   for(i32 i = 0; i < countof(pl.notes); ++i) {
      mem_size_t len = pl.notes[i] ? faststrlen(pl.notes[i]) : 0;
      writeio(&out, len);
      writeco(&out, PLACEHOLDER);
      writesno(&out, pl.notes[i], len);
   }
   chunk_out(&out, "bipu", 2);
   i32 numpage = 0;
   for_page() {
      if(get_bit(page->flags, _page_unlocked) &&
         page->category <= _bipc_last_normal)
      {
         ++numpage;
      }
   }
   writeio(&out, numpage);
   writeco(&out, PLACEHOLDER);
   for_page() {
      if(get_bit(page->flags, _page_unlocked) &&
         page->category <= _bipc_last_normal)
      {
         i32 flg = 0;
         if(get_bit(page->flags, _page_new)) set_bit(flg, _bipu_new);
         writeso(&out, page->name);
         writeco(&out, PLACEHOLDER);
         writeio(&out, flg);
         writeco(&out, PLACEHOLDER);
      }
   }
   chunk_out(&out, "Lend", 7);
   CVarSetS(sa_psave[out.strptr], ACS_EndStrParam());
}

script void load_agrp(struct input_array *inp) {
   i32 agrps = readii(inp);
   check_next(inp, PLACEHOLDER);
   for(i32 i = 0; i < agrps; ++i) {
      noinit static char name[12];
      readsi(inp, name, sizeof(name) - 1, PLACEHOLDER);
      check_next(inp, PLACEHOLDER);
      i32 agroups = readii(inp);
      check_next(inp, PLACEHOLDER);
      i32 upgr = Upgr_StrToEnum(name);
      if(upgr != -1) pl.upgrades[upgr].agroups = agroups;
   }
}

script void load_note(struct input_array *inp) {
   for(i32 i = 0; i < countof(pl.notes); ++i) {
      mem_size_t len = readii(inp);
      check_next(inp, PLACEHOLDER);
      Dalloc(pl.notes[i]);
      pl.notes[i] = nil;
      if(len) {
         pl.notes[i] = Malloc(len + 1, _tag_plyr);
         readsi(inp, pl.notes[i], len, '\0');
      }
   }
}

script void load_bipu_page(struct input_array *inp) {
   noinit static char name[32];
   readsi(inp, name, sizeof(name) - 1, PLACEHOLDER);
   check_next(inp, PLACEHOLDER);
   i32 flg = readii(inp);
   check_next(inp, PLACEHOLDER);
   struct page *page = P_BIP_NameToPage(name);
   if(page) {
      P_BIP_UnlockPage(page, true);
      if(get_bit(flg, _bipu_new)) set_bit(page->flags, _page_new);
      else                        dis_bit(page->flags, _page_new);
   }
}

script void load_bipu(struct input_array *inp) {
   i32 numpage = readii(inp);
   check_next(inp, PLACEHOLDER);
   for(i32 i = 0; i < numpage; ++i) {
      load_bipu_page(inp);
   }
}

script void P_Data_Load(void) {
   noinit static char chunk_name[5];
   Dbg_Log(log_save, _l("Loading data..."));
   struct input_array inp = {
      .strs = {
         CVarGetS(sa_psave[0]),
         CVarGetS(sa_psave[1]),
         CVarGetS(sa_psave[2]),
         CVarGetS(sa_psave[3]),
         CVarGetS(sa_psave[4]),
         CVarGetS(sa_psave[5]),
         CVarGetS(sa_psave[6]),
         CVarGetS(sa_psave[7]),
         CVarGetS(sa_psave[8]),
         CVarGetS(sa_psave[9]),
         CVarGetS(sa_psave[10]),
         CVarGetS(sa_psave[11]),
         CVarGetS(sa_psave[12]),
         CVarGetS(sa_psave[13]),
         CVarGetS(sa_psave[14]),
         CVarGetS(sa_psave[15]),
      },
      .ptr = 0,
      .strptr = 0,
   };
   for(;;) {
      switch(setjmp(data_env)) {
      case 0:
         break;
      case _datajmp_fatal:
         Dbg_Log(log_save, _l("fatal error -- terminated early"));
         return;
      }
      readsi(&inp, chunk_name, 4, '\0');
      if(chunk_name[0] == '\0') {
         return;
      }
      i32 version = readii(&inp);
      check_next(&inp, PLACEHOLDER);
      Dbg_Log(log_save,
              _l("loading chunk "), _l(chunk_name),
              _l(" version "), _p(version));
      switch(P_SaveChunkName(chunk_name)) {
      case _save_chunk_Lith:
         chunk_ver(8);
         break;
      case _save_chunk_Lend:
         chunk_ver(7);
         return;
      case _save_chunk_intr:
         chunk_ver(2);
         pl.done_intro = readii(&inp);
         check_next(&inp, PLACEHOLDER);
         break;
      case _save_chunk_agrp:
         chunk_ver(2);
         load_agrp(&inp);
         break;
      case _save_chunk_note:
         chunk_ver(1);
         load_note(&inp);
         break;
      case _save_chunk_bipu:
         chunk_ver(2);
         load_bipu(&inp);
         break;
      default:
         Dbg_Log(log_save, _l("unknown chunk "), _l(chunk_name));
         longjmp(data_env, _datajmp_fatal);
      }
   }
}

/* EOF */
