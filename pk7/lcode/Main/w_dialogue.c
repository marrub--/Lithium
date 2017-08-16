#include "lith_common.h"
#include "lith_world.h"
#include "lith_player.h"
#include "lith_dialogue.h"

#include <Lth.h>

#define LogTok(s, tok) \
   Log(s " (type %i string %s)", tok->type, tok->str ? tok->str : c"no string")


//----------------------------------------------------------------------------
// Static Functions
//

static int *NextCode(struct dlgparsestate_s *d);
static void GetStatement(struct dlgparsestate_s *d);


//----------------------------------------------------------------------------
// Types
//

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
   property nextCode {call: NextCode(this)}

   Lth_TokenStream *tkstr;

   dlgdef_t *def;
   int codeptr;
   int codesiz;
} dlgparsestate_t;

GDCC_HashMap_Decl(strtable_t,  char const *, struct strent)
GDCC_HashMap_Decl(functable_t, char const *, struct dlgfunc)


//----------------------------------------------------------------------------
// Static Objects
//

static strtable_t  stbl;
static functable_t ftbl;


//----------------------------------------------------------------------------
// Static Functions
//

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
// NextCode
//
static int *NextCode(dlgparsestate_t *d)
{
   if(d->codeptr + 1 > d->codesiz)
   {
      d->def->code = realloc(d->def->code, sizeof(int) * (d->codesiz += 128));

      LogDebug(log_dlg, "reallocating code to size %i", d->codesiz);
   }

   return d->def->code + d->codeptr++;
}

//
// GetCode_Cond
//
// Parses and generates code for a conditional statement.
//
static void GetCode_Cond(dlgparsestate_t *d)
{
   Lth_Token const *tok = d->tkstr->bump();
   int code = DCD_NOP;

   // Get the code to generate.
   if(tok->type == Lth_TOK_Identi)
   {
      switch(stbl.find(tok->str)->name)
      {
      case STR_item:  code = DCD_JNITEM;  break;
      case STR_class: code = DCD_JNCLASS; break;
      default: Log("GetCode_Cond: invalid conditional"); return;
      }
   }

   // Generate code.
   int *ptr = null;
   tok = d->tkstr->bump();
   if(tok->type == Lth_TOK_Identi || tok->type == Lth_TOK_String)
   {
      *d->nextCode() = code;
      ptr = d->nextCode();

      if(code == DCD_JNCLASS)
      {
         switch(stbl.find(tok->str)->name)
         {
         case STR_Marine:    *d->nextCode() = pclass_marine;     break;
         case STR_CyberMage: *d->nextCode() = pclass_cybermage;  break;
         case STR_Informant: *d->nextCode() = pclass_informant;  break;
         default: Log("GetCode_Cond: invalid playerclass type"); return;
         }
      }
      else
         *d->nextCode() = (int)Lth_strentdup(tok->str);
   }

   // Generate statement.
   GetStatement(d);

   // Set the pointer in the generated code to be after the statement.
   if(ptr) *ptr = d->codeptr;
}

