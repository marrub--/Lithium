// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#if LITHIUM
#include "lith_common.h"
#include "lith_world.h"
#include "lith_player.h"
#include "lith_dialogue.h"
#include "lith_file.h"
#include "lith_tokbuf.h"

StrEntON

#define LogTok(tok, s) \
   Log("(%i:%i) " s " (%i:\"%s\")", tok->orig.line, tok->orig.colu, tok->type, \
      tok->textV ? tok->textV : c"<no string>")

#define LogOri(tok, s) \
   Log("(%i:%i) " s, tok->orig.line, tok->orig.colu)

// Extern Objects ------------------------------------------------------------|

extern struct dlgdef *lmvar dlgdefs;

// Static Functions ----------------------------------------------------------|

static int *NextCode(struct pstate *d);
static int AddString(struct pstate *d, char const *s, int l);
static void GetStatement(struct pstate *d);

// Types ---------------------------------------------------------------------|

enum
{
   #define Str(name, str) STR_##name,
   #include "lith_dlgstrtable.h"
   STR_MAX
};

struct strent {
   char const *key, *str;
   int name;
   size_t keyhash;
   struct strent *next, **prev;
};

struct dlgfunc {
   char const *name, *args;
   int lit[4];
   void (*genCode)(struct pstate *d, struct arg *argv, int argc);
   size_t keyhash;
   struct dlgfunc *next, **prev;
};

struct arg {
   int  s;
   int  i;
   char t;
};

struct pstate {
   __prop nextCode  {call: NextCode(this)}
   __prop addString {call: AddString(this)}

   struct tokbuf tb;
   struct dlgdef *def;
};

GDCC_HashMap_Decl(strtable_t,  char const *, struct strent)
GDCC_HashMap_Decl(functable_t, char const *, struct dlgfunc)

// Static Objects ------------------------------------------------------------|

static strtable_t  stbl;
static functable_t ftbl;

// Static Functions ----------------------------------------------------------|

#define strtable_t_GetKey(o) ((o)->key)
#define strtable_t_GetNext(o) (&(o)->next)
#define strtable_t_GetPrev(o) (&(o)->prev)
#define strtable_t_HashKey(k) (lstrhash(k))
#define strtable_t_HashObj(o) ((o)->keyhash)
#define strtable_t_KeyCmp(l, r) (strcmp(l, r))
GDCC_HashMap_Defn(strtable_t, char const *, struct strent)

#define functable_t_GetKey(o) ((o)->name)
#define functable_t_GetNext(o) (&(o)->next)
#define functable_t_GetPrev(o) (&(o)->prev)
#define functable_t_HashKey(k) (lstrhash(k))
#define functable_t_HashObj(o) ((o)->keyhash)
#define functable_t_KeyCmp(l, r) (strcmp(l, r))
GDCC_HashMap_Defn(functable_t, char const *, struct dlgfunc)

static int StrName(char const *s)
{
   ifauto(struct strent *, e, stbl.find(s))
      return e->name;
   else
      return STR_NULL;
}

static int *NextCode(struct pstate *d)
{
   Vec_GrowN(d->def->code, 1, 32);
   return &Vec_Next(d->def->code);
}

static int AddString(struct pstate *d, char const *s, int l)
{
   int p = d->def->stabC;
   Vec_Grow(d->def->stab, l);
   Vec_MoveEnd(d->def->stab, s, l);
   return p;
}

#define GenCode_Reg(code) \
        if(d->def->stabV[argv[0].s] == 'a') *d->nextCode() = DCD_##code##_A; \
   else if(d->def->stabV[argv[0].s] == 'b') *d->nextCode() = DCD_##code##_B; \
   else if(d->def->stabV[argv[0].s] == 'c') *d->nextCode() = DCD_##code##_C; \
   else if(d->def->stabV[argv[0].s] == 'd') *d->nextCode() = DCD_##code##_D; \
   else

