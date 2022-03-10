// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Save file loading/saving.                                                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "m_trie.h"

#define unwrap_eof(label) \
   statement( \
      if(*inp == '\0') goto label; \
   )

#define unwrap_end(chr, label) \
   statement( \
      if(*inp != '\0' && *inp != chr) goto label; \
      else                            ++inp; \
   )

#define unwrap_delim(chr, label) \
   statement( \
      if(*inp != chr) goto label; \
      else            ++inp; \
   )

#define chunk_ver(ver, label) \
   statement(if(!check_version(chunk_name, version, ver)) goto label;)

#define while_not_eoc() while(*inp && byte(*inp) != _eoc_1)

#define for_until_eoc(init, cond, itr) \
   for(init; *inp && byte(*inp) != _eoc_1 && cond; itr)

enum {
   _eoc_1 = 0xC2,
   _eoc_2 = 0x91,
};

enum {
   _bipu_new,
};

alloc_aut(0) stkcall static
void num_out(i32 n) {
   noinit static
   char buf[64], *p;
   p = buf + sizeof(buf) - 1;
   while(n) {
      register div_t div = __div(n, 93);
      *--p = div.rem + '!';
      n = div.quot;
   }
   PrintChrSt(p);
}

alloc_aut(0) stkcall static
void chunk_out(cstr name, i32 version) {
   ACS_PrintChar(_eoc_1);
   ACS_PrintChar(_eoc_2);
   PrintChrSt(name);
   num_out(version);
   ACS_PrintChar(' ');
}

alloc_aut(0) stkcall static
i32 num_inp(astr *p) {
   register i32 ret = 0, digit;
   for(; **p && (digit = **p - '!') < 93 && digit > 0; ++*p) {
      ret = ret * 93 + digit;
   }
   return ret;
}

alloc_aut(0) stkcall static
void str_inp(astr *p, char *out, mem_size_t max_len, char delim) {
   register i32 i;
   --max_len;
   for(i = 0; i < max_len && **p && byte(**p) != delim; ++i, ++*p) {
      out[i] = **p;
      out[i + 1] = '\0';
   }
}

static
bool check_version(cstr name, i32 version, i32 expected) {
   if(version != expected) {
      Dbg_Log(log_dev, "version %i expected for chunk %s but got %i",
              expected, name, version);
      return false;
   } else {
      return true;
   }
}

script
void P_Data_Save(void) {
   Dbg_Log(log_dev, "Saving data...");

   ACS_BeginPrint();
   chunk_out("Lith", 7);

   if(pl.done_intro) {
      chunk_out("intr", 1);
      num_out(pl.done_intro);
   }

   chunk_out("agrp", 1);
   for(i32 i = 0; i < UPGR_MAX; ++i) {
      u32 groups = pl.upgrades[i].agroups;
      if(groups) {
         PrintChrSt(Upgr_EnumToStr(i));
         ACS_PrintChar(' ');
         num_out(groups);
         ACS_PrintChar(' ');
      }
   }

   chunk_out("note", 1);
   for(i32 i = 0; i < countof(pl.notes); ++i) {
      mem_size_t len = pl.notes[i] ? faststrlen(pl.notes[i]) : 0;
      num_out(len);
      ACS_PrintChar(' ');
      PrintChars(pl.notes[i], len);
   }

   chunk_out("bipu", 1);
   for_page() {
      if(get_bit(page->flags, _page_unlocked) &&
         page->category <= _bipc_last_normal)
      {
         i32 flg = 0;
         if(get_bit(page->flags, _page_new)) set_bit(flg, _bipu_new);

         PrintChrSt(page->name);
         ACS_PrintChar(' ');
         num_out(flg);
         ACS_PrintChar(' ');
      }
   }

   CVarSetS(sc_psave, ACS_EndStrParam());
}

script
void P_Data_Load(void) {
   noinit static
   char chunk_name[5];

   Dbg_Log(log_dev, "Loading data...");

   astr inp = CVarGetS(sc_psave);
   while(byte(*inp) == _eoc_1 && byte(inp[1]) == _eoc_2) {
      inp += 2;

      str_inp(&inp, chunk_name, 5, '\0');
      unwrap_eof(error);

      i32 version = num_inp(&inp);
      unwrap_end(' ', error);

      Dbg_Log(log_dev, "loading chunk %s version %i", chunk_name, version);

      switch(P_SaveChunkName(chunk_name)) {
      case _save_chunk_Lith:
         chunk_ver(7, error);
         break;
      case _save_chunk_intr:
         chunk_ver(1, skip);
         pl.done_intro = num_inp(&inp);
         break;
      case _save_chunk_agrp:
         chunk_ver(1, skip);
         while_not_eoc() {
            noinit static
            char name[12];

            str_inp(&inp, name, sizeof name, ' ');
            unwrap_delim(' ', skip_err);

            i32 agroups = num_inp(&inp);
            unwrap_end(' ', skip_err);

            i32 upgr = Upgr_StrToEnum(name);
            if(upgr != -1) pl.upgrades[upgr].agroups = agroups;
         }
         break;
      case _save_chunk_note:
         chunk_ver(1, skip);
         for_until_eoc(i32 i = 0, i < countof(pl.notes), ++i) {
            mem_size_t len = num_inp(&inp);
            unwrap_delim(' ', skip_err);

            Dalloc(pl.notes[i]);
            pl.notes[i] = nil;

            if(len) {
               pl.notes[i] = Malloc(len + 1, _tag_plyr);
               str_inp(&inp, pl.notes[i], len + 1, '\0');
               unwrap_eof(skip_err);
            }
         }
         break;
      case _save_chunk_bipu:
         chunk_ver(1, skip);
         while_not_eoc() {
            noinit static
            char name[32];

            str_inp(&inp, name, sizeof name, ' ');
            unwrap_delim(' ', skip_err);

            i32 flg = num_inp(&inp);
            unwrap_end(' ', skip_err);

            struct page *page = P_BIP_NameToPage(name);
            if(page) {
               P_BIP_Unlock(page, true);
               if(get_bit(flg, _bipu_new)) set_bit(page->flags, _page_new);
               else                        dis_bit(page->flags, _page_new);
            }
         }
         break;
      skip_err:
         Dbg_Log(log_dev, "chunk %s has errors -- skipping", chunk_name);
         goto skip;
      default:
         Dbg_Log(log_dev, "unknown chunk %s -- skipping", chunk_name);
      skip:
         while(*inp && !(*inp == _eoc_1 && inp[1] == _eoc_2)) ++inp;
         break;
      }
   }

   if(!*inp) {
      return;
   }

error:
   Dbg_Log(log_dev, "terminated early");
}

/* EOF */
