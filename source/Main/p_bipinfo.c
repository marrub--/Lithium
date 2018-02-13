// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_bip.h"
#include "lith_player.h"

struct page_initializer const bip_pages[] = {
   {.category = BIPC_WEAPONS},

   {gO, "ChargeFist", {"KSKK"}},

   {pM, "Pistol",          {"Omakeda"}, .isfree = true},
   {pM, "Revolver",        {"Earth"}},
   {pM, "Shotgun",         {"Omakeda"}},
   {pM, "ShotgunUpgr",     {"Shotgun", "AOF", "DurlaPrime"}},
   {pM, "ShotgunUpg2",     {"Shotgun"}},
   {pM, "LazShotgun",      {"Earth"}},
   {pM, "SuperShotgun",    {"ChAri"}},
   {pM, "CombatRifle",     {"AllPoint"}},
   {pM, "RifleUpgr",       {"CombatRifle"}},
   {pM, "RifleUpg2",       {"CombatRifle", "Semaphore"}},
   {pM, "SniperRifle",     {"Facer"}},
   {pM, "GrenadeLauncher", {"Sym43"}},
   {pM, "LauncherUpgr",    {"GrenadeLauncher", "UnrealArms"}},
   {pM, "LauncherUpg2",    {"GrenadeLauncher", "Sym43"}},
   {pM, "PlasmaRifle",     {"AllPoint", "MDDO"}},
   {pM, "PlasmaUpgr",      {"PlasmaRifle"}},
   {pM, "PlasmaUpg2",      {"PlasmaRifle", "Semaphore"}},
   {pM, "BFG9000",         {"Cid"}},
   {pM, "CannonUpgr",      {"BFG9000", "SuperDimension"}},
   {pM, "CannonUpg2",      {"BFG9000"}},

   {pC, "Mateba",        {"AOF"}, .isfree = true},
   {pC, "MatebaUpgr",    {"Mateba", "AOF", "Algidistari"}},
   {pC, "ShockRifle",    {"ChAri"}},
   {pC, "ShockRifUpgr",  {"ShockRifle"}},
   {pC, "ShockRifUpg2",  {"ShockRifle"}},
   {pC, "SPAS",          {"AOF", "Newvec"}},
   {pC, "SPASUpgr",      {"SPAS"}},
   {pC, "SPASUpg2",      {"SPAS", "Newvec"}},
   {pC, "SMG",           {"Omakeda", "Sym43"}},
   {pC, "SMGUpgr",       {"SMG", "AOF", "Sym43"}},
   {pC, "SMGUpg2",       {"SMG", "AOF"}},
   {pC, "SMGUpg3",       {"SMG", "Sym43"}},
   {pC, "IonRifle",      {"KSKK"}},
   {pC, "IonRifleUpgr",  {"IonRifle", "KSKK"}},
   {pC, "IonRifleUpg2",  {"IonRifle", "KSKK"}},
   {pC, "CPlasmaRifle",  {"AllPoint", "MDDO"}},
   {pC, "CPlasmaUpgr",   {"CPlasmaRifle", "MDDO"}},
   {pC, "StarDestroyer", {"Hell"}},
   {pC, "ShipGunUpgr",   {"StarDestroyer"}},
   {pC, "ShipGunUpg2",   {"StarDestroyer"}},

   {gO, "MissileLauncher"},
   {gO, "PlasmaDiffuser", {"Sym43", "MDDO", "Semaphore"}},

   {pC, "Blade",               .isfree = true},
   {pC, "Delear",   {"Earth"}, .isfree = true},
   {pC, "Feuer"},
   {pC, "Rend"},
   {pC, "Hulgyon",  {"Heaven"}},
   {pC, "StarShot", {"AOF"}},
   {pC, "Cercle",   {"Earth"}},

   {.category = BIPC_ENEMIES, .isfree = true},

   {gA, "ZombieMan"},
   {gA, "ShotgunGuy"},
   {gA, "ChaingunGuy"},

   {gA, "Imp"},
   {gA, "Demon"},
   {gA, "Spectre"},

