// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_world.h"
#include "lith_player.h"
#include "lith_dialogue.h"
#include "lith_tokbuf.h"

#define LogTok(s, tok) \
   Log(s " (%i:\"%s\")", tok->type, tok->textV ? tok->textV : c"<no string>")

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
   int   code;
   int   lit[4];
   size_t keyhash;
   struct dlgfunc *next, **prev;
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

//
// GetCode_Cond
//
// Parses and generates code for a conditional statement.
//
static void GetCode_Cond(dlgparsestate_t *d)
{
   int code = DCD_NOP;
   token_t *tok;

   // Get the code to generate.
   if((tok = d->tb.get())->type == tok_identi)
   {
      switch(StrName(tok->textV)) {
      case STR_item:  code = DCD_JNITEM;  break;
      case STR_class: code = DCD_JNCLASS; break;
      default: Log("GetCode_Cond: invalid conditional"); return;
      }
   }
   else
      LogTok("GetCode_Cond: expected identifier", tok);

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
         case STR_Marine:    *d->nextCode() = pcl_marine;        break;
         case STR_CyberMage: *d->nextCode() = pcl_cybermage;     break;
         case STR_Informant: *d->nextCode() = pcl_informant;     break;
         default: Log("GetCode_Cond: invalid playerclass type"); return;
         }
      }
      else
         *d->nextCode() = (int)StrParam("%.*s", tok->textC, tok->textV);
   }
   else
      Log("GetCode_Cond: invalid token in conditional statement");

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
      Log("GetCode_Option: invalid option parameter");

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
      Log("GetCode_Generic: invalid function in dialogue code");
      return;
   }

   // Generate code.
   *d->nextCode() = func->code;

   // Get arguments.
   int args[8] = {};
   int i = 0, l = 0;

   while(func->args[i])
   {
      if(func->args[i] == 'L') {
         args[i++] = func->lit[l++];
         continue;
      }

      if(!(tok = d->tb.get())->textV) {
         LogTok("GetCode_Generic: invalid token in argument list", tok);
         return;
      }

      switch(func->args[i]) {
      case 'I': args[i++] = strtoi(tok->textV, null, 0); break;
      case 'S': args[i++] = (int)StrParam("%.*s", tok->textC, tok->textV);
         break;
      }

      LogDebug(log_dlg, "arg %i: %s", i, tok->textV);

      if(!d->tb.drop(tok_comma) || d->tb.drop(tok_semico))
         break;
   }

   // Fill in unfinished arguments.
   while(func->args[i])
   {
      switch(func->args[i]) {
      case 'I': args[i++] = 0;              break;
      case 'S': args[i++] = (int)"";        break;
      case 'L': args[i++] = func->lit[l++]; break;
      }

      LogDebug(log_dlg, "arg %i emptied", i);
   }

   // Generate arguments.
   for(int a = 0; a < i; a++)
      *d->nextCode() = args[a];
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
   token_t *tok;

   switch((tok = d->tb.get())->type)
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
      LogTok("GetCode_Line: invalid token in line", tok);
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

   d->def = salloc(dlgdef_t);
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
      Log("GetDecl_Dialogue: invalid dialogue number token");
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
      Log("GetDecl_Terminal: invalid terminal number token");
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
      Log("GetDecl_Page: invalid page number token");

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
      {"die",  "", DCD_DIE},
      {"exit", "", DCD_DIE},

      {"page", "I", DCD_JPAGE},

      {"script",      "IIIII", DCD_SCRIPTI},
      {"scriptnamed", "SIIII", DCD_SCRIPTS},
      {"trace",       "S",     DCD_TRACE},

      {"intralevelteleport", "I", DCD_TELEPORT_INTRALEVEL},
      {"interlevelteleport", "I", DCD_TELEPORT_INTERLEVEL},

      {"text",      "S",  DCD_SETTEXT},
      {"local",     "S",  DCD_SETTEXTLOCAL},
      {"addtext",   "S",  DCD_ADDTEXT},
      {"addlocal",  "S",  DCD_ADDTEXTLOCAL},
      {"setstr",    "IS", DCD_SETSTRING},
      {"name",      "LS", DCD_SETSTRING, DSTR_NAME},
      {"icon",      "LS", DCD_SETSTRING, DSTR_ICON},
      {"remote",    "LS", DCD_SETSTRING, DSTR_REMOTE},
      {"concat",    "",   DCD_CONCAT},
      {"endconcat", "",   DCD_CONCATEND},

      {"dlgwait", "", DCD_DLGWAIT},

      {"logon",  "S", DCD_LOGON},
      {"logoff", "S", DCD_LOGOFF},
      {"info",   "",  DCD_INFO},
      {"pict",   "S", DCD_PICT},
      {"trmwait", "", DCD_TRMWAIT},
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
         free(def);
         def = next;
      }

      free(dlgdefs);
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
         LogTok("Lith_LoadMapDialogue: invalid toplevel token", tok);
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

