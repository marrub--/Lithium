// Copyright © 2017 Graham Sanderson, all rights reserved.
#ifndef LITH_ATTRIB_H
#define LITH_ATTRIB_H

#define ATTR_BAS_MAX 60
#define ATTR_VIS_MAX 50
#define ATTR_VIS_DIFF (ATTR_BAS_MAX - ATTR_VIS_MAX)

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

struct player_attributes
{
   u64   exp, expnext;
   u32   level;
   __str names[at_max];
   u32   attrs[at_max];
   __str lvupstr;
};

#endif
