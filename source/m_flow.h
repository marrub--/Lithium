// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Flow control statements.                                                 │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef m_flow_h
#define m_flow_h

#define ifw(decl, ...) __with(decl;) if(__VA_ARGS__)
#define ifauto(type, name, ...) ifw(type name = (__VA_ARGS__), name)

#define statement(body) if(1) {body} else unwrap_retn()

#define unwrap_do(e, stmt) statement(if((e)->some) statement(stmt);)
#define unwrap_retn() [[return]] __asm("Rjnk()")
#define unwrap(e) \
   unwrap_do(e, { \
      unwrap_retn(); \
   })
#define unwrap_print(e) \
   unwrap_do(e, { \
      PrintErr(_p((e)->err)); \
   })
#define unwrap_goto(e, label) \
   unwrap_do(e, { \
      goto label; \
   })

#define gosub_(label, lh) \
   statement({ \
      _gsret = &&_l##lh; \
      goto label; \
   _l##lh:; \
   })
#define gosub__(label, lh)  gosub_ (label, lh)
#define gosub___(label)     gosub__(label, LineHashIdent)
#define gosub(label, ...)   __with((__VA_ARGS__);) gosub___(label)
#define gosub_ret()         goto *_gsret
#define gosub_enable()      lbl _gsret = lnil

#define err_set(e, buf) ((e)->some = true, (e)->err = buf, (void)0)

struct err {
   bool some;
   cstr err;
};

#endif