static void GenCode_Trace(struct pstate *d, struct arg *argv, int argc)
{
   GenCode_Reg(TRACE)
   {
      *d->nextCode() = DCD_TRACE_S;
      *d->nextCode() = argv[0].s;
   }
}

static void GenCode_Push(struct pstate *d, struct arg *argv, int argc)
{
   GenCode_Reg(PUSH)
   {
      *d->nextCode() = DCD_PUSH_I;
      *d->nextCode() = argv[0].s;
   }
}

static void GenCode_Pop(struct pstate *d, struct arg *argv, int argc)
{
   GenCode_Reg(POP)
   {
      struct token *tok = d->tb.reget();
      LogOri(tok, "GenCode_Pop: invalid argument");
   }
}

#define GenCode_Arith(code) \
   static void GenCode_Arith_##code(struct pstate *d, struct arg *argv, int argc) \
   { \
      GenCode_Reg(code) \
      { \
         *d->nextCode() = DCD_##code##_I; \
         *d->nextCode() = argv[0].i; \
      } \
   }

GenCode_Arith(ADD)
GenCode_Arith(SUB)
GenCode_Arith(MUL)
GenCode_Arith(DIV)
GenCode_Arith(MOD)
GenCode_Arith(IOR)
GenCode_Arith(AND)
GenCode_Arith(XOR)
GenCode_Arith(LSH)
GenCode_Arith(RSH)

#undef GenCode_Arith

static void GenCode_Generic(struct pstate *d, struct arg *argv, int argc)
{
   for(int i = 0; i < argc; i++)
   {
      if(argv[i].t == 'S') *d->nextCode() = argv[i].s;
      else                 *d->nextCode() = argv[i].i;
   }
}

