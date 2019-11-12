%{
// $Id: parser.y,v 1.22 2019-04-23 14:07:52-07 - - $
// Dummy parser for scanner project.

#include <cassert>

#include "lyutils.h"
#include "astree.h"

%}

%debug
%defines
%define parse.error verbose
%token-table
%verbose

%destructor { destroy ($$); } <>
%printer { assert (yyoutput == stderr); astree::dump (cerr, $$); } <>

%initial-action {
   parser::root = new astree (TOK_ROOT, {0, 0, 0}, "<<ROOT>>");
}

%token TOK_VOID TOK_INT TOK_STRING TOK_TYPE_ID
%token TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_NULLPTR TOK_ARRAY TOK_ARROW TOK_ALLOC TOK_PTR
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE TOK_NOT
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON
%token TOK_ROOT TOK_BLOCK TOK_CALL TOK_PARAM TOK_FUNCTION
%token TOK_INDEX

%precedence NO_ELSE
%precedence TOK_ELSE
%right '='
%left  TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left  '+' '-'
%left  '*' '/' '%'
%precedence TOK_POS TOK_NEG
%precedence TOK_ARROW '['

%start program

%%

program     : program structdef                                            { $$ = $1->adopt($2); }
            | program function                                             { $$ = $1->adopt($2); }
            | program statement                                            { $$ = $1->adopt($2); }
            | program error '}'                                            { $$ = $1; destroy($2, $3); }
            | program error ';'                                            { $$ = $1; destroy($2, $3); }
            | %empty                                                       { $$ = parser::root; }
            ;
structdef   : TOK_STRUCT TOK_IDENT '{' structbody '}' ';'                  { $$ = $1->adopt($2, $4); destroy($3, $5, $6); } 
            ;
structbody  : type TOK_IDENT ';'                                           { $$ = $1->adopt($2); destroy($3); }
            | structbody type TOK_IDENT ';'                                { $$ = $1->buddy_up($2->adopt($3)); destroy($4); }
            ;
function    : type TOK_IDENT '(' parameters ')' block                      { $$ = parser::make_function($1, $2, $3, $4, $6); destroy($5); }
            ;
parameters  : type TOK_IDENT                                               { $$ = $1->adopt($2); }
            | parameters ',' type TOK_IDENT                                { $$ = $1->buddy_up($3->adopt($4)); destroy($2); }
            ;
type        : plaintype                                                    { $$ = $1; }
            | TOK_VOID                                                     { $$ = $1; }
            | TOK_ARRAY '<' plaintype '>'                                  { $$ = $1->adopt($3); destroy($2, $4); }
            ;
plaintype   : TOK_INT                                                      { $$ = $1; }
            | TOK_STRING                                                   { $$ = $1; }
            | TOK_PTR '<' TOK_STRUCT '>' TOK_IDENT                         { $$ = $1->adopt($5); destroy($2, $3, $4); }
            ;
block       : '{' statement '}'                                            { $$ = $1->adopt_sym(TOK_BLOCK, $2); destroy($3); }
            | ';'                                                          { $$ = nullptr; destroy($1); }
            ;
statement   : vardecl                                                      { $$ = $1; }
            | block                                                        { $$ = $1; }
            | while                                                        { $$ = $1; }
            | ifelse                                                       { $$ = $1; }
            | return                                                       { $$ = $1; }
            | expr ';'                                                     { $$ = $1; destroy ($2); }
            ;
vardecl     : type TOK_IDENT '=' expr ';'                                  { $$ = parser::make_type_id($1, $2, $4); destroy($3, $5); }
            | type TOK_IDENT ';'                                           { $$ = parser::make_type_id($1, $2); destroy($3); }
            ;
while       : TOK_WHILE '(' expr ')' statement                             { $$ = $1->adopt($3, $5); destroy($2, $4); }
            ;
ifelse      : TOK_IF '(' expr ')' statement TOK_ELSE statement             { $$ = $1->adopt($3, $5, $7); destroy($2, $4, $6); }
            | TOK_IF '(' expr ')' statement %prec NO_ELSE                  { $$ = $1->adopt($3, $5); destroy($2, $4); }
            ;
