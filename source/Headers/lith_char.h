// Copyright © 2018 Alison Sanderson, all rights reserved.
#define IsGraph(ch) ((ch) >  ' ' && (ch) <= '~')
#define IsPrint(ch) ((ch) >= ' ' && (ch) <= '~')
#define IsPunct(ch) ((ch) >= '!' && (ch) <= '/' || \
                     (ch) >= ':' && (ch) <= '@' || \
                     (ch) >= '[' && (ch) <= '`' || \
                     (ch) >= '{' && (ch) <= '~')
#define IsSpace(ch) ((ch) >= 0x9 && (ch) <= 0xD || \
                     (ch) == ' ')
#define IsBlank(ch) ((ch) == ' ' || (ch) == 0x9)
#define IsDigit(ch) ((ch) >= '0' && (ch) <= '9')
#define IsXDigi(ch) ((ch) >= 'a' && (ch) <= 'f' || \
                     (ch) >= 'A' && (ch) <= 'F' || \
                     IsDigit(ch))
#define IsLower(ch) ((ch) >= 'a' && (ch) <= 'z')
#define IsUpper(ch) ((ch) >= 'A' && (ch) <= 'Z')
#define IsAlpha(ch) (IsLower(ch) || IsUpper(ch))
#define IsAlNum(ch) (IsAlpha(ch) || IsDigit(ch))
#define IsIdent(ch) (IsAlNum(ch) || (ch) == '_')
#define IsNumId(ch) (IsIdent(ch) || (ch) == '.')
#define IsCntrl(ch) (!IsPrint(ch))

// EOF
