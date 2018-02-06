// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_world.h"
#include "lith_player.h"
#include "lith_dialogue.h"
#include "lith_tokbuf.h"

#define LogTok(tok, s) \
   Log("(%i:%i) " s " (%i:\"%s\")", tok->orig.line, tok->orig.colu, tok->type, \
      tok->textV ? tok->textV : c"<no string>")

#define LogOri(tok, s) \
   Log("(%i:%i) " s, tok->orig.line, tok->orig.colu)

// Extern Objects ------------------------------------------------------------|

extern dlgdef_t *lmvar dlgdefs;

// Static Functions ----------------------------------------------------------|

[[__optional_args(1)]]
static int *NextCode(struct dlgparsestate_s *d);
static void GetStatement(struct dlgparsestate_s *d);

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
   void (*genCode)(struct dlgparsestate_s *d, union arg *argv, int argc);
   size_t keyhash;
   struct dlgfunc *next, **prev;
};

union arg {
   __str s;
   int   i;
};

typedef struct dlgparsestate_s
{
   __prop nextCode {call: NextCode(this)}

   tokbuf_t  tb;
   dlgdef_t *def;
} dlgparsestate_t;

GDCC_HashMap_Decl(strtable_t,  char const *, struct strent)
GDCC_HashMap_Decl(functable_t, char const *, struct dlgfunc)

// Static Objects ------------------------------------------------------------|

static strtable_t  stbl;
static functable_t ftbl;

// Static Functions ----------------------------------------------------------|

#define strtable_t_GetKey(o) ((o)->key)
#define strtable_t_GetNext(o) (&(o)->next)
#define strtable_t_GetPrev(o) (&(o)->prev)
#define strtable_t_HashKey(k) (CStrHash(k))
#define strtable_t_HashObj(o) ((o)->keyhash)
#define strtable_t_KeyCmp(l, r) (strcmp(l, r))
GDCC_HashMap_Defn(strtable_t, char const *, struct strent)

#define functable_t_GetKey(o) ((o)->name)
#define functable_t_GetNext(o) (&(o)->next)
#define functable_t_GetPrev(o) (&(o)->prev)
#define functable_t_HashKey(k) (CStrHash(k))
#define functable_t_HashObj(o) ((o)->keyhash)
#define functable_t_KeyCmp(l, r) (strcmp(l, r))
GDCC_HashMap_Defn(functable_t, char const *, struct dlgfunc)

//
// StrName
//
static int StrName(char const *s)
{
   ifauto(struct strent *, e, stbl.find(s))
      return e->name;
   else
      return STR_NULL;
}

//
// NextCode
//
static int *NextCode(dlgparsestate_t *d)
{
   Vec_GrowN(d->def->code, 1, 32);
   return &Vec_Next(d->def->code);
}

#define GenCode_Reg(code) \
        if(argv[0].s == "a") *d->nextCode() = DCD_##code##_A; \
   else if(argv[0].s == "b") *d->nextCode() = DCD_##code##_B; \
   else if(argv[0].s == "c") *d->nextCode() = DCD_##code##_C; \
   else if(argv[0].s == "d") *d->nextCode() = DCD_##code##_D; \
   else

//
// GenCode_Trace
//
static void GenCode_Trace(dlgparsestate_t *d, union arg *argv, int argc)
{
   GenCode_Reg(TRACE)
   {
      *d->nextCode() = DCD_TRACE_S;
      *d->nextCode() = argv[0].i;
   }
}

//
// GenCode_Push
//
static void GenCode_Push(dlgparsestate_t *d, union arg *argv, int argc)
{
   GenCode_Reg(PUSH)
   {
      *d->nextCode() = DCD_PUSH_I;
      *d->nextCode() = strtoi_str(argv[0].s, null, 0);
   }
}

//
// GenCode_Pop
//
static void GenCode_Pop(dlgparsestate_t *d, union arg *argv, int argc)
{
   GenCode_Reg(POP)
   {
      token_t *tok = d->tb.reget();
      LogOri(tok, "GenCode_Pop: invalid argument");
   }
}