   {gA, "LostSoul"},
   {gA, "Mancubus"},
   {gA, "Arachnotron"},
   {gA, "Cacodemon"},

   {gA, "HellKnight"},
   {gA, "BaronOfHell"},
   {gA, "Revenant"},

   {gA, "PainElemental"},
   {gA, "Archvile"},
   {gA, "SpiderMastermind"},
   {gA, "Cyberdemon"},

   {gA, "Phantom"},
   {gA, "IconOfSin"},

   {.category = BIPC_YOURSELF, .isfree = true},

   {pM, "P114"},
   {pC, "OPD2"},
   {pC, "Info400"},
   {pC, "Info402"},

   {gA, "BIP"},
   {pM, "CBI"},
   {pC, "CBIJem"},

   {gA,    "AttrACC"},
   {gA,    "AttrDEF"},
   {gH,    "AttrSTRHuman"},
   {gR,    "AttrSTRRobot"},
   {gN,    "AttrSTRNonHuman"},
   {gH,    "AttrVIT"},
   {gR|gN, "AttrPOT"},
   {gH,    "AttrSTM"},
   {gR,    "AttrREP"},
   {gN,    "AttrREG"},
   {gA,    "AttrLUK"},
   {gA,    "AttrRGE"},

   {.category = BIPC_UPGRADES},

   {pM, "HeadsUpDisp", {"OFMD"}},
   {pM, "JetBooster",  {"OFMD"}},
   {pM, "ReflexWetw",  {"OFMD"}},
   {pM, "CyberLegs",   {"OFMD"}},
   {pM, "Yh0",         {"DurlaPrime"}},
   {pM, "DefenseNuke", {"OFMD"}},
   {pM, "Adrenaline",  {"KSKK"}},

   {pC, "HeadsUpDispJem", {"AOF"}},
   {pC, "ReflexWetwJem",  {"AOF"}},
   {pC, "Magic"},
   {pC, "SoulCleaver"},
   {pC, "StealthSys"},

   {gO, "VitalScanner", {"KSKK"}},
   {gO, "AutoReload",   {"KSKK"}},

   {pM, "WeapnInter", {"OFMD", "AllPoint"}},
   {pM, "WeapnInte2"},
   {pM, "ArmorInter"},
   {pM, "CBIUpgr1",   {"KSKK"}},
   {pM, "CBIUpgr2",   {"KSKK"}},

   {.category = BIPC_PLACES},

   {gO, "AetosVi"},
   {pC, "Algidistari"},
   {gO, "ChAri",          {"AetosVi"}},
   {pM, "DurlaPrime",     {"Earth", "AetosVi"}},
   {gO, "Earth"},
   {pC, "Hell",           {"Earth"}},
   {gO, "Mars",           {"Earth", "OFMD"}},
   {gO, "OmicronXevv"},
   {pM, "SuperDimension", {"BFG9000", "SIGFPE"}},
   {pC, "Heaven"},

   {.category = BIPC_CORPORATIONS},

   {gO, "AllPoint"},
   {pM, "AOF"},
   {pC, "AOFJem"},
   {pM, "Cid",        {"SuperDimension", "Earth"}},
   {pM, "Facer"},
   {pM, "KSKK",       {"Earth"}},
   {pC, "KSKKJem",    {"Earth"}},
   {gO, "MDDO",       {"Mars", "OFMD"}},
   {pC, "Newvec",     {"Earth"}},
   {pM, "OFMD"},
   {pC, "OFMDJem"},
   {gO, "Omakeda",    {"Earth"}},
   {gO, "Semaphore",  {"OmicronXevv"}},
   {gO, "Sym43",      {"AetosVi"}},
   {pM, "UnrealArms", {"AetosVi"}},

   {.category = BIPC_EXTRA, .isfree = true},

   {gA, "Extra1"},
   {gA, "Extra2"},
   {gA, "Extra3"},
   {gA, "Extra4"},
   {gA, "Extra5"},

   {},
};

// EOF
