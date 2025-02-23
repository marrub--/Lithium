// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │    Predefined names and titles mostly taken from Kyle873's Doom RPG.     │
// │                          Used with permission.                           │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Procedural monster name generation.                                      │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "w_world.h"

enum {
   _vty_bck,
   _vty_fro,
   _vty_max,
};

struct word_root {
   i32  vty;
   cstr root;
};

/* consonants:
 * q [kʷ] k [k]
 * v [v] s [ɕ] t [t]
 * r [ɻ] l [l]
 *
 * vowels:
 * u [u] o [o] ó [ɒ] (back)
 * i [i] e [e] a [a] (front)
 */

static
cstr const vowels[_vty_max][3] = {
   {"u", "o", "ó"},
   {"i", "e", "a"},
};

static
struct word_root const roots[] = {
   {_vty_bck, "Kusu"},  /* i decay, i rot */
   {_vty_bck, "Luqó"},  /* i debauch, i corrupt */
   {_vty_bck, "Lós"},   /* i give, i destroy, i conquer */
   {_vty_bck, "Qós"},   /* i remove, i delete, i purge */
   {_vty_bck, "Rolv"},  /* i surrender, i stop, i discontinue */
   {_vty_bck, "Sutk"},  /* i sink, i decline */
   {_vty_bck, "Vók"},   /* i create */
   {_vty_fro, "Aei"},   /* i anger, i defile */
   {_vty_fro, "Iqti"},  /* i menace, i dismay */
   {_vty_fro, "Kve"},   /* i transcend, i surpass */
   {_vty_fro, "Mek"},   /* i desynthesize, i uncreate */
   {_vty_fro, "Salek"}, /* i betray, i deceive */
   {_vty_fro, "Silv"},  /* i skip, i proceed, i go ahead */
   {_vty_fro, "Viqka"}, /* i envigor, i enhance, i rule */
};

static
cstr const adjectives[] = {
   "k0q1", /* understated, scheming */
   "k0r",  /* stagnant */
   "k0r1", /* something that rushes to the top */
   "k1t",  /* animated */
   "k1t1", /* absurd, over-powered */
   "l0l0", /* slow */
   "l0l2", /* swift */
   "l0t2", /* cowardly */
   "l1v1", /* the last alive of a group */
   "l2q0", /* abnormal, weak */
   "q0l",  /* writer */
   "q2r0", /* brave */
   "r1q1", /* raging, tyrannical, amazing */
   "s1m0", /* warrior */
   "s1v0", /* odd, strange */
   "s2l",  /* normal, strong */
   "t1v",  /* murderer */
   "t2r",  /* something that slowly reaches a pinnacle point */
};

static
cstr const inflections[] = {
   "",  /* + past */
   "e", /* - past */
   "ó", /* + present */
   "i", /* - present (der. var) */
   "u", /* + future (der. Vók) */
   "a", /* - future (der. vak) */
   "o", /* gone from all time */
};

static
cstr const amusing_name[] = {
   "Be'elzebubba",
   "Jerkules",
   "Killinger",
   "Killzor",
   "Kökksukk",
   "M'Rub",
   "Makob",
   "Manslayer",
   "Murdercles",
   "P'Noon Ikl",
   "Satan Jr.",
   "Starface",
   "Warrdeth",
};

static
cstr const titles[] = {
   "Agent of Damnation",
   "Alpha and Omega",
   "Betrayer of All",
   "Bringer of Famine",
   "Chosen of Sin",
   "Cold-eyed Slayer",
   "Dark One",
   "Destroyer",
   "Dogma of Evil",
   "Eater of Souls",
   "Ender of Days",
   "Engine of Ruin",
   "Executioner",
   "Final Judgement",
   "Hideous Destructor",
   "Horrendeous Tyrant",
   "Image of Darkness",
   "Incarnation of Death",
   "Invincible",
   "Juggernaut",
   "Lord of Betrayal",
   "Lord of Brimstone",
   "Lord of Chaos",
   "Lord of Darkness",
   "Lord of Hatred",
   "One Who Seeks Battle",
   "Overlord of Pandemonium",
   "Seeker of Bloodshed",
   "Shadow of the Night",
   "Sinful",
   "Slayer of Worlds",
   "Supreme",
   "Walking Apocalypse",
};

void copy_name_str(char **p, cstr src) {
   mem_size_t len = faststrlen(src);
   fastmemcpy(*p, src, len);
   *p += len;
}

void RandomName(i32 id) {
   srand(id ? ml.seed + id : ACS_Random(0, INT32_MAX));

   PrintStrL("\Cg");

   if(id && rand() % 1000 != 0) {
      noinit static
      char s[128];
      char *p = s;
      cstr last = nil;
      cstr suf;
      struct word_root const *root;
      for(i32 i = 0, n = 3 + (rand() & 3); i < n; i++) {
         if(i == 0) {
            /* root word */
            root = &roots[rand() % countof(roots)];
            copy_name_str(&p, root->root);
         } else if(i == n - 1) {
            /* (optional) inflection/voice */
            suf = inflections[rand() % countof(inflections)];
            /* duplicated terminals have a glottal stop between */
            if(*(p - 1) == *suf || *(p - 1) == *(suf + 1)) {
               *p++ = '\'';
            }
            copy_name_str(&p, suf);
         } else {
            /* never duplicate adjectives, repetition is okay */
            cstr adj;
            do {
               adj = adjectives[rand() % countof(adjectives)];
            } while(last == adj);
            /* create vowel harmony */
            for(cstr adjp = adj; *adjp; ++adjp) {
               if(IsDigit(*adjp)) {
                  copy_name_str(&p, vowels[root->vty][*adjp - '0']);
               } else {
                  *p++ = *adjp;
               }
            }
            last = adj;
         }
      }
      *p = '\0';
      PrintStr(s);
   } else {
      PrintStr(amusing_name[rand() % countof(amusing_name)]);
   }

   if(id) {
      PrintStrL(", the ");
      PrintStr(titles[rand() % countof(titles)]);
   }
}

/* EOF */
