#include "lith_common.h"
#include "lith_world.h"
#include "lith_player.h"
#include "lith_dialogue.h"

#include <Lth.h>


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
   STR_MAX,
};

typedef struct dlgparsestate_s
{
   property nextCode {call: NextCode(this)}
   
   Lth_TokenStream *tkstr;
   
   dlgdef_t *def;
   int codeptr;
   int codesiz;
} dlgparsestate_t;


//----------------------------------------------------------------------------
// Static Objects
//

static Lth_HashMap dlgstrtable;


//----------------------------------------------------------------------------
// Static Functions
//

//
// NextCode
//
static int *NextCode(dlgparsestate_t *d)
{
   if(d->codeptr + 1 > d->codesiz)
   {
      d->def->code = realloc(d->def->code, sizeof(int) * (d->codesiz += 128));
      
      if(ACS_GetCVar("__lith_debug_on"))
         Log("reallocating code to size %i", d->codesiz);
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
      switch((int)dlgstrtable.findcstr(tok->str))
      {
      case STR_item:  code = DCD_JNITEM;  break;
      case STR_class: code = DCD_JNCLASS; break;
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
         switch((int)dlgstrtable.findcstr(tok->str))
         {
         case STR_Marine:    *d->nextCode() = pclass_marine;    break;
         case STR_CyberMage: *d->nextCode() = pclass_cybermage; break;
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
// GetCode_Generic
//
// Parses and generates code for a generic statement.
//
static void GetCode_Generic(dlgparsestate_t *d, Lth_Token const *tok)
{
   if(ACS_GetCVar("__lith_debug_on"))
      Log("call: %s", tok->str);
   
   // Get the code to generate.
   int code = DCD_NOP;
   switch((int)dlgstrtable.findcstr(tok->str))
   {
   case STR_exit:   code = DCD_DIE;    break;
   case STR_icon:   code = DCD_ICON;   break;
   case STR_name:   code = DCD_NAME;   break;
   case STR_page:   code = DCD_JPAGE;  break;
   case STR_text:   code = DCD_TEXT;   break;
   case STR_remote: code = DCD_REMOTE; break;
   }
   
   // Generate code.
   *d->nextCode() = code;
   
   // Generate arguments.
   while(!d->tkstr->drop(Lth_TOK_Semico))
   {
      switch((tok = d->tkstr->bump())->type)
      {
      case Lth_TOK_String:
      case Lth_TOK_Identi:
         *d->nextCode() = (int)Lth_strentdup(tok->str);
         if(ACS_GetCVar("__lith_debug_on"))
            Log("> %s", tok->str);
         break;
      case Lth_TOK_Number:
         *d->nextCode() = strtoi(tok->str, null, 0);
         if(ACS_GetCVar("__lith_debug_on"))
            Log("> %i", strtoi(tok->str, null, 0));
         break;
      }
   }
}

//
// GetCode_Line
//
// Parse and generate a line of code.
//
static void GetCode_Line(dlgparsestate_t *d)
{
   Lth_Token const *tok = d->tkstr->bump();
   
   if(tok->type == Lth_TOK_Identi)
   {
      switch((int)dlgstrtable.findcstr(tok->str))
      {
      case STR_if:     GetCode_Cond(d);         break;
      case STR_option: GetCode_Option(d);       break;
      default:         GetCode_Generic(d, tok); break;
      }
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
   
   while(!d->tkstr->drop(Lth_TOK_BraceC))
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
// GetDecl_Dialogue
//
static void GetDecl_Dialogue(dlgparsestate_t *d)
{
   d->tkstr->drop(Lth_TOK_Identi);
   
   Lth_Token const *tok = d->tkstr->bump();
   if(tok->type == Lth_TOK_Number)
   {
      dlgdef_t *last = d->def;
      
      d->def = malloc(sizeof(dlgdef_t));
      
      if(!last) dlgdefs    = d->def;
      else      last->next = d->def;
      
      d->def->num  = strtoi(tok->str, null, 0);
      d->def->next = null;
   }
}

//
// GetDecl_Page
//
static void GetDecl_Page(dlgparsestate_t *d)
{
   Lth_Token const *tok = d->tkstr->bump();
   
   if(tok->type == Lth_TOK_Number)
   {
      int num = strtoi(tok->str, null, 0);
      d->def->pages[num] = d->codeptr;
      
      if(ACS_GetCVar("__lith_debug_on"))
         Log("page %i = %i", num, d->codeptr);
   }
   
   GetStatement(d);
   
   *d->nextCode() = DCD_WAIT;
   *d->nextCode() = DCD_DIE;
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_GSInit_DlgStrTable
//
// Loads all string indices into the global dlgstrtable.
//
void Lith_GSInit_DlgStrTable(void)
{
   #pragma GDCC STRENT_LITERAL OFF
   
   dlgstrtable.alloc(STR_MAX);
   
   #define Str(name, str) \
      dlgstrtable.elem.data[STR_##name].keyhash = Lth_Hash_char(str); \
      dlgstrtable.elem.data[STR_##name].value   = (void *)STR_##name;
   #include "lith_dlgstrtable.h"
   
   dlgstrtable.build();
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
            switch((int)dlgstrtable.findcstr(tok->str))
            {
            case STR_dialogue: GetDecl_Dialogue(&d); break;
            case STR_page:     GetDecl_Page(&d);     break;
            }
            break;
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