#define GenCode_Arith(code) \
   static void GenCode_Arith_##code(dlgparsestate_t *d, union arg *argv, int argc) \
   { \
      GenCode_Reg(code) \
      { \
         *d->nextCode() = DCD_##code##_I; \
         *d->nextCode() = strtoi_str(argv[0].s, null, 0); \
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

//
// GenCode_Generic
//
static void GenCode_Generic(dlgparsestate_t *d, union arg *argv, int argc)
{
   for(int i = 0; i < argc; i++)
      *d->nextCode() = argv[i].i;
}

//
// GetCode_Cond
//
// Parses and generates code for a conditional statement.
//
static void GetCode_Cond(dlgparsestate_t *d)
{
   int code = DCD_NOP;
   token_t *tok = d->tb.get();

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
         switch(StrName(tok->textV)) {
         case STR_Marine:    *d->nextCode() = pcl_marine;    break;
         case STR_CyberMage: *d->nextCode() = pcl_cybermage; break;
         case STR_Informant: *d->nextCode() = pcl_informant; break;
         default: LogOri(tok, "GetCode_Cond: invalid playerclass type"); return;
         }
      }
      else
         *d->nextCode() = (int)StrParam("%.*s", tok->textC, tok->textV);
   }
   else
      LogOri(tok, "GetCode_Cond: invalid token in conditional statement");

   // Generate statement.
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

//
// GetCode_Option
//
// Parses and generates code for an option statement.
//
static void GetCode_Option(dlgparsestate_t *d)
{
   token_t *tok = d->tb.get();

   // Generate code.
   int ptr = 0;
   if(tok->type == tok_identi || tok->type == tok_string)
   {
      *d->nextCode() = DCD_PUTOPT;
      ptr = d->def->codeC;
      d->nextCode();
      *d->nextCode() = (int)StrParam("%.*s", tok->textC, tok->textV);
   }
   else
      LogOri(tok, "GetCode_Option: invalid option parameter");

   // Generate statement.
   GetStatement(d);

   // Set the pointer in the generated code to be after the statement.
   if(ptr) d->def->codeV[ptr] = d->def->codeC;
}

//
// GetCode_Exec
//
// Parses and generates code for an exec statement.
//
static void GetCode_Exec(dlgparsestate_t *d)
{
   *d->nextCode() = DCD_TRMWAIT;
   GetStatement(d);
}

//
// GetCode_Generic
//
// Parses and generates code for a generic statement.
//
static void GetCode_Generic(dlgparsestate_t *d)
{
   token_t *tok = d->tb.reget();
   LogDebug(log_dlg, "call: %s", tok->textV);

   // Get the function to generate.
   struct dlgfunc const *func = ftbl.find(tok->textV);
   if(!func) {
      LogOri(tok, "GetCode_Generic: invalid function in dialogue code");
      return;
   }

   // Get arguments.
   union arg argv[8] = {};
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

      switch(func->args[argc])
      {
      case 'I': argv[argc++].i = strtoi(tok->textV, null, 0); break;
      case 'S': argv[argc++].s = StrParam("%s", tok->textV);  break;
      }

      LogDebug(log_dlg, "arg %i: %s", argc, tok->textV);

      if(!d->tb.drop(tok_comma) || d->tb.drop(tok_semico))
         break;
   }

   // Fill in unfinished arguments.
   while(func->args[argc])
   {
      switch(func->args[argc])
      {
      case 'I': argv[argc++].i = 0;                break;
      case 'S': argv[argc++].s = "";               break;
      case 'L': argv[argc++].i = func->lit[lit++]; break;
      }

      LogDebug(log_dlg, "arg %i emptied", argc);
   }

   func->genCode(d, argv, argc);
}

//
// GetCode_Text
//
static void GetCode_Text(dlgparsestate_t *d, token_t *tok, int code)
{
   *d->nextCode() = code;
   *d->nextCode() = (int)StrParam("%.*s", tok->textC, tok->textV);
}

