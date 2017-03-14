// Predefined names and suffixes mostly taken from Kyle873's Doom RPG.
// Used with permission.
#include "lith_common.h"
#include "lith_world.h"
#include <ctype.h>


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_RandomName
//
__str Lith_RandomName(int id)
{
   static __str syllables[] = {
      /*か*/ "ka","k'","khi","kaz",
      /*が*/ "ga","go","gor","got","gn",
      /*さ*/ "sha","sk","shu","shi",
      /*ざ*/ "zk",
      /*た*/ "th","tch","ch","tt","t'","tar","cth",
      /*な*/ "nil","n'",
      /*だ*/ "do","dak'","dax",
      /*は*/ "fta","h",
      /*ば*/ "bur","bel'",
      /*ま*/ "ma","mo","mar",
      /*ら*/ "rly","l","r'",
      /*う*/ "u","ul",
      /*え*/ "eh","er",
      /*無*/ "ver","xel'",
   };
   
   static __str lul[] = {"ohgodwhat","kdizd","help","ohno","explod","why","fuck"};
   
   static __str names[] = {
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
   
   static __str suffixes[] = {
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
   };
   
   __str ret = "";
   
   if(id)
      srand(world.mapseed + id);
   else
      srand(ACS_Random(0, 0x7FFFFFFF));
   
   if(id && (rand() % 10) == 0)
      ret = names[rand() % countof(names)];
   else
      for(int i = 0, n = 3 + (rand() % 6); i < n; i++)
         if((rand() % 101) == 0)
            ret = StrParam("%S%S", ret, lul[rand() % countof(lul)]);
         else
            ret = StrParam("%S%S", ret, syllables[rand() % countof(syllables)]);
   
   if(id)
      ret = StrParam("%S, the %S", ret, suffixes[rand() % countof(suffixes)]);
   
   return StrParam("\Cg%c%S", toupper(ACS_StrLeft(ret, 1)[0]), ACS_StrRight(ret, ACS_StrLen(ret) - 1));
}

// EOF