// Parses and generates code for a conditional statement.
static void GetCode_Cond(struct pstate *d)
{
   int code = DCD_NOP;
   struct token *tok = d->tb.get();

   // Get the code to generate.
   if(tok->type == tok_identi)
   {
      switch(StrName(tok->textV)) {
      case STR_item:  code = DCD_JNITEM;  break;
      case STR_class: code = DCD_JNCLASS; break;
      default: LogOri(tok, "GetCode_Cond: invalid conditional"); return;
      }
   }
   else
      LogTok(tok, "GetCode_Cond: expected identifier");

   // Generate code.
   int ptr = 0;

   tok = d->tb.get();
   if(tok->type == tok_identi || tok->type == tok_string)
   {
      *d->nextCode() = code;
      ptr = d->def->codeC;
      d->nextCode();

      if(code == DCD_JNCLASS)
      {
         StrEntOFF
         #define LITH_X(l, r) \
            if(strcmp(tok->textV, #l) == 0 || strcmp(tok->textV, #r) == 0) \
               {*d->nextCode() = r; goto ok;}
         #include "lith_player.h"

         LogOri(tok, "GetCode_Cond: invalid playerclass type");
         return;
      }
      else
         *d->nextCode() = d->addString(tok->textV, tok->textC);
   }
   else
      LogOri(tok, "GetCode_Cond: invalid token in conditional statement");

   // Generate statement.
ok:
   GetStatement(d);

   tok = d->tb.get();
   if(tok->type == tok_identi && StrName(tok->textV) == STR_else)
   {
      int tmp = ptr;

      // Add jump to end.
      *d->nextCode() = DCD_JMP;
      ptr = d->def->codeC;
      d->nextCode();

      // Set original jump target to here.
      d->def->codeV[tmp] = d->def->codeC;

      // Generate statement.
      GetStatement(d);
   }
   else
      d->tb.unget();

   // Set the pointer in the generated code to be after the statement.
   if(ptr) d->def->codeV[ptr] = d->def->codeC;
}

// Parses and generates code for an option statement.
static void GetCode_Option(struct pstate *d)
{
   struct token *tok = d->tb.get();

   // Generate code.
   int ptr = 0;
   if(tok->type == tok_identi || tok->type == tok_string)
   {
      *d->nextCode() = DCD_PUTOPT;
      ptr = d->def->codeC;
      d->nextCode();
      *d->nextCode() = d->addString(tok->textV, tok->textC);
   }
   else
      LogOri(tok, "GetCode_Option: invalid option parameter");

   // Generate statement.
   GetStatement(d);

   // Set the pointer in the generated code to be after the statement.
   if(ptr) d->def->codeV[ptr] = d->def->codeC;
}

// Parses and generates code for an exec statement.
static void GetCode_Exec(struct pstate *d)
{
   *d->nextCode() = DCD_TRMWAIT;
   GetStatement(d);
}

// Parses and generates code for a generic statement.
static void GetCode_Generic(struct pstate *d)
{
   struct token *tok = d->tb.reget();
   LogDebug(log_dlg, "call: %s", tok->textV);

   // Get the function to generate.
   struct dlgfunc const *func = ftbl.find(tok->textV);
   if(!func) {
      LogOri(tok, "GetCode_Generic: invalid function in dialogue code");
      return;
   }

   // Get arguments.
   struct arg argv[8] = {};
   int argc = 0;
   int lit  = 0;

   while(func->args[argc])
   {
      if(func->args[argc] == 'L')
      {
         argv[argc++].i = func->lit[lit++];
         continue;
      }

      if(!(tok = d->tb.get())->textV)
      {
         LogTok(tok, "GetCode_Generic: invalid token in argument list");
         return;
      }

      switch(argv[argc].t = func->args[argc]) {
      case 'I': argv[argc++].i = strtoi(tok->textV, null, 0);          break;
      case 'S': argv[argc++].s = d->addString(tok->textV, tok->textC); break;
      }

      LogDebug(log_dlg, "arg %i: %s", argc, tok->textV);

      if(!d->tb.drop(tok_comma) || d->tb.drop(tok_semico))
         break;
   }

   // Fill in unfinished arguments.
   while(func->args[argc])
   {
      switch(argv[argc].t = func->args[argc]) {
      case 'I': argv[argc++].i = 0;                    break;
      case 'S': argv[argc++].s = d->addString(c"", 0); break;
      case 'L': argv[argc++].i = func->lit[lit++];     break;
      }

      LogDebug(log_dlg, "arg %i emptied", argc);
   }

   func->genCode(d, argv, argc);
}

static void GetCode_Text(struct pstate *d, struct token *tok, int code)
{
   *d->nextCode() = code;
   *d->nextCode() = d->addString(tok->textV, tok->textC);
}

// Parse and generate a line of code.
static void GetCode_Line(struct pstate *d)
{
   struct token *tok = d->tb.get();

   switch(tok->type)
   {
   case tok_identi:
      LogDebug(log_dlg, "GetCode_Line: %s", tok->textV);
      switch(StrName(tok->textV)) {
      case STR_if:     GetCode_Cond   (d); break;
      case STR_option: GetCode_Option (d); break;
      case STR_exec:   GetCode_Exec   (d); break;
      default:         GetCode_Generic(d); break;
      }
      break;
   case tok_quote:  GetCode_Text(d, d->tb.reget(), DCD_ADDTEXT);      break;
   case tok_dollar: GetCode_Text(d, d->tb.  get(), DCD_ADDTEXTLOCAL); break;
   case tok_semico:
   case tok_eof:
      break;
   default:
      LogTok(tok, "GetCode_Line: invalid token in line");
   }
}

// Parse and generate a block statement.
static void GetBlock(struct pstate *d)
{
   while(!d->tb.drop(tok_bracec) && !d->tb.drop(tok_eof))
      GetStatement(d);
}

// Parse and generate a concat block statement.
static void GetConcatBlock(struct pstate *d)
{
   *d->nextCode() = DCD_CONCAT;
   while(!d->tb.drop(tok_at2) && !d->tb.drop(tok_eof))
      GetStatement(d);
   *d->nextCode() = DCD_CONCATEND;
}

// Parse and generate a statement.
static void GetStatement(struct pstate *d)
{
        if(d->tb.drop(tok_braceo)) GetBlock(d);
   else if(d->tb.drop(tok_at2))    GetConcatBlock(d);
   else                            GetCode_Line(d);
}

static void SetupDialogue(struct pstate *d, int num)
{
   struct dlgdef *last = d->def;

   d->def = Salloc(struct dlgdef);
   d->def->num = num;

   if(!last) dlgdefs    = d->def;
   else      last->next = d->def;

   LogDebug(log_dlg, "set up dialogue %i", num);
}

static void GetDecl_Dialogue(struct pstate *d)
{
   struct token *tok = d->tb.get();

   if(tok->type == tok_number) {
      SetupDialogue(d, strtoi(tok->textV, null, 0));
      LogDebug(log_dlg, "\n---\ndialogue %i (%i)\n---",
         d->def->num, d->def->codeC);
   } else {
      LogOri(tok, "GetDecl_Dialogue: invalid dialogue number token");
   }
}

static void GetDecl_Terminal(struct pstate *d)
{
   struct token *tok = d->tb.get();

   if(tok->type == tok_number) {
      SetupDialogue(d, -strtoi(tok->textV, null, 0));
      LogDebug(log_dlg, "\n---\nterminal %i (%i)\n---",
         -d->def->num, d->def->codeC);
   } else {
      LogOri(tok, "GetDecl_Terminal: invalid terminal number token");
   }
}

static void SetupPage(struct pstate *d, int num)
{
   d->def->pages[num] = d->def->codeC;

   LogDebug(log_dlg, "--- page %i (%i)", num, d->def->codeC);
}

static void GetDecl_Page(struct pstate *d)
{
   struct token *tok = d->tb.get();

   if(tok->type == tok_number)
      SetupPage(d, strtoi(tok->textV, null, 0));
   else
      LogOri(tok, "GetDecl_Page: invalid page number token");

   GetStatement(d);

   *d->nextCode() = DCD_DLGWAIT;
   *d->nextCode() = DCD_DIE;
}

static void GetDecl_TrmPage(struct pstate *d, int num)
{
   SetupPage(d, num);
   GetStatement(d);
   *d->nextCode() = DCD_TRMWAIT;
   *d->nextCode() = DCD_DIE;
}

// Extern Functions ----------------------------------------------------------|

// Loads all string indices into the global stbl, and all function
// prototypes into the global ftbl.
void Lith_GSInit_Dialogue(void)
{
   StrEntOFF

   static struct dlgfunc funcs[] = {
      {"nop", "L", DCD_NOP},

      {"push", "S", .genCode = GenCode_Push},
      {"pop",  "S", .genCode = GenCode_Pop},

      {"add", "I", .genCode = GenCode_Arith_ADD},
      {"sub", "I", .genCode = GenCode_Arith_SUB},
      {"mul", "I", .genCode = GenCode_Arith_MUL},
      {"div", "I", .genCode = GenCode_Arith_DIV},
      {"mod", "I", .genCode = GenCode_Arith_MOD},
      {"ior", "I", .genCode = GenCode_Arith_IOR},
      {"and", "I", .genCode = GenCode_Arith_AND},
      {"xor", "I", .genCode = GenCode_Arith_XOR},
      {"lsh", "I", .genCode = GenCode_Arith_LSH},
      {"rsh", "I", .genCode = GenCode_Arith_RSH},

      {"die",  "L", DCD_DIE},
      {"exit", "L", DCD_DIE},

      {"page", "LI", DCD_JPAGE},

      {"script",      "LIIIII", DCD_SCRIPTI},
      {"scriptnamed", "LSIIII", DCD_SCRIPTS},

      {"trace", "S", .genCode = GenCode_Trace},

      {"intralevelteleport", "LI", DCD_TELEPORT_INTRALEVEL},
      {"interlevelteleport", "LI", DCD_TELEPORT_INTERLEVEL},

      {"text",      "LS",  DCD_SETTEXT},
      {"local",     "LS",  DCD_SETTEXTLOCAL},
      {"addtext",   "LS",  DCD_ADDTEXT},
      {"addlocal",  "LS",  DCD_ADDTEXTLOCAL},
      {"setstr",    "LIS", DCD_SETSTRING},
      {"name",      "LLS", DCD_SETSTRING, DSTR_NAME},
      {"icon",      "LLS", DCD_SETSTRING, DSTR_ICON},
      {"remote",    "LLS", DCD_SETSTRING, DSTR_REMOTE},
      {"concat",    "L",   DCD_CONCAT},
      {"endconcat", "L",   DCD_CONCATEND},

      {"dlgwait", "L", DCD_DLGWAIT},

      {"logon",   "LS", DCD_LOGON},
      {"logoff",  "LS", DCD_LOGOFF},
      {"info",    "L",  DCD_INFO},
      {"pict",    "LS", DCD_PICT},
      {"trmwait", "L",  DCD_TRMWAIT},
   };

   static struct strent strs[] = {
      #define Str(name, str) {#name, #str},
      #include "lith_dlgstrtable.h"
   };

   strtable_t_ctor(&stbl, countof(strs), 1);
   for(int i = 0; i < countof(strs); i++) {
      strs[i].keyhash = lstrhash(strs[i].key);
      strs[i].name = i;
      stbl.insert(&strs[i]);
   }

   functable_t_ctor(&ftbl, countof(funcs), 1);
   for(int i = 0; i < countof(funcs); i++) {
      funcs[i].keyhash = lstrhash(funcs[i].name);
      if(!funcs[i].genCode) funcs[i].genCode = GenCode_Generic;
      ftbl.insert(&funcs[i]);
   }
}

void Lith_LoadMapDialogue(void)
{
   // Free any previous dialogue definitions.
   if(dlgdefs)
   {
      for(struct dlgdef *def = dlgdefs; def;) {
         struct dlgdef *next = def->next;
         Vec_Clear(def->code);
         Vec_Clear(def->stab);
         Dalloc(def);
         def = next;
      }

      Dalloc(dlgdefs);
      dlgdefs = null;
   }

   struct pstate d = {{
      .bbeg = 4, .bend = 10,
      .fp = W_Open(StrParam("lfiles/Dialogue_%tS.txt", PRINTNAME_LEVEL), c"r")
   }};

   if(!d.tb.fp) return;

   d.tb.ctor();

   for(struct token *tok; (tok = d.tb.get())->type != tok_eof;)
   {
      if(tok->type != tok_identi) {
         LogTok(tok, "Lith_LoadMapDialogue: invalid toplevel token");
         break;
      }

      switch(StrName(tok->textV))
      {
      case STR_dialogue:   GetDecl_Dialogue(&d); break;
      case STR_page:       GetDecl_Page    (&d); break;
      case STR_terminal:   GetDecl_Terminal(&d); break;
      case STR_failure:    GetDecl_TrmPage (&d, DTRMPAGE_FAILURE);    break;
      case STR_finished:   GetDecl_TrmPage (&d, DTRMPAGE_FINISHED);   break;
      case STR_unfinished: GetDecl_TrmPage (&d, DTRMPAGE_UNFINISHED); break;
      default:
         Log("Lith_LoadMapDialogue: invalid identifier \"%s\"", tok->textV);
         goto done;
      }
   }

done:
   d.tb.dtor();
   fclose(d.tb.fp);

   if(world.dbgLevel & log_dlg)
   {
      StrEntOFF
      static char const *dcdnames[] = {
         #define DCD(name) #name,
         #include "lith_dialogue.h"
      };

      for(struct dlgdef *def = dlgdefs; def; def = def->next) {
         Log(s"Dumping code for script %i...", def->num);
         for(int i = 0; i < def->codeC; i++)
            Log(s"%10i %s", def->codeV[i], def->codeV[i] < countof(dcdnames)
               ? dcdnames[def->codeV[i]] : c"");
         Log(s"Dumping string table for script %i...\n%s");
         Lith_PrintMem(def->stabV, def->stabC);
      }

      Log(s"Done.");
   }
}
#endif

// EOF
