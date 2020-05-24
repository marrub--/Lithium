/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * Predefined names and suffixes mostly taken from Kyle873's Doom RPG.
 * Used with permission.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Procedural monster name generation.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "w_world.h"
#include "m_char.h"

/* Static Objects ---------------------------------------------------------- */

static cstr const syll[] = {
   "ka",  "k'",   "khi", "kaz", "kr",
   "ga",  "go",   "gor", "got", "gn", "gl",
   "sha", "sk",   "shu", "shi", "si",
   "zk",  "zan",
   "th",  "tch",  "ch",  "tt",  "t'", "tar", "cth",
   "nil", "n'",
   "do",  "dak'", "dax",
   "fta", "h",    "ff",
   "bur", "bel'",
   "ma",  "mo",   "mar",
   "rly", "l",    "r'",
   "u",   "ul",
   "eh",  "er",
   "ver", "xel'", "xe",
};

static cstr const lulz[] = {
   "explod",
   "fuck",
   "help",
   "kdizd",
   "ohgodwhat",
   "ohno",
   "why",
};

static cstr const name[] = {
   "Alevot",
   "Arafonad",
   "Araxidak",
   "Atund",
   "Axurador",
   "Bagurar",
   "Be'elzebubba",
   "Caethind",
   "Chaotica",
   "Chattur'gha",
   "Dak'shira",
   "Dethul",
   "Duri",
   "Edorir",
   "Eradicus",
   "Erelak",
   "Eroam",
   "Errexioth",
   "Faellat",
   "Falau",
   "Fauror",
   "Fismit",
   "G'ldethi",
   "Ginavak",
   "Gorgoth",
   "Gowron",
   "Haddrox",
   "Helliox",
   "Hethaeg",
   "Hex'vorr",
   "Hirthe",
   "Horoxoloth",
   "Ikhon",
   "Inchindr",
   "Jerkules",
   "K'banian",
   "Kaelarmaul",
   "Kafondam",
   "Kantul",
   "Kazgoroth",
   "Kazidax",
   "Keghas",
   "Killinger",
   "Killzor",
   "Kofarutul",
   "Koritu",
   "Kormat",
   "Korthaet",
   "Laniaro",
   "Legetto",
   "Lortuc",
   "M'Rub",
   "Makob",
   "Manslayer",
   "Mantorok",
   "Mar'khi",
   "Mathanat",
   "Mek'tethel",
   "Melulind",
   "Merdiklo",
   "Modoch",
   "Mogaltu",
   "Morcatu",
   "Morchaur",
   "Mortarr",
   "Murdercles",
   "N'hogura",
   "Nakor",
   "Ndur",
   "Ngot",
   "Ngugobal",
   "Nilrin",
   "Norvenica",
   "Nudob",
   "Obair",
   "Olok",
   "P'Noon Ikl",
   "Persecon",
   "Pha'otho",
   "Pox",
   "Rilor",
   "Rorgia",
   "S'goth",
   "Satan Jr.",
   "Saugur",
   "Shabakauth",
   "Shel",
   "Shuldu",
   "Sofolos",
   "Sot-ilh",
   "Starface",
   "Tarivror",
   "Tarvo",
   "Ubote",
   "Ukam",
   "Ulkal",
   "Ulyaoth",
   "Valdo",
   "Vameth",
   "Vathan",
   "Veritt",
   "Vex'genn",
   "Vorbenix",
   "Vup",
   "Warrdeth",
   "Wendun",
   "Xel'lotath",
   "Xibavel",
   "Yar'udab",
};

static cstr const suff[] = {
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

/* Extern Functions -------------------------------------------------------- */

str RandomName(i32 id) {
   srand(id ? mapseed + id : ACS_Random(0, INT32_MAX));

   ACS_BeginPrint();
   PrintChars("\Cg", 2);

   if(id && (rand() % 10) == 0) {
      PrintChrSt(name[rand() % countof(name)]);
   } else for(i32 i = 0, n = 3 + (rand() % 6); i < n; i++) {
      cstr s = rand() % 101 == 0 ?
               lulz[rand() % countof(lulz)] :
               syll[rand() % countof(syll)];

      if(i == 0) {ACS_PrintChar(ToUpper(*s)); s++;}

      PrintChrSt(s);
   }

   if(id) {
      PrintChars(", the ", 6);
      PrintChrSt(suff[rand() % countof(suff)]);
   }

   return ACS_EndStrParam();
}

/* EOF */
