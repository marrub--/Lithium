// Copyright © 2017 Alison Sanderson, all rights reserved.
#ifdef Ret
   #define Case(n) Ret(n) case UPGR_##n:;
   #define A(n) Fn_F(n, Activate)
   #define D(n) Fn_F(n, Deactivate)
   #define U(n) Fn_S(n, Update)
   #define E(n) Fn_F(n, Enter)
   #define R(n) Fn_F(n, Render)
#else
   #define Case(n)
   #define A(n) Fn_F(n, Activate)
   #define D(n) Fn_F(n, Deactivate)
   #define U(n) Fn_S(n, Update)
   #define E(n) Fn_F(n, Enter)
   #define R(n) Fn_F(n, Render)
#endif

// EOF
