// Copyright © 2017 Graham Sanderson, all rights reserved.
#ifndef LITH_ATTRIB_H
#define LITH_ATTRIB_H

#define ATTR_MAX 150
#define ATTR_VIS_MAX 100
#define ATTR_VIS_DIFF (ATTR_MAX - ATTR_VIS_MAX)

enum
{
   at_acc,
   at_def,
   at_str,
   at_vit,
   at_stm,
   at_luk,
   at_rge,
   at_max
};

struct player_attr_theta
{
   unsigned points;
   unsigned attrs[at_max];
};

struct player_attributes
{
   bool          autolevel;
   unsigned long exp, expnext;
   unsigned      level;
   __str         names[at_max];
   [[__anonymous]] struct player_attr_theta cur;
                   struct player_attr_theta sup;
};

#endif
