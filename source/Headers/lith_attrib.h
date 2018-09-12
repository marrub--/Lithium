// Copyright © 2017 Graham Sanderson, all rights reserved.
#ifndef LITH_ATTRIB_H
#define LITH_ATTRIB_H

#define ATTR_MAX 60
#define ATTR_VIS_MAX 50
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
   u32 points;
   u32 attrs[at_max];
};

struct player_attributes
{
   bool  autolevel;
   u64   exp, expnext;
   u32   level;
   __str names[at_max];

   [[__anonymous]]
   struct player_attr_theta cur;
   struct player_attr_theta sup;
};

#endif
