/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * World entry points.
 *
 * ---------------------------------------------------------------------------|
 */

#if defined(finale_code)

finale_code(next, 'n', 'e', 'x', 't', 0, "")
finale_code(fade, 'f', 'a', 'd', 'e', 1, "u")
finale_code(fmus, 'f', 'm', 'u', 's', 1, "u")
finale_code(imge, 'i', 'm', 'g', 'e', 1, "s")
finale_code(musi, 'm', 'u', 's', 'i', 1, "s")
finale_code(text, 't', 'e', 'x', 't', 4, "suiu")
finale_code(wait, 'w', 'a', 'i', 't', 1, "u")
#undef finale_code

#else

#include "common.h"
#include "p_player.h"
#include "w_world.h"
#include "w_monster.h"
#include "m_version.h"
#include "m_file.h"
#include "m_tokbuf.h"

#define NextCode(c) fcode[(c)->fnum][(c)->fptr++]

enum {
   #define finale_code(name, c1, c2, c3, c4, narg, arg) _fcd_##name,
   #include "w_finale.c"

   _fill_x = 280,
   _fill_y = 220,
};

union finale_code {
   i32 i;
   u32 u;
   str s;
};

struct finale_state {
   union finale_code *prg;
   str bgnd;
};

struct finale_compiler {
   struct tokbuf tb;
   u32 fptr, fnum;
   u32 fnarg;
   cstr farg;
   bool ok;
};

static cstr finale;
static union finale_code fcode[16][64];
static bool finit;

script static
void F_parseArgs(struct finale_compiler *c) {
   for(i32 i = 0; i < c->fnarg; i++) {
      switch(c->farg[i]) {
      case 'i':
         NextCode(c).i =
            strtoi(c->tb.expc(c->tb.get(), tok_number)->textV, nil, 0);
         break;
      case 'u':
         NextCode(c).u =
            strtoui(c->tb.expc(c->tb.get(), tok_number)->textV, nil, 0);
         break;
      case 's':
         NextCode(c).s = l_strdup(c->tb.expc(c->tb.get(), tok_string)->textV);
         break;
      }
   }
}

script static
void F_parseFunc(struct finale_compiler *c, struct token *tok) {
   if(tok->textC != 5)
      c->tb.err("invalid function name '%s'", tok->textV);

   switch(FourCCPtr(tok->textV)) {
   #define finale_code(name, c1, c2, c3, c4, narg, arg) \
   case FourCC(c1, c2, c3, c4): \
      NextCode(c).u = _fcd_##name; \
      c->farg = arg; \
      c->fnarg = narg; \
      break;
   #include "w_finale.c"
   default:
      c->tb.err("no function named '%s'", tok->textV);
   }

   F_parseArgs(c);

   c->tb.expc(c->tb.get(), tok_semico);
}

script static
void F_parseLabel(struct finale_compiler *c, struct token *tok) {
   c->fnum = strtoi(tok->textV, nil, 0);
   c->fptr = 0;
}

static
bool F_loadFile(cstr which) {
   static struct finale_compiler c = {};

   c.ok = true;

   Dbg_Log(log_dev, "%s: loading %s", __func__, which);

   c.tb.fp = W_Open(StrParam("lfiles/End_%s.txt", which), "r");

   TBufCtor(&c.tb);

   if(setjmp(c.tb.env)) {
      c.ok = false;
      goto done;
   }

   for(;;) {
      struct token *tok =
         c.tb.expc3(c.tb.get(), tok_eof, tok_identi, tok_number);

      if(tok->type == tok_eof) {
         break;
      } else if(tok->type == tok_identi) {
         F_parseFunc(&c, tok);
      } else if(tok->type == tok_number) {
         F_parseLabel(&c, tok);
      }
   }

done:
   TBufDtor(&c.tb);
   fclose(c.tb.fp);

   return c.ok;
}

void F_Load(void) {
   if(F_loadFile("Common") && F_loadFile(finale))
      finit = true;
}

static
void F_drawBack(struct finale_state *st) {
   SetSize(320, 200);
   if(st->bgnd == s_NIL) {
      PrintRect(0, 0, 320, 200, 0x000000);
   } else {
      PrintSprite(st->bgnd, 0,1, 0,1);
   }
   SetSize(320, 240);
}

static
void F_drawFade(k32 amount) {
   PrintRectA(0, 0, 320, 240, 0x000000, 255 * amount);
}

alloc_aut(0) sync static
void F_fade(struct finale_state *st) {
   u32 fade = st->prg++->u;

   for(i32 i = fade; i >= 0; i--) {
      F_drawBack(st);
      F_drawFade(i / (k32)fade);
      ACS_Delay(1);
      DrawCallI(sm_LE);
   }
}

