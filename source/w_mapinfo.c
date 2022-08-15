// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Map metadata reading.                                                    │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "w_world.h"
#include "m_file.h"
#include "m_tokbuf.h"
#include "m_trie.h"

static void ReadKeys(struct tokbuf *tb, struct err *res, struct map_info *mi) {
   noinit static char k[16], v[64];
   while(tb_kv(tb, res, k, v)) {
      unwrap(res);
      i32 key = MapInfoKeyName(k);
      set_bit(mi->use, key);
      switch(key) {
      case _mi_key_humidity:
      case _mi_key_seed:
      case _mi_key_temperature:
         mi->keys[key] = faststrtoi32(v);
         break;
      case _mi_key_environment:
         mi->keys[key] = MapInfoEnvName(v);
         break;
      case _mi_key_sky:
         mi->keys[key] = MapInfoSkyName(v);
         break;
      case _mi_key_flags:
         mi->keys[key] = tb_rflag(tb, res, v, MapInfoFlagName);
         unwrap(res);
         break;
      default:
         tb_err(tb, res, "invalid key %s", nil, _f, k);
         unwrap_retn();
      }
   }
   tb_expc(tb, res, tb_get(tb), tok_eof, 0);
   unwrap(res);
}

script struct map_info ReadMapInfo(void) {
   struct map_info ret;
   ret.use = 0;
   i32 lump = W_GetMapInfoLump();
   if(lump == -1) {
      return ret;
   }
   FILE *fp = W_OpenNum(lump, 't');
   struct tokbuf tb;
   tb_ctor(&tb, fp, "LITHMAP");
   struct err res = {};
   ReadKeys(&tb, &res, &ret);
   unwrap_print(&res);
   tb_dtor(&tb);
   fclose(fp);
   return ret;
}
