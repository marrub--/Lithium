// Copyright © 2017 Graham Sanderson, all rights reserved.
#ifndef LITH_ATTRIB_H
#define LITH_ATTRIB_H

struct player_attributes
{
   bool autolevel;
   unsigned long exp, expnext;
   unsigned level;
   unsigned points;
   unsigned acc : 8, def : 8, str : 8, vit : 8,
            arm : 8, stm : 8, luk : 8, rge : 8;
};

#endif
