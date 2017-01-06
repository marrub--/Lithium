#include "lith_common.h"
#include "lith_player.h"
#include "lith_pickups.h"

#define name(p) {sizeof(p) / sizeof(*p), p}

struct pickupfmt_s
{
   int flag;
   __str fmt;
};

struct pickupname_s
{
   size_t const num;
   __str *const ptr;
};

static __str names_unknown[] = {
   "Unknown Weapon",
   "Literally What The Fuck",
   "How Even",
   "Why Are You Doing This"
};

static __str names_pistol[] = {
   "Pistol",
   "Postil",
   "Peestol",
   "Pinstool",
   "Peetool",
   "Pistell",
   "Pistuh",
   "Listep",
   "hurty peestal"
};

static __str names_shotgun[] = {
   "Shotgun",
   "Shtegnu",
   "Shartgun",
   "Shitgun",
   "Shtognu",
   "Shetgun"
};

static __str names_rifle[] = {
   "Combat Rifle",
   "Cmobta Riffel",
   "Combit Raffle",
   "Combat Rifl",
   "Rifcom Litbl",
   "Bltbm Liffem"
};

static __str names_launcher[] = {
   "Grenade Launcher",
   "Ruckebt Laurence",
   "Rawket Lawnchair",
   "Rokkit Luncher"
};

static __str names_plasma[] = {
   "Plasma Rifle",
   "Spalmo Rinfer",
   "Layzuh Beem",
   "Shooty-pew-pew Gun",
   "Elfir Amsalp"
};

static __str names_bfg[] = {
   "Omega Cannon",
   "Biffgee Nintendo",
   "BFG 8000",
   "Biffug 9999",
   "Bilmap Frignarp Gnu 9"
};

// Flags
// 1: Two copies of name
// 2: Very rare
// 4: Randomized certainty
static struct pickupfmt_s const pickupfmt[] = {
   0, "You got the %S!",
   0, "Oh yes, the %S.",
   0, "The %S has been acquired.",
   0, "Snatched up a %S!",
   0, "Oh baby, it's time for %S!",
   0, "There was a %S here, but you stole it. It is now in your inventory. You Monster.",
   0, "Acquired a %S.",
   0, "The %S is safe and sound with you.",
   0, "I bet that %S totally wanted you to pick it up.",
   0, "It's not like the %S wanted you to pick it up or anything.",
   0, "It's a %S... I think.",
   4, "You thought you picked up a %S ... %S",
   0, "1 in 3 of every %Ss do not approve of being picked up.",
   0, "Studies show that normal people do not steal %Ss.",
   0, "OH NO, NOT %S",
   4, "What? Is that.. %S? ... %S",
   0, "Magnetically attached a %S to your body, crushing your every bone.",
   0, "DavidPH was here, nyu.",
   0, "%S-senpai still won't notice you.",
   0, "Grabbed a %S!",
   0, "Snatched up a %S!",
   0, "Embraced the %S carefully!",
   0, "There's a thing that looks like a %S here. You picked it up. Past-tense.",
   0, "Another day, another %S.",
   0, "Xaser didn't make this %S, so it sucks. You should drop it.",
   0, "\Cg%S. Finally.",
   4, "The %S. %S",
   0, "I don't know about you, but I quite like the %S. I'll pick it up for you.",
   0, "...\nHuh? You do something? I wasn't paying attention.",
   1, "The %S. It is very much a %S.",
   0, "%S, desu.",
   0, "?? ??? ???? ?? ????????? ???? ??",
   1, "My %S lies over the ocean, my %S lies over the sea.",
   0, "Liberated the %S from gypsies.",
   0, "Granted the %S true independence.",
   1, "The %S, for maximum %S-ing.",
   4, "Brutal %S, featuring 5x more explosions! .. %S",
   2, "aeiou",
   2, "What the fuck did you just fucking say about me, you little bitch?\n"
      "I'll have you know I graduated top of my class in the Navy Seals,\n"
      "and I've been involved in numerous secret raids on Al-Quaeda, and I\n"
      "have over 300 confirmed kills.\n"
      "I am trained in gorilla warfare and I'm the top sniper\n"
      "in the entire US armed forces.\n"
      "You are nothing to me but just another target. I will wipe you the\n"
      "fuck out with precision the likes of which has never been seen before\n"
      "on this Earth, mark my fucking words.\n"
      "You think you can get away with saying that shit to me over the Internet?\n"
      "Think again, fucker. As we speak I am contacting my secret network of\n"
      "spies across the USA and your IP is being traced right now so you\n"
      "better prepare for the storm, maggot.\n"
      "The storm that wipes out the pathetic little thing you call your life.\n"
      "You're fucking dead, kid.\n"
      "I can be anywhere, anytime, and I can kill you in over\n"
      "seven hundred ways, and that's just with my bare hands. Not only am I\n"
      "extensively trained in unarmed combat, but I have access to the entire\n"
      "arsenal of the United States Marine Corps and I will use it to its full\n"
      "extent to wipe your miserable ass off the face of the continent, you little shit."
      "If only you could have known what unholy retribution your little \"clever\"\n"
      "comment was about to bring down upon you, maybe you would have held\n"
      "your fucking tongue. But you couldn't, you didn't, and now you're paying\n"
      "the price, you goddamn idiot. I will shit fury all over you\n"
      "and you will drown in it. You're fucking dead, kiddo.",
};

static __str uncertainty[] = {
   "Yes.",
   "Maybe.",
   "Hmm, yes. Indeed.",
   "Yeah.",
   "Yep."
};

static struct pickupname_s const pickupnames[weapon_max] = {
   [weapon_unknown]  = name(names_unknown),
   [weapon_pistol]   = name(names_pistol),
   [weapon_shotgun]  = name(names_shotgun),
   [weapon_rifle]    = name(names_rifle),
   [weapon_launcher] = name(names_launcher),
   [weapon_plasma]   = name(names_plasma),
   [weapon_bfg]      = name(names_bfg)
};

static size_t const pickupfmtnum   = sizeof(pickupfmt)   / sizeof(*pickupfmt);
static size_t const uncertaintynum = sizeof(uncertainty) / sizeof(*uncertainty);

void Lith_StupidPickup(int weapon)
{
   struct pickupname_s const *names = &pickupnames[weapon];
   
   int ifmt  = ACS_Random(0, pickupfmtnum   - 1);
   int iname = ACS_Random(1, names->num     - 1);
   int iunc  = ACS_Random(0, uncertaintynum - 1);
   
   if(pickupfmt[ifmt].flag & 2)
      ifmt = ACS_Random(0, pickupfmtnum - 1);
   
   __str fmt = StrParam("\Cj%S", pickupfmt[ifmt].fmt);
   int flag = pickupfmt[ifmt].flag;
   
   if(flag & 1 && flag & 4)
      Log(fmt, names->ptr[iname], names->ptr[iname], uncertainty[iunc]);
   else if(flag & 1)
      Log(fmt, names->ptr[iname], names->ptr[iname]);
   else if(flag & 4)
      Log(fmt, names->ptr[iname], uncertainty[iunc]);
   else
      Log(fmt, names->ptr[iname]);
}

void Lith_IntelligentPickup(int weapon)
{
   Log(pickupfmt[0].fmt, pickupnames[weapon].ptr[0]);
}

// EOF

