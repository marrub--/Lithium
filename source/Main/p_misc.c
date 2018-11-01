// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"

script ext("ACS")
void Lith_GiveMeAllOfTheScore(void)
{
   withplayer(LocalPlayer) p->giveScore(0x7FFFFFFFFFFFFFFFFFFFFFFFLL, true);
}

script ext("ACS")
void Lith_GiveHealthBonus(int amount)
{
   withplayer(LocalPlayer)
   {
      amount += p->health;
      if(amount > p->maxhealth + 100) amount = p->maxhealth + 100;
      p->health = amount;
   }
}

script ext("ACS")
void Lith_GiveHealth(int amount)
{
   withplayer(LocalPlayer)
   {
      amount += p->health;
      amount *= 1 + p->attr.attrs[at_vit] / 80.0;
      if(amount > p->maxhealth) amount = p->maxhealth;
      p->health = amount;
   }
}

script ext("ACS")
bool Lith_CheckHealth()
{
   withplayer(LocalPlayer) return p->health < p->maxhealth;
   return 0;
}

script ext("ACS")
void Lith_Discount()
{
   withplayer(LocalPlayer) p->discount = 0.9;
}

script type("net") ext("ACS")
void Lith_Glare(void)
{
   withplayer(LocalPlayer)
   {
      ACS_FadeTo(255, 255, 255, 1.0, 0.0);

      ACS_LocalAmbientSound("player/glare", 127);
      ACS_LineAttack(0, p->yaw, p->pitch, 1, "Lith_Dummy", "None",
         32767.0, FHF_NORANDOMPUFFZ | FHF_NOIMPACTDECAL);

      ACS_Delay(14);

      ACS_FadeTo(255, 255, 255, 0.0, 0.2);

      ACS_Delay(19);
   }
}

// EOF
