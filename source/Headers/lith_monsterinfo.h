enum {
   mif_full = 1 << 0
};

#define M(name) Exp_##name, Score_##name
static struct monster_info const monsterinfo[] = {
#if 0
   // Hexen
   {9999, Score_ShotgunGuy,  mtype_imp,         "Ettin",         mif_full},
   {9999, Score_Imp,         mtype_lostsoul,    "FireDemon",     mif_full},
   {9999, Score_Arachnotron, mtype_arachnotron, "CentaurLeader", mif_full},
   {9999, Score_Demon,       mtype_demon,       "Centaur",       mif_full},
   {9999, Score_Mancubus,    mtype_hellknight,  "IceGuy",        mif_full},
   {9999, Score_Mancubus,    mtype_hellknight,  "SerpentLeader", mif_full},
   {9999, Score_Arachnotron, mtype_hellknight,  "Serpent",       mif_full},
   {9999, Score_HellKnight,  mtype_hellknight,  "Demon1",        mif_full},
   {9999, Score_BaronOfHell, mtype_baron,       "Demon2",        mif_full},
   {9999, Score_Cacodemon,   mtype_mancubus,    "Bishop",        mif_full},
   {9999, Score_HellKnight,  mtype_lostsoul,    "Wraith",        mif_full},
   {9999, Score_CyberDemon,  mtype_cyberdemon,  "Dragon",        mif_full},
   {9999, Score_CyberDemon,  mtype_phantom,     "ClericBoss",    mif_full},
   {9999, Score_CyberDemon,  mtype_phantom,     "FighterBoss",   mif_full},
   {9999, Score_CyberDemon,  mtype_phantom,     "MageBoss",      mif_full},
   {9999, Score_CyberDemon,  mtype_cyberdemon,  "Heresiarch",    mif_full},
   {9999, Score_DSparil * 2, mtype_cyberdemon,  "Korax",         mif_full},
#endif

   // Doom 2
   {M(ZombieMan),     mtype_zombie,        "ZombieMan"       },
   {M(ShotgunGuy),    mtype_zombie,        "ShotgunGuy"      },
   {M(ChaingunGuy),   mtype_zombie,        "ChaingunGuy"     },
   {M(Imp),           mtype_imp,           "Imp"             },
   {M(Demon),         mtype_demon,         "Demon"           },
   {M(Spectre),       mtype_demon,         "Spectre"         },
   {M(LostSoul),      mtype_lostsoul,      "LostSoul"        },
   {M(Mancubus),      mtype_mancubus,      "Fatso"           },
   {M(Mancubus),      mtype_mancubus,      "Mancubus"        },
   {M(Arachnotron),   mtype_arachnotron,   "Arachnotron"     },
   {M(Cacodemon),     mtype_cacodemon,     "Cacodemon"       },
   {M(HellKnight),    mtype_hellknight,    "Knight"          },
   {M(BaronOfHell),   mtype_baron,         "Baron"           },
   {M(Revenant),      mtype_revenant,      "Revenant"        },
   {M(PainElemental), mtype_painelemental, "PainElemental"   },
   {M(Archvile),      mtype_archvile,      "Archvile"        },
   {M(SpiderDemon),   mtype_mastermind,    "SpiderMastermind"},
   {M(CyberDemon),    mtype_cyberdemon,    "Cyberdemon"      },

   // Heretic
   {M(Imp),         mtype_imp,         "Gargoyle"  },
   {M(Demon),       mtype_demon,       "Golem"     },
   {M(Nitrogolem),  mtype_demon,       "Nitrogolem"},
   {M(Spectre),     mtype_demon,       "Sabreclaw" },
   {M(Cacodemon),   mtype_mancubus,    "Disciple"  },
   {M(Ophidian),    mtype_arachnotron, "Ophidian"  },
   {M(HellKnight),  mtype_hellknight,  "Warrior"   },
   {M(BaronOfHell), mtype_cacodemon,   "IronLich"  },
   {M(Maulotaur),   mtype_mastermind,  "Maulotaur" },
   {M(DSparil),     mtype_cyberdemon,  "DSparil"   },

   // Lithium
   {1000, 0, mtype_phantom,    "James"   },
   {2000, 0, mtype_phantom,    "Makarov" },
   {3000, 0, mtype_phantom,    "Isaac"   },
   {0,    0, mtype_cyberdemon, "Steggles"},

   // Cheogsh.wad
   {M(Imp),         mtype_imp,        "Howler",         mif_full},
   {M(Imp),         mtype_imp,        "SoulHarvester",  mif_full},
   {M(Demon),       mtype_demon,      "Satyr",          mif_full},
   {M(BaronOfHell), mtype_baron,      "HellWarrior",    mif_full},
   {M(CyberDemon),  mtype_cyberdemon, "Cheogsh",        mif_full},
   {M(Spectre),     mtype_demon,      "MaulerDemon",    mif_full},
   {M(Spectre),     mtype_demon,      "KDiZDNightmare", mif_full},

