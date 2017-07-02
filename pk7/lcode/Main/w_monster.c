
list_t dmonhash[DMONHASH_MAX];

dmon_t *Dmon(int id)
{
   int hash = id % DMONHASH_MAX;
   Lith_ForList(dmon_t *m, dmonhash[hash])
      if(m && m->id == id && m->active)
         return m;
   
   return null;
}

dmon_t *AllocDmon(void)
{
   int Lith_MapVariable curid;
   
   int id   = ++curid;
   int hash = id % DMONHASH_MAX;
   
   dmon_t *m = calloc(1, sizeof(*m));
   InitDmon(m);
   
   dmonhash[hash].prev.link(&m->link);
   
   return m;
}

[[__call("ScriptS")]]
void Lith_MonsterMain(dmon_t *m)
{
   for(;;)
   {
      ACS_Delay(1);
   }
}

// EOF

