/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Opening segment of u_func.h.
 *
 * ---------------------------------------------------------------------------|
 */

#if defined(Ret)
   #define Case(n) Ret(n) case UPGR_##n:
#else
   #define Case(n)
#endif
#define A(n) Fn_F(n, Activate)
#define D(n) Fn_F(n, Deactivate)
#define U(n) Fn_S(n, Update)
#define E(n) Fn_F(n, Enter)
#define R(n) Fn_F(n, Render)

/* EOF */