static
void F_next(struct finale_state *st) {
   if(st->prg > fcode[14]) {
      st->prg = nil;
   } else {
      st->prg = fcode[14];
   }
}

alloc_aut(0) sync static
void F_fmus(struct finale_state *st) {
   u32 fade = st->prg++->u;

   for(i32 i = fade; i >= 0; i--) {
      F_drawBack(st);
      ACS_SetMusicVolume(i / (k32)fade);
      ACS_Delay(1);
      DrawCallI(sm_LE);
   }
}

static
void F_imge(struct finale_state *st) {
   str name = st->prg++->s;
   st->bgnd = name;
}

static
void F_musi(struct finale_state *st) {
   str name = st->prg++->s;
   ACS_SetMusic(name);
}

alloc_aut(0) sync static
void F_text(struct finale_state *st) {
   str name = st->prg++->s;
   u32 time = st->prg++->u;
   i32 hold = st->prg++->i;
   u32 fade = st->prg++->u;

   cstr txt = LanguageC(LANG "ENDING_%S", name);
   u32  len = strlen(txt);

   u32 fill     = hold == -1 ? 0 : hold;
   u32 skipfill = 0;

   for(i32 i = time + fade; i >= 0; i--) {
      F_drawBack(st);

      i32 buttons = ACS_GetPlayerInput(-1, INPUT_BUTTONS);
      u32 p, h;

      if(i > fade) {
         k32 mul = 1.0 - (i - fade) / (k32)time;
         p = len * mul;
         h = 220 * mul;
      } else {
         p = len;
         h = 220;
      }

      SetClipW(10, 10, 300, h, 300);
      PrintRectA(0, 0, 320, 240, 0x000000, 127);
      PrintTextChr(txt, p);
      PrintText(s_smallfnt, CR_WHITE, 10,1, 10,1);
      ClearClip();

      if(i == fade) {
         if(hold == -1) {
            if(!G_Filler(_fill_x, _fill_y, &fill, 87,
                         buttons & (BT_USE | BT_ATTACK))) {
               i++;
            }
         } else if(--fill) {
            i++;
         }
      } else if(i < fade) {
         F_drawFade(1.0 - i / (k32)fade);
      } else if(time > 100 &&
                G_Filler(_fill_x, _fill_y, &skipfill, 175,
                         buttons & (BT_USE | BT_ATTACK))) {
         break;
      }

      ACS_Delay(1);
      DrawCallI(sm_LE);
   }
}

alloc_aut(0) sync static
void F_wait(struct finale_state *st) {
   u32 hold = st->prg++->u;

   for(i32 i = hold; i >= 0; i--) {
      F_drawBack(st);
      ACS_Delay(1);
      DrawCallI(sm_LE);
   }
}

void F_Start(cstr which) {
   Str(lithend, s"LITHEND");

   finale = which;

   ACS_ChangeLevel(lithend, 0, CHANGELEVEL_NOINTERMISSION |
                   CHANGELEVEL_PRERAISEWEAPON, -1);
}

_Noreturn dynam_aut script
void F_Run(struct player *p) {
   Str(actually_end_the_game, s"ActuallyEndTheGame");

   while(!finit) ACS_Delay(1);

   struct finale_state st = {};
   u32 prg;

   switch(p->pclass) {
   case pcl_marine:    prg = 0; break;
   case pcl_cybermage: prg = 1; break;
   case pcl_informant: prg = 2; break;
   case pcl_wanderer:  prg = 3; break;
   case pcl_assassin:  prg = 4; break;
   case pcl_darklord:  prg = 5; break;
   case pcl_thoth:     prg = 6; break;
   }

   st.prg = fcode[prg];

   Dbg_Log(log_dev, "%s: running %s", __func__, finale);

   while(st.prg) {
      switch(st.prg++->u) {
      #define finale_code(name, c1, c2, c3, c4, narg, arg) \
      case _fcd_##name: \
         Dbg_Log(log_dev, "%s: " #name, __func__); \
         F_##name(&st); \
         break;
      #include "w_finale.c"
      }
   }

   ServCallI(actually_end_the_game);
}

script_str ext("ACS") addr(OBJ "Finale")
void Sc_Finale() {
   i32 boss = ServCallI(sm_GetBossLevel);
   cstr which;

   if(GetFun() & lfun_division) {
      which = "Division";
   } else {
      switch(boss) {
      case boss_none:        which = "Normal";      break;
      case boss_other:       which = "Other";       break;
      case boss_barons:      which = "Barons";      break;
      case boss_cyberdemon:  which = "CyberDemon";  break;
      case boss_spiderdemon: which = "SpiderDemon"; break;
      case boss_iconofsin:   which = "IconOfSin";   break;
      }
   }

   F_Start(which);
}

#endif

/* EOF */