return      : TOK_RETURN expr ';'                                          { $$ = $1->adopt($2); destroy($3); }
            | TOK_RETURN ';'                                               { $$ = $1; destroy($2); }
            ;
expr        : expr '+' expr                                                { $$ = $2->adopt($1, $3); }
            | expr '-' expr                                                { $$ = $2->adopt($1, $3); }
            | expr '/' expr                                                { $$ = $2->adopt($1, $3); }
            | expr '*' expr                                                { $$ = $2->adopt($1, $3); }
            | expr '%' expr                                                { $$ = $2->adopt($1, $3); }
            | expr '=' expr                                                { $$ = $2->adopt($1, $3); }
            | expr TOK_EQ expr                                             { $$ = $2->adopt($1, $3); }
            | expr TOK_NE expr                                             { $$ = $2->adopt($1, $3); }
            | expr TOK_LT expr                                             { $$ = $2->adopt($1, $3); }
            | expr TOK_LE expr                                             { $$ = $2->adopt($1, $3); }
            | expr TOK_GT expr                                             { $$ = $2->adopt($1, $3); }
            | expr TOK_GE expr                                             { $$ = $2->adopt($1, $3); }
            | '+' expr %prec TOK_POS                                       { $$ = $1->adopt($2); }
            | '-' expr %prec TOK_NEG                                       { $$ = $1->adopt($2); }
            | allocator                                                    { $$ = $1; }
            | call                                                         { $$ = $1; }
            | '(' expr ')'                                                 { $$ = $2; destroy($1, $3); }
            | variable                                                     { $$ = $1; }
            | constant                                                     { $$ = $1; }
            ;
exprs       : expr                                                         { $$ = $1; }
            | exprs ',' expr                                               { $$ = $1->buddy_up($3); destroy($2); }
            ;
allocator   : TOK_ALLOC '<' TOK_STRING '>' '(' expr ')'                    { $$ = $1->adopt($3, $6); destroy($2, $4, $5, $7); }
            | TOK_ALLOC '<' TOK_STRUCT TOK_IDENT '>' '(' ')'               { $$ = $1->adopt($4); destroy($2, $3, $5, $6, $7); }
            | TOK_ALLOC '<' TOK_ARRAY '<' plaintype '>' '>' '(' expr ')'   { $$ = $1->adopt($3->adopt($5), $9); destroy($2, $4, $6, $7, $8, $10); }
            ;
call        : TOK_IDENT '(' exprs ')'                                      { $$ = $2->adopt_sym(TOK_CALL, $1, $3); destroy($4); }
            | TOK_IDENT '(' ')'                                            { $$ = $2->adopt_sym(TOK_CALL, $1); destroy($3); }
            ;
variable    : TOK_IDENT                                                    { $$ = $1; }
            | expr '[' expr ']' %prec TOK_INDEX                            { $$ = $2->adopt_sym(TOK_INDEX, $1, $3); destroy($4); }
            | expr TOK_ARROW TOK_IDENT                                     { $$ = $2->adopt($1, $3); }
            ;
constant    : TOK_INTCON                                                   { $$ = $1; }
            | TOK_CHARCON                                                  { $$ = $1; }
            | TOK_STRINGCON                                                { $$ = $1; }
            | TOK_NULLPTR                                                  { $$ = $1; }
            ;
%%

const char * parser::get_tname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}

astree* parser::make_function(astree* type, astree* id, astree* paren,
      astree* params, astree* block) {
   astree* function = new astree(TOK_FUNCTION, type->loc,
         astree::NOINFO);
   function->adopt(type->adopt(id), paren->adopt_sym(TOK_PARAM, params));
   return function->adopt(block);
}

astree* parser::make_type_id(astree* type, astree* id, astree* expr) {
   astree* type_id = new astree(TOK_TYPE_ID, type->loc, astree::NOINFO);
   return type_id->adopt(type, id, expr);
}

bool is_defined_token (int symbol) {
   return YYTRANSLATE (symbol) > YYUNDEFTOK;
}

