// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"

#define name(p) {countof(p), p}

struct pickupname_s {
   size_t const num;
   __str *const ptr;
};

static __str names_unknown[] = {
   "Literally What The Fuck",
   "How Even",
   "Why Are You Doing This"
};

static __str names_cfist[] = {
   "Chaaji Fisuto",
   "Charred Fist",
   "Charged Mr. Fisto",
   "Chargle Fargle"
};

static __str names_pistol[] = {
   "Postil",
   "Peestol",
   "Pinstool",
   "Peetool",
   "Pistell",
   "Pistuh",
   "Listep",
   "hurty peestal",
   "Pete Stool",
   "Bandity Pistal"
};

static __str names_revolver[] = {
   "Rvleolr",
   "Revolrvo",
   "Volvo",
   "Rebolber :=DD"
};

static __str names_shotgun[] = {
   "Shtegnu",
   "Shartgun",
   "Shitgun",
   "Shtognu",
   "Shetgun",
   "Sug Glug"
};

static __str names_ssg[] = {
   "Sbepru Shtetgun",
   "Suber Shodgub",
   "Soer Shotgan",
   "Suupaa Shottogan",
   "Susser Sarsaparilla",
   "Shovel Subbutt",
   "Suggle Kruggle"
};

static __str names_rifle[] = {
   "Cmobta Riffel",
   "Combit Raffle",
   "Combat Rifl",
   "Rifcom Litbl",
   "Bltbm Liffem"
};

static __str names_launcher[] = {
   "Ruckebt Laurence",
   "Rawket Lawnchair",
   "Rokkit Luncher",
   "Pocket Lunchbox"
};

static __str names_plasma[] = {
   "Spalmo Rinfer",
   "Layzuh Beem",
   "Shooty-pew-pew Gun",
   "Elfir Amsalp",
   "Pulsmer Rifram",
   "Plisplam Fimfam",
   "Plastic Hamdonk"
};

static __str names_bfg[] = {
   "Biffgee Nintendo",
   "BFG 8000",
   "Biffug 9999",
   "Bilmap Frignarp Gnu 9",
   "Frosty Balga Gams"
};

static __str names_c_mateba[] = {
   "Metabolism",
   "Materberb",
   "Beet Tam",
   "Able Temabl",
   "Tomato",
   "Material Defender",
   "Murdererererer",
   "Mad Bee",
   "Matatebb",
   "Marteba",
   "Metabe",
   "Mercury Poisoning"
};

static __str names_c_rifle[] = {
   "Sock Rifle",
   "Shock Stifle",
   "Stock Knifle",
   "ShohS RifiR",
};

static __str names_c_smg[] = {
   "Chug Smug",
   "Real Supersand",
   "Secure Gambler",
   "Sag Mill",
   "Santa Surprise",
   "Sandlebar Snorkel",
   "Submarine Screendoor",
   "Sbubby Mcgungus",
   "Slow Reggae",
   "Adolescent Assault Rifle"
};

static __str names_c_sniper[] = {
   "Iron Raffle",
   "Eye on Rustle",
   "Onion Riffraff",
   "Plasmagun's Lazy Cousin",
   "Homemade Pringle Cannon",
   "Idle Riffraff",
   "Ionic Breeze",
   "Ix Rix",
   "Io Rofl",
   "100% Ion",
   "Ions of Ion",
   "Slight Chance of Ions Today",
   "Illegal Rat",
   "Ion Juicer"
};

static __str names_c_delear[] = {
   "Dleay",
   "Bob Dylan",
   "Delet Ears",
   "Dala Llama",
   "Datadoor",
   "die pls",
   "Delicious",
   "Del Taco",
   "Deleleleleelear",
   "Deadlreadl"
};

static __str names_c_starshot[] = {
   "Stock Image",
   "Star Gnu",
   "Stelr Emitroid",
   "Silly Blatt",
   "Stot Stot",
   "Shart Shot",
   "Stargle Shargle",
   "Stumgus Sungus"
};

static __str names_c_shipgun[] = {
   "Stahp Blastroyer",
   "Rats Derkler",
   "Fnukni Gigagnu",
   "Solarr Bitchmace",
   "Stan Distro",
   "Technicolor Bangstick",
   "Shooty Shooty Point 'n Shooty",
   "The Entire Doomsday Engine",
   "Deleterious Thisus",
   "S.S. Enterprise",
   "Stungle Dungle",
   "Srassa Drass"
};

static struct pickupname_s const pickupnames[weapon_max] = {
   [weapon_unknown]    = name(names_unknown),
   [weapon_fist]       = name(names_unknown),
   [weapon_cfist]      = name(names_cfist),
   [weapon_pistol]     = name(names_pistol),
   [weapon_revolver]   = name(names_revolver),
   [weapon_shotgun]    = name(names_shotgun),
   [weapon_ssg]        = name(names_ssg),
   [weapon_rifle]      = name(names_rifle),
   [weapon_launcher]   = name(names_launcher),
   [weapon_plasma]     = name(names_plasma),
   [weapon_bfg]        = name(names_bfg),
   [weapon_c_fist]     = name(names_unknown),
   [weapon_c_mateba]   = name(names_c_mateba),
   [weapon_c_rifle]    = name(names_c_rifle),
   [weapon_c_spas]     = name(names_shotgun),
   [weapon_c_smg]      = name(names_c_smg),
   [weapon_c_sniper]   = name(names_c_sniper),
   [weapon_c_plasma]   = name(names_plasma),
   [weapon_c_starshot] = name(names_c_starshot),
   [weapon_c_shipgun]  = name(names_c_shipgun),
   [weapon_c_delear]   = name(names_c_delear),
};

static void Lith_StupidPickup(struct player *p, int weapon)
{
   static __str uncertainty[] = {
      "Yes.",
      "Maybe.",
      "Hmm, yes. Indeed.",
      "Yeah.",
      "Yep.",
      "Ye.",
      "Mmyes.",
      "Yes. No. Yes?",
      "Yes, no, maybe. I don't know."
   };

   static int fmtnum;
   struct pickupname_s const *names = &pickupnames[weapon];

   if(!fmtnum)
      fmtnum = strtoi_str(L("LITH_PICKUP_NUM"), null, 10);

   int ifmt  = ACS_Random(0, fmtnum);
   int iname = ACS_Random(1, names->num - 1);
   int iunc  = ACS_Random(0, countof(uncertainty) - 1);
   int flag  = strtoi_str(Language("LITH_PICKUP_%.3i_FLAGS", ifmt), null, 0);

   if(flag & 2) {
      ifmt = ACS_Random(0, fmtnum);
      flag = strtoi_str(Language("LITH_PICKUP_%.3i_FLAGS", ifmt), null, 0);
   }

   __str fmt = Language("LITH_PICKUP_%.3i", ifmt);

   if(flag & 1 && flag & 4)
      p->log(1, fmt, names->ptr[iname], names->ptr[iname], uncertainty[iunc]);
   else if(flag & 1)
      p->log(1, fmt, names->ptr[iname], names->ptr[iname]);
   else if(flag & 4)
      p->log(1, fmt, names->ptr[iname], uncertainty[iunc]);
   else
      p->log(1, fmt, names->ptr[iname]);
}

void Lith_PickupMessage(struct player *p, weaponinfo_t const *info)
{
   if(p->getCVarI("lith_player_stupidpickups"))
      Lith_StupidPickup(p, info->type);
   else if(info->name)
      p->log(1, StrParam("%S", L("LITH_PICKUP_000")),
         Language("LITH_INFO_SHORT_%S", info->name));
   else
      p->log(1, "Acquired impossible object");
}

// EOF