//
// GetCode_Line
//
// Parse and generate a line of code.
//
static void GetCode_Line(dlgparsestate_t *d)
{
   token_t *tok = d->tb.get();

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

//
// GetBlock
//
// Parse and generate a block statement.
//
static void GetBlock(dlgparsestate_t *d)
{
   while(!d->tb.drop(tok_bracec) && !d->tb.drop(tok_eof))
      GetStatement(d);
}

//
// GetConcatBlock
//
// Parse and generate a concat block statement.
//
static void GetConcatBlock(dlgparsestate_t *d)
{
   *d->nextCode() = DCD_CONCAT;
   while(!d->tb.drop(tok_at2) && !d->tb.drop(tok_eof))
      GetStatement(d);
   *d->nextCode() = DCD_CONCATEND;
}

//
// GetStatement
//
// Parse and generate a statement.
//
static void GetStatement(dlgparsestate_t *d)
{
        if(d->tb.drop(tok_braceo)) GetBlock(d);
   else if(d->tb.drop(tok_at2))    GetConcatBlock(d);
   else                            GetCode_Line(d);
}

//
// SetupDialogue
//
static void SetupDialogue(dlgparsestate_t *d, int num)
{
   dlgdef_t *last = d->def;

   d->def = Salloc(dlgdef_t);
   d->def->num = num;

   if(!last) dlgdefs    = d->def;
   else      last->next = d->def;

   LogDebug(log_dlg, "set up dialogue %i", num);
}

//
// GetDecl_Dialogue
//
static void GetDecl_Dialogue(dlgparsestate_t *d)
{
   token_t *tok = d->tb.get();

   if(tok->type == tok_number) {
      SetupDialogue(d, strtoi(tok->textV, null, 0));
      LogDebug(log_dlg, "\n---\ndialogue %i (%i)\n---",
         d->def->num, d->def->codeC);
   } else {
      LogOri(tok, "GetDecl_Dialogue: invalid dialogue number token");
   }
}

//
// GetDecl_Terminal
//
static void GetDecl_Terminal(dlgparsestate_t *d)
{
   token_t *tok = d->tb.get();

   if(tok->type == tok_number) {
      SetupDialogue(d, -strtoi(tok->textV, null, 0));
      LogDebug(log_dlg, "\n---\nterminal %i (%i)\n---",
         -d->def->num, d->def->codeC);
   } else {
      LogOri(tok, "GetDecl_Terminal: invalid terminal number token");
   }
}

//
// SetupPage
//
static void SetupPage(dlgparsestate_t *d, int num)
{
   d->def->pages[num] = d->def->codeC;

   LogDebug(log_dlg, "--- page %i (%i)", num, d->def->codeC);
}

//
// GetDecl_Page
//
static void GetDecl_Page(dlgparsestate_t *d)
{
   token_t *tok = d->tb.get();

   if(tok->type == tok_number)
      SetupPage(d, strtoi(tok->textV, null, 0));
   else
      LogOri(tok, "GetDecl_Page: invalid page number token");

   GetStatement(d);

   *d->nextCode() = DCD_DLGWAIT;
   *d->nextCode() = DCD_DIE;
}

//
// GetDecl_TrmPage
//
static void GetDecl_TrmPage(dlgparsestate_t *d, int num)
{
   SetupPage(d, num);
   GetStatement(d);
   *d->nextCode() = DCD_TRMWAIT;
   *d->nextCode() = DCD_DIE;
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_GSInit_Dialogue
//
// Loads all string indices into the global stbl, and all function
// prototypes into the global ftbl.
//
void Lith_GSInit_Dialogue(void)
{
   #pragma GDCC STRENT_LITERAL OFF

   static struct dlgfunc funcs[] = {
      {"nop", "L", DCD_NOP},

      {"push", "S", .genCode = GenCode_Push},
      {"pop",  "S", .genCode = GenCode_Pop},

      {"add", "S", .genCode = GenCode_Arith_ADD},
      {"sub", "S", .genCode = GenCode_Arith_SUB},
      {"mul", "S", .genCode = GenCode_Arith_MUL},
      {"div", "S", .genCode = GenCode_Arith_DIV},
      {"mod", "S", .genCode = GenCode_Arith_MOD},
      {"ior", "S", .genCode = GenCode_Arith_IOR},
      {"and", "S", .genCode = GenCode_Arith_AND},
      {"xor", "S", .genCode = GenCode_Arith_XOR},
      {"lsh", "S", .genCode = GenCode_Arith_LSH},
      {"rsh", "S", .genCode = GenCode_Arith_RSH},

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
      strs[i].keyhash = CStrHash(strs[i].key);
      strs[i].name = i;
      stbl.insert(&strs[i]);
   }

   functable_t_ctor(&ftbl, countof(funcs), 1);
   for(int i = 0; i < countof(funcs); i++) {
      funcs[i].keyhash = CStrHash(funcs[i].name);
      if(!funcs[i].genCode) funcs[i].genCode = GenCode_Generic;
      ftbl.insert(&funcs[i]);
   }
}

//
// Lith_LoadMapDialogue
//
void Lith_LoadMapDialogue(void)
{
   // Free any previous dialogue definitions.
   if(dlgdefs)
   {
      for(dlgdef_t *def = dlgdefs; def;) {
         dlgdef_t *next = def->next;
         Vec_Clear(def->code);
         Dalloc(def);
         def = next;
      }

      Dalloc(dlgdefs);
      dlgdefs = null;
   }

   __str script = LanguageNull("LITH_DLG_SCRIPT_%tS", PRINTNAME_LEVEL);
   if(!script) return;

   dlgparsestate_t d = {{.bbeg = 4, .bend = 10,
      .fp = __fmemopen_str(script, ACS_StrLen(script), c"r")}};

   d.tb.ctor();

   for(token_t *tok; (tok = d.tb.get())->type != tok_eof;)
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
      static __str const dcdnames[] = {
         #define DCD(name) #name,
         #include "lith_dialogue.h"
      };

      for(dlgdef_t *def = dlgdefs; def; def = def->next) {
         Log("Dumping code for script %i...", def->num);
         for(int i = 0; i < def->codeC; i++)
            Log("%i (%S)", def->codeV[i], def->codeV[i] < countof(dcdnames)
               ? dcdnames[def->codeV[i]] : "<none>");
      }

      Log("Done.");
   }
}

// EOF

