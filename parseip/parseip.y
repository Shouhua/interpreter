/*
参考官方示例 /usr/share/doc/bison/examples/c/lexcalc，test.txt文档用于测试，使用如下命令运行(172.168.5.1 <=> 2896692481):
make
./parseip < <(cat test.txt) # 注意test.txt文档最后需要换行结尾
./parseip <<< $'172.168.5.1\n172.168.1.1'
*/
// To avoid warnings from Bison about predictable, legitimate shift/reduce conflicts, you can use the %expect n declaration
%expect 0
// after YYSTYPE YYLTYPE definition
%code provides
{
#define YY_DECL \
    yytoken_kind_t yylex (YYSTYPE* yylval, YYLTYPE *yylloc, int *nerrs)
    YY_DECL;

int yyerror(const YYLTYPE *loc, int *nerrs, const char *msg, ...);

#define YYDEBUG 1
#define YYERROR_VERBOSE 1
}
%code top
{
#include <stdio.h>
#include <stdarg.h>
}
/*
// Include the header in the implementation rather than duplicating it.
%define api.header.include {"parse.h"}

// Don't share global variables between the scanner and the parser.
%define api.pure full

// To avoid name clashes (e.g., with C's EOF) prefix token definitions
// with TOK_ (e.g., TOK_EOF).
%define api.token.prefix {TOK_}

// %token and %type use genuine types (e.g., "%token <int>").  Let
// %bison define YYSTYPE as a union of all these types.
%define api.value.type union

// Generate detailed error messages.
%define parse.error detailed

// with locations.
%locations

// Enable debug traces (see yydebug in main).
%define parse.trace
*/

/* %define api.header.include {"parseip.tab.h"} */

// reentrant，yyloc，yylval等不会是全局变量，需要在lex()中声明，不然在flex文件中找不到这些变量
%define api.pure full
// Enable debug traces (see yydebug in main).
/* %define parse.trace */
// Generate detailed error messages.
%define parse.error detailed

// 防止token定义与其他变量冲突
%define api.token.prefix {TOK_}

// yyparse,yylex传参需要包含的参数
%param {int *nerrs}

/* %define api.value.type union */
%union {
    unsigned int num;
}

// 语法位置信息，也可以在命令行中打开
/* %define api.location.type { localtion_t } */
%locations

%token<num> NUMBER
%token DOT EOL

%type<num> ip num
%type ip_list

%%
ip_list: 
    %empty
    | ip_list ip { 
        if($2 > 0)
            printf(">>%u\n", $2); 
    }

ip: EOL { yyerrok; }
    | num DOT num DOT num DOT num EOL
    { 
        if($1>=256 || $3>=256 || $5>=256 || $7>=256) {
            yyerror(&@$, nerrs, "number is equal or greater than 256");
            YYERROR;
            // fprintf(stderr, "the second part %u at %d.%d-%d.%d is equal or greater than 256\n", 
            //     $3, @3.first_line, @3.first_column, @3.last_line, @3.last_column);
            //     return -1;
        } else 
            $$ = ($1 << 24) | ($3 << 16) | ($5 << 8) | $7; 
            // printf("%d, %d, %d, %d\n", $1, $3, $5, $7);
    }
    | error EOL { yyerrok; }
num: NUMBER
%%

int main()
{
    int nerrs = 0;
    yyparse(&nerrs);
    return !!nerrs;
}

int yyerror(const YYLTYPE *loc, int *nerrs, const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);

    YYLOCATION_PRINT (stderr, loc);
    fprintf (stderr, ": ");
    vfprintf(stderr, msg, ap);
    va_end(ap);
    /* fprintf (stderr, ": %s\n", msg); */
    ++*nerrs;
    return 1;
}