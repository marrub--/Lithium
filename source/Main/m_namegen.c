// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
// Predefined names and suffixes mostly taken from Kyle873's Doom RPG.
// Used with permission.
#include "lith_common.h"
#include "lith_world.h"
#include <ctype.h>

StrEntOFF

// Extern Functions ----------------------------------------------------------|

__str Lith_RandomName(int id)
{
   static char const *syll[] = {
      "ka","k'","khi","kaz","kr",
      "ga","go","gor","got","gn","gl",
      "sha","sk","shu","shi","si",
      "zk","zan",
      "th","tch","ch","tt","t'","tar","cth",
      "nil","n'",
      "do","dak'","dax",
      "fta","h","ff",
      "bur","bel'",
      "ma","mo","mar",
      "rly","l","r'",
      "u","ul",
      "eh","er",
      "ver","xel'","xe",
   };

   static char const *lulz[] = {"ohgodwhat","kdizd","help","ohno","explod","why","fuck"};

   static char const *name[] = {
      "Gorgoth",
      "Merdiklo",
      "Starface",
      "Mar'khi",
      "Veritt",
      "Modoch",
      "Vup",
      "Helliox",
      "Warrdeth",
      "Killzor",
      "Ikhon",
      "Manslayer",
      "Fismit",
      "Killinger",
      "Eradicus",
      "Mortarr",
      "Pox",
      "Murdercles",
      "Jerkules",
      "Haddrox",
      "Persecon",
      "Errexioth",
      "Satan Jr.",
      "Makob",
      "Vorbenix",
      "Norvenica",
      "Mek'tethel",
      "Vex'genn",
      "Horoxoloth",
      "Chaotica",
      "Hex'vorr",
      "Legetto",
      "Fauror",
      "Kormat",
      "Mogaltu",
      "Shel",
      "Vameth",
      "Dethul",
      "Ginavak",
      "Kantul",
      "Korthaet",
      "Laniaro",
      "Lortuc",
      "Morcatu",
      "Ndur",
      "Saugur",
      "Vathan",
      "Caethind",
      "Hirthe",
      "Inchindr",
      "Kaelarmaul",
      "Koritu",
      "Nakor",
      "Ngugobal",
      "Obair",
      "Rorgia",
      "Falau",
      "Hethaeg",
      "Melulind",
      "Morchaur",
      "Nilrin",
      "Shabakauth",
      "Ulkal",
      "Valdo",
      "Wendun",
      "Atund",
      "Bagurar",
      "Shuldu",
      "Be'elzebubba",
      "K'banian",
      "Faellat",
      "G'ldethi",
      "Kazgoroth",
      "Mathanat",
      "Ngot",
      "S'goth",
      "Sot-ilh",
      "Ubote",
      "Yar'udab",
      "Ulyaoth",
      "Xel'lotath",
      "Chattur'gha",
      "Mantorok",
      "Gowron",
      "Dak'shira",
      "Keghas",
      "Pha'otho",
      "N'hogura",
      "Alevot",
      "Arafonad",
      "Axurador",
      "Erelak",
      "Eroam",
      "Kazidax",
      "Nudob",
      "Tarvo",
      "Araxidak",
      "Duri",
      "Kafondam",
      "Rilor",
      "Ukam",
      "Kofarutul",
      "Edorir",
      "Sofolos",
      "Tarivror",
      "Xibavel",
      "M'Rub",
      "P'Noon Ikl",
      "Olok",
   };

   static char const *suff[] = {
      "Supreme",
      "Lord of Brimstone",
      "Eater of Souls",
      "Betrayer of All",
      "Executioner",
      "Seeker of Bloodshed",
      "Agent of Damnation",
      "Image of Darkness",
      "Bringer of Famine",
      "Engine of Ruin",
      "Ender of Days",
      "Lord of Chaos",
      "Lord of Darkness",
      "Lord of Betrayal",
      "Slayer of Worlds",
      "Shadow of the Night",
      "Dark One",
      "Chosen of Sin",
      "Cold-eyed Slayer",
      "Juggernaut",
      "Walking Apocalypse",
      "Invincible",
      "Final Judgement",
      "One Who Seeks Battle",
      "Overlord of Pandemonium",
      "Lord of Hatred",
      "Incarnation of Death",
      "Hideous Destructor",
      "Horrendeous Tyrant",
      "Destroyer",
      "Sinful",
      "Dogma of Evil",
      "Alpha and Omega",
   };

   srand(id ? world.mapseed + id : ACS_Random(0, 0x7FFFFFFF));

   ACS_BeginPrint();
   ACS_PrintString(s"\Cg");

   char const *n = name[rand() % countof(name)];

   if(id && (rand() % 10) == 0)
      PrintChars(n, strlen(n));
   else for(int i = 0, n = 3 + (rand() % 6); i < n; i++)
   {
      char const *s = rand() % 101 == 0 ?
                      lulz[rand() % countof(lulz)] :
                      syll[rand() % countof(syll)];

      if(i == 0) ACS_PrintChar(toupper(s++[0]));

      PrintChars(s, strlen(s));
   }

   if(id)
   {
      char const *s = suff[rand() % countof(suff)];
      ACS_PrintString(s", the ");
      PrintChars(s, strlen(s));
   }

   return ACS_EndStrParam();
}

// EOF
