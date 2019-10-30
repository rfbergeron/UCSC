/* $Id: lexer.l,v 1.4 2019-08-22 17:31:32-07 - - $ */

%{

#include "lyutils.h"

#define YY_USER_ACTION  { lexer::advance(); }
#define YY_FATAL_ERROR(msg) { lexer::fatal_error (msg); }

%}

%option 8bit
%option debug
%option nobackup
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option warn
/*%option verbose*/

LETTER          [A-Za-z_]
DIGIT           [0-9]
MANTISSA        ({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)
EXPONENT        ([Ee][+-]?{DIGIT}+)
NUMBER          ({MANTISSA}{EXPONENT}?)
NOTNUMBER       ({MANTISSA}[Ee][+-]?)
IDENT           ({LETTER}({LETTER}|{DIGIT})*)

%%

"#".*\n         { lexer::include(); lexer::newline(); }
[ \t]+          { }
\n              { lexer::newline(); }

{NUMBER}        { return lexer::token (NUMBER); }
{IDENT}         { return lexer::token (IDENT); }
"="             { return lexer::token ('='); }
"+"             { return lexer::token ('+'); }
"-"             { return lexer::token ('-'); }
"*"             { return lexer::token ('*'); }
"/"             { return lexer::token ('/'); }
"^"             { return lexer::token ('^'); }
"("             { return lexer::token ('('); }
")"             { return lexer::token (')'); }
";"             { return lexer::token (';'); }

{NOTNUMBER}     { return lexer::badtoken (NUMBER); }
.               { lexer::badchar (*yytext); }

%%

