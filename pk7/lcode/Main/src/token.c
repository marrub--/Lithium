//-----------------------------------------------------------------------------
//
// Copyright © 2016-2017 Project Golan, David Hill
//
// Code by David Hill has been relicensed with consent.
//
// See "LICENSE.lithos3" for more information.
//
//-----------------------------------------------------------------------------
//
// Tokenizer.
//
//-----------------------------------------------------------------------------

#define _GNU_SOURCE

#include "Lth.h"

#include <ctype.h>
#include <string.h>
#include <ACS_ZDoom.h>


// Extern Functions ----------------------------------------------------------|

//
// Lth_TokenGet
//
void Lth_TokenGet(FILE *fp, Lth_Token *out)
{
   char ch = fgetc(fp);

   out->type = Lth_TOK_EOF;

   if(out->str)
      free(out->str), out->str = NULL;

   switch(ch)
   {
   case '=':  Lth_TokenSet(out, Equals); return;
   case ';':  Lth_TokenSet(out, Semico); return;

   case '[':  Lth_TokenSet(out, BrackO); return;
   case ']':  Lth_TokenSet(out, BrackC); return;

   case '\n': Lth_TokenSet(out, LnEnd ); return;

   case '-':
      if(isdigit(Lth_fpeekc(fp))) break;
      Lth_TokenSet(out, Minus);
      return;

   case '.':
      if(isdigit(Lth_fpeekc(fp))) break;
      Lth_TokenSet(out, Dot);
      return;
   }

   if(isspace(ch))
   {
      ACS_BeginPrint();

      do ACS_PrintChar(ch);
      while((ch = fgetc(fp)) != EOF && (isspace(ch) && ch != '\n'));
      if(ch != EOF) ungetc(ch, fp);

      out->str  = Lth_strdup_str(ACS_EndStrParam());
      out->type = Lth_TOK_Space;

      return;
   }

   if(ch == '"' || ch == '\'')
   {
      char beg = ch;

      ACS_BeginPrint();

      while((ch = fgetc(fp)) != beg && ch != EOF)
      {
         if(ch != '\\')
            ACS_PrintChar(ch);
         else if((ch = fgetc(fp)) == beg)
            ACS_PrintChar(ch);
         else
         {
            ungetc(ch, fp);
            ACS_PrintChar('\\');
         }
      }

      out->str  = Lth_strdup_str(ACS_EndStrParam());
      out->type = Lth_TOK_String;

      return;
   }

   if(isdigit(ch) || ch == '.' || ch == '-')
   {
      ACS_BeginPrint();

      do ACS_PrintChar(ch);
      while((ch = fgetc(fp)) != EOF && (isalnum(ch) || ch == '.'));
      if(ch != EOF) ungetc(ch, fp);

      out->str  = Lth_strdup_str(ACS_EndStrParam());
      out->type = Lth_TOK_Number;

      return;
   }

   if(Lth_isidenti(ch))
   {
      ACS_BeginPrint();

      do ACS_PrintChar(ch);
      while((ch = fgetc(fp)) != EOF && Lth_isidenti(ch));
      if(ch != EOF) ungetc(ch, fp);

      out->str  = Lth_strdup_str(ACS_EndStrParam());
      out->type = Lth_TOK_Identi;

      return;
   }

   if(ch == EOF)
   {
      Lth_TokenSet(out, EOF);
      return;
   }

   char chrseq[2] = {ch, '\0'};
   out->str  = Lth_strdup(chrseq);
   out->type = Lth_TOK_ChrSeq;
}

//
// Lth_TokenName
//
char const *Lth_TokenName(Lth_TokenType type)
{
   static char const *names[] = {
      "None",
#define Lth_X(name) #name,
#include "Lth_token.h"
      "EOF",
      "Max"
   };
   return names[type];
}

// EOF