//
// GetCode_Option
//
// Parses and generates code for an option statement.
//
static void GetCode_Option(dlgparsestate_t *d)
{
   Lth_Token const *tok = d->tkstr->bump();

   // Generate code.
   int *ptr = null;
   if(tok->type == Lth_TOK_String || tok->type == Lth_TOK_Identi)
   {
      *d->nextCode() = DCD_PUTOPT;
      ptr = d->nextCode();
      *d->nextCode() = (int)Lth_strentdup(tok->str);
   }

   // Generate statement.
   GetStatement(d);

   // Set the pointer in the generated code to be after the statement.
   if(ptr) *ptr = d->codeptr;
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
static void GetCode_Generic(dlgparsestate_t *d, Lth_Token const *tok)
{
   LogDebug(log_dlg, "call: %s", tok->str);

   // Get the function to generate.
   struct dlgfunc const *func = ftbl.find(tok->str);
   if(!func)
   {
      Log("GetCode_Generic: invalid function in dialogue code");
      return;
   }

   // Generate code.
   *d->nextCode() = func->code;

   // Get arguments.
   int args[8] = {}, i = 0, l = 0;

   while(func->args[i])
   {
      if(func->args[i] == 'L')
      {
         args[i++] = func->lit[l++];
         continue;
      }

      if(!(tok = d->tkstr->bump())->str)
      {
         LogTok("GetCode_Generic: invalid token in argument list", tok);
         return;
      }

      switch(func->args[i])
      {
      case 'I': args[i++] = strtoi(tok->str, null, 0);    break;
      case 'S': args[i++] = (int)Lth_strentdup(tok->str); break;
      }

      LogDebug(log_dlg, "arg %i: %s", i, tok->str);

      if(!d->tkstr->drop(Lth_TOK_Comma) || d->tkstr->drop(Lth_TOK_Semico))
         break;
   }

   // Fill in unfinished arguments.
   for(; func->args[i];)
   {
      switch(func->args[i])
      {
      case 'I': args[i++] = 0;              break;
      case 'S': args[i++] = (int)"";        break;
      case 'L': args[i++] = func->lit[l++]; break;
      }

      LogDebug(log_dlg, "arg %i emptied", i, tok->str);
   }

   // Generate arguments.
   for(int a = 0; a < i; a++)
      *d->nextCode() = args[a];
}

//
// GetCode_Text
//
static void GetCode_Text(dlgparsestate_t *d)
{
   int ch;

   *d->nextCode() = DCD_ADDTEXT;

   ACS_BeginPrint();
   while((ch = fgetc(d->tkstr->fp)) != '\n') ACS_PrintChar(ch);
   *d->nextCode() = (int)ACS_EndStrParam();
}

//
// GetCode_Line
//
// Parse and generate a line of code.
//
static void GetCode_Line(dlgparsestate_t *d)
{
   Lth_Token const *tok = d->tkstr->bump();

   switch(tok->type)
   {
   case Lth_TOK_Identi:
      switch(stbl.find(tok->str)->name)
      {
      case STR_if:     GetCode_Cond(d);         break;
      case STR_option: GetCode_Option(d);       break;
      case STR_exec:   GetCode_Exec(d);         break;
      default:         GetCode_Generic(d, tok); break;
      }
      break;
   case Lth_TOK_GT: GetCode_Text(d); break;
   case Lth_TOK_Semico: break;
   case Lth_TOK_EOF: break;
   default: LogTok("GetCode_Line: invalid token in line", tok);
   }
}

//
// GetBlock
//
// Parse and generate a block statement.
//
static void GetBlock(dlgparsestate_t *d)
{
   d->tkstr->drop(Lth_TOK_BraceO);

   while(!d->tkstr->drop(Lth_TOK_BraceC) && !d->tkstr->drop(Lth_TOK_EOF))
      GetCode_Line(d);
}

//
// GetStatement
//
// Parse and generate a statement.
//
static void GetStatement(dlgparsestate_t *d)
{
   if(d->tkstr->peek()->type == Lth_TOK_BraceO)
      GetBlock(d);
   else
      GetCode_Line(d);
}

//
// SetupDialogue
//
static void SetupDialogue(dlgparsestate_t *d, int num)
{
   dlgdef_t *last = d->def;

   d->def = malloc(sizeof(dlgdef_t));

   if(!last) dlgdefs    = d->def;
   else      last->next = d->def;

   d->def->num  = num;
   d->def->next = null;
}

//
// GetDecl_Dialogue
//
static void GetDecl_Dialogue(dlgparsestate_t *d)
{
   Lth_Token const *tok = d->tkstr->bump();
   if(tok->type == Lth_TOK_Number)
   {
      SetupDialogue(d, strtoi(tok->str, null, 0));

      LogDebug(log_dlg, "\n---\ndialogue %i (%i)\n---", d->def->num, d->codeptr);
   }
}

//
// GetDecl_Terminal
//
static void GetDecl_Terminal(dlgparsestate_t *d)
{
   Lth_Token const *tok = d->tkstr->bump();
   if(tok->type == Lth_TOK_Number)
   {
      SetupDialogue(d, -strtoi(tok->str, null, 0));

      LogDebug(log_dlg, "\n---\nterminal %i (%i)\n---", -d->def->num, d->codeptr);
   }
}

//
// SetupPage
//
static void SetupPage(dlgparsestate_t *d, int num)
{
   d->def->pages[num] = d->codeptr;

   LogDebug(log_dlg, "--- page %i (%i)", num, d->codeptr);
}

//
// GetDecl_Page
//
static void GetDecl_Page(dlgparsestate_t *d)
{
   Lth_Token const *tok = d->tkstr->bump();

   if(tok->type == Lth_TOK_Number)
      SetupPage(d, strtoi(tok->str, null, 0));

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


//----------------------------------------------------------------------------
// Extern Functions
//

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

      {"text",     "S",  DCD_SETTEXT},
      {"local",    "S",  DCD_SETTEXTLOCAL},
      {"addtext",  "S",  DCD_ADDTEXT},
      {"addlocal", "S",  DCD_ADDTEXTLOCAL},
      {"setstr",   "IS", DCD_SETSTRING},
      {"name",     "LS", DCD_SETSTRING, DSTR_NAME},
      {"icon",     "LS", DCD_SETSTRING, DSTR_ICON},
      {"remote",   "LS", DCD_SETSTRING, DSTR_REMOTE},

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
      for(dlgdef_t *def = dlgdefs; def;)
      {
         dlgdef_t *next = def->next;
         free(def->code);
         free(def);
         def = next;
      }

      free(dlgdefs);
      dlgdefs = null;
   }

   // Get the script string.
   __str script_str = LanguageNull("LITH_DLG_SCRIPT_%tS", PRINTNAME_LEVEL);
   if(script_str)
   {
      // Open it as a token stream.
      char *script = Lth_strdup_str(script_str);
      dlgparsestate_t d = {Lth_TokenStreamOpen(script, strlen(script))};

      // Parse the file recursively.
      for(;;)
      {
         Lth_Token const *tok = d.tkstr->bump();

         switch(tok->type)
         {
         case Lth_TOK_Identi:
            switch(stbl.find(tok->str)->name)
            {
            case STR_dialogue:   GetDecl_Dialogue(&d); break;
            case STR_page:       GetDecl_Page    (&d); break;
            case STR_terminal:   GetDecl_Terminal(&d); break;
            case STR_failure:    GetDecl_TrmPage (&d, DTRMPAGE_FAILURE);    break;
            case STR_finished:   GetDecl_TrmPage (&d, DTRMPAGE_FINISHED);   break;
            case STR_unfinished: GetDecl_TrmPage (&d, DTRMPAGE_UNFINISHED); break;
            default: Log("Lith_LoadMapDialogue: invalid toplevel identifier %s", tok->str);
            }
            break;
         default: LogTok("Lith_LoadMapDialogue: invalid toplevel token", tok);
         case Lth_TOK_EOF:
            goto done;
         }
      }

   done:
      Lth_TokenStreamClose(d.tkstr);
      free(script);
   }
}

// EOF

