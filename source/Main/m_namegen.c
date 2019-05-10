/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
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

static cstr syll[] = {
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

static cstr lulz[] = {
   "ohgodwhat",
   "kdizd",
   "help",
   "ohno",
   "explod",
   "why",
   "fuck"
};

static str name[] = {
   s"Gorgoth",
   s"Merdiklo",
   s"Starface",
   s"Mar'khi",
   s"Veritt",
   s"Modoch",
   s"Vup",
   s"Helliox",
   s"Warrdeth",
   s"Killzor",
   s"Ikhon",
   s"Manslayer",
   s"Fismit",
   s"Killinger",
   s"Eradicus",
   s"Mortarr",
   s"Pox",
   s"Murdercles",
   s"Jerkules",
   s"Haddrox",
   s"Persecon",
   s"Errexioth",
   s"Satan Jr.",
   s"Makob",
   s"Vorbenix",
   s"Norvenica",
   s"Mek'tethel",
   s"Vex'genn",
   s"Horoxoloth",
   s"Chaotica",
   s"Hex'vorr",
   s"Legetto",
   s"Fauror",
   s"Kormat",
   s"Mogaltu",
   s"Shel",
   s"Vameth",
   s"Dethul",
   s"Ginavak",
   s"Kantul",
   s"Korthaet",
   s"Laniaro",
   s"Lortuc",
   s"Morcatu",
   s"Ndur",
   s"Saugur",
   s"Vathan",
   s"Caethind",
   s"Hirthe",
   s"Inchindr",
   s"Kaelarmaul",
   s"Koritu",
   s"Nakor",
   s"Ngugobal",
   s"Obair",
   s"Rorgia",
   s"Falau",
   s"Hethaeg",
   s"Melulind",
   s"Morchaur",
   s"Nilrin",
   s"Shabakauth",
   s"Ulkal",
   s"Valdo",
   s"Wendun",
   s"Atund",
   s"Bagurar",
   s"Shuldu",
   s"Be'elzebubba",
   s"K'banian",
   s"Faellat",
   s"G'ldethi",
   s"Kazgoroth",
   s"Mathanat",
   s"Ngot",
   s"S'goth",
   s"Sot-ilh",
   s"Ubote",
   s"Yar'udab",
   s"Ulyaoth",
   s"Xel'lotath",
   s"Chattur'gha",
   s"Mantorok",
   s"Gowron",
   s"Dak'shira",
   s"Keghas",
   s"Pha'otho",
   s"N'hogura",
   s"Alevot",
   s"Arafonad",
   s"Axurador",
   s"Erelak",
   s"Eroam",
   s"Kazidax",
   s"Nudob",
   s"Tarvo",
   s"Araxidak",
   s"Duri",
   s"Kafondam",
   s"Rilor",
   s"Ukam",
   s"Kofarutul",
   s"Edorir",
   s"Sofolos",
   s"Tarivror",
   s"Xibavel",
   s"M'Rub",
   s"P'Noon Ikl",
   s"Olok",
};

static str suff[] = {
   s"Supreme",
   s"Lord of Brimstone",
   s"Eater of Souls",
   s"Betrayer of All",
   s"Executioner",
   s"Seeker of Bloodshed",
   s"Agent of Damnation",
   s"Image of Darkness",
   s"Bringer of Famine",
   s"Engine of Ruin",
   s"Ender of Days",
   s"Lord of Chaos",
   s"Lord of Darkness",
   s"Lord of Betrayal",
   s"Slayer of Worlds",
   s"Shadow of the Night",
   s"Dark One",
   s"Chosen of Sin",
   s"Cold-eyed Slayer",
   s"Juggernaut",
   s"Walking Apocalypse",
   s"Invincible",
   s"Final Judgement",
   s"One Who Seeks Battle",
   s"Overlord of Pandemonium",
   s"Lord of Hatred",
   s"Incarnation of Death",
   s"Hideous Destructor",
   s"Horrendeous Tyrant",
   s"Destroyer",
   s"Sinful",
   s"Dogma of Evil",
   s"Alpha and Omega",
};

/* Extern Functions -------------------------------------------------------- */

str RandomName(i32 id)
{
   srand(id ? mapseed + id : ACS_Random(0, INT_MAX));

   ACS_BeginPrint();
   PrintChars("\Cg", 2);

   if(id && (rand() % 10) == 0)
      ACS_PrintString(name[rand() % countof(name)]);
   else for(i32 i = 0, n = 3 + (rand() % 6); i < n; i++)
   {
      cstr s = rand() % 101 == 0 ?
               lulz[rand() % countof(lulz)] :
               syll[rand() % countof(syll)];

      if(i == 0) {ACS_PrintChar(ToUpper(*s)); s++;}

      PrintChrSt(s);
   }

   if(id)
   {
      PrintChars(", the ", 6);
      ACS_PrintString(suff[rand() % countof(suff)]);
   }

   return ACS_EndStrParam();
}

/* EOF */