   // ed4_rfo1.wad
   {M(CyberDemon),    mtype_cacodemon,     "RealAbstractPain", mif_full},
   {M(Arachnotron),   mtype_arachnotron,   "RailSpider",       mif_full},
   {M(PainElemental), mtype_painelemental, "EDPE",             mif_full},
   {M(Arachnotron),   mtype_arachnotron,   "Arachnophyte",     mif_full},
   {M(Imp),           mtype_imp,           "DragonFamiliar",   mif_full},
   {M(Imp),           mtype_imp,           "BatFamiliar",      mif_full},
   {M(Imp),           mtype_imp,           "SnakeImp",         mif_full},
   {M(DRLACaptain),   mtype_zombie,        "BazookaZombie",    mif_full},
   {M(Imp),           mtype_imp,           "DarkImp",          mif_full},
   {M(Imp),           mtype_imp,           "ImpWarlord",       mif_full},

   // dop.pk3
   {M(LostSoul), mtype_lostsoul, "LostGhoul", mif_full},

   // DoomRL Arsenal Monsters
   {M(ZombieMan),   mtype_zombie, "FormerHuman"   },
   {M(ShotgunGuy),  mtype_zombie, "FormerSergeant"},
   {M(ChaingunGuy), mtype_zombie, "FormerCommando"},
   {M(DRLACaptain), mtype_zombie, "Former"}, // hue

   // Colorful Hell
   {M(ZombieMan),     mtype_zombie,        "Zombie"},
   {M(ShotgunGuy),    mtype_zombie,        "SG"    },
   {M(ChaingunGuy),   mtype_zombie,        "CGuy"  },
   {M(LostSoul),      mtype_lostsoul,      "LSoul" },
   {M(HellKnight),    mtype_hellknight,    "HK"    },
   {M(Arachnotron),   mtype_arachnotron,   "SP1"   },
   {M(Cacodemon),     mtype_cacodemon,     "Caco"  },
   {M(Archvile),      mtype_archvile,      "Arch"  },
   {M(PainElemental), mtype_painelemental, "PE"    },
   {M(SpiderDemon),   mtype_mastermind,    "Mind"  },
   {M(CyberDemon),    mtype_cyberdemon,    "Cybie" },

   // Shut Up and Lithium
   {M(ZombieMan),     mtype_imp,         "Roach"},
   {M(ZombieMan),     mtype_imp,         "Remnant"},
   {M(Imp),           mtype_imp,         "Turmoil"},
   {M(ShotgunGuy),    mtype_imp,         "Pitkis"},
   {M(ChaingunGuy),   mtype_imp,         "Helliate"},
   {M(Demon),         mtype_demon,       "Satyr"},
   {M(Spectre),       mtype_demon,       "Mush"},
   {M(LostSoul),      mtype_lostsoul,    "Gsoul"},
   {M(Cacodemon),     mtype_cacodemon,   "Accuser"},
   {M(Revenant),      mtype_revenant,    "Famine"},
   {M(PainElemental), mtype_arachnotron, "Writhe"},
   {M(HellKnight),    mtype_hellknight,  "Sonnelion"},
   {M(LostSoul),      mtype_lostsoul,    "Eotu"},
   {M(Arachnotron),   mtype_arachnotron, "Lolth"},
   {M(Mancubus),      mtype_mancubus,    "Malbouge"},
   {M(BaronOfHell),   mtype_baron,       "Abaddon"},
   {M(Archvile),      mtype_archvile,    "Pestilence"},
   {M(SpiderDemon),   mtype_mastermind,  "Granfalloon"},
   {M(CyberDemon),    mtype_cyberdemon,  "SoF"},
   {M(LostSoul),      mtype_lostsoul,    "Karkass"},
   {M(HellKnight),    mtype_imp,         "Doorman"},
   {M(LostSoul),      mtype_lostsoul,    "UnbodiedFury"},
   {M(ShotgunGuy),    mtype_imp,         "Bloodfiend"},
   {M(Demon),         mtype_demon,       "Golem"},
   {M(Demon),         mtype_demon,       "Phantasm"},
   {M(Demon),         mtype_demon,       "Mortuus"},
   {M(ChaingunGuy),   mtype_imp,         "Malum"},
   {M(ShotgunGuy),    mtype_imp,         "Phasma"},
   {M(Spectre),       mtype_demon,       "Licho"},
   {M(Cacodemon),     mtype_cacodemon,   "Mandingo"},
   {M(Cacodemon),     mtype_cacodemon,   "Hierophant"},
   {M(CyberDemon),    mtype_cyberdemon,  "Burrower"},
   {M(Archvile),      mtype_arachnotron, "Aranearum"},
   {M(Cacodemon),     mtype_cacodemon,   "Lividus"},
   {M(HellKnight),    mtype_hellknight,  "Deepone"},
   {M(BaronOfHell),   mtype_baron,       "Deeptwo"},
};
#undef M

// EOF
