// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_common.h"

#include <GDCC/HashMap.h>

struct metric
{
   int key;
   int xadv;
   int yofs;

   struct metric *next, **prev;
};

#define metricmap_t_GetKey(o) ((o)->key)
#define metricmap_t_GetNext(o) (&(o)->next)
#define metricmap_t_GetPrev(o) (&(o)->prev)
#define metricmap_t_HashKey(k) (k)
#define metricmap_t_HashObj(o) ((o)->key)
#define metricmap_t_KeyCmp(l, r) ((l) - (r))
GDCC_HashMap_Decl(metricmap_t, int, struct metric)
GDCC_HashMap_Defn(metricmap_t, int, struct metric)

static metricmap_t metrics;
noinit static struct metric mttab[1024];
static int mttab_p;

//
// Lith_InitFontMetrics
//
script ext("ACS")
void Lith_InitFontMetrics(void)
{
   metricmap_t_ctor(&metrics, 64, 64);
}

//
// Lith_SetFontMetric
//
script ext("ACS")
void Lith_SetFontMetric(int key, int xadv, int yofs)
{
   struct metric *metr = &mttab[mttab_p++];

   if(mttab_p > countof(mttab))
      Log("metrics overflow, yell at developer");

   metr->key  = key;
   metr->xadv = xadv;
   metr->yofs = yofs;

   metrics.insert(metr);
}

//
// Lith_GetFontMetric
//
script ext("ACS")
struct metric *Lith_GetFontMetric(int key)
{
   return metrics.find(key);
}

//
// Lith_Metr_Xadv
//
script ext("ACS")
int Lith_Metr_Xadv(struct metric *metr)
{
   return metr->xadv;
}

//
// Lith_Metr_Yofs
//
script ext("ACS")
int Lith_Metr_Yofs(struct metric *metr)
{
   return metr->yofs;
}

//
// Lith_CloseFontMetrics
//
script type("unloading")
void Lith_CloseFontMetrics(void)
{
   metricmap_t_dtor(&metrics);
   mttab_p = 0;
}

// EOF
