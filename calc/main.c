#include <stdio.h>
#include <stdlib.h>

typedef enum
{
	TK_ADD,
	TK_SUB,
	TK_MUL,
	TK_DIV,
	TK_NEWLINE,
	TK_NUMBER
} TK_TYPE;

typedef struct
{
	TK_TYPE type;
	double value;
} token;

#define isdigit(c) (c >= '0' && c <= '9')
#define isempty(c) (c == '\x0' || c == '\x20')
#define isnewline(c) (c == '\x0a')

static char *source;
token lookahead;

#define yy_accept(x) (*source == x ? (yylex(), 1) : 0)
#define yy_expect(x)   \
	if (!yy_accept(x)) \
		printf("expectd'%c'\n", x);

void init_lex(const char *source)
{
	source = source;
	lookahead = yylex();
}

token yylex()
{
	token t;
	while (isempty(*source++))
	{
		continue;
	}
	if (isnewline(*source))
	{
		source++;
		t.type = TK_NEWLINE;
		return t;
	}
	if (isdigit(*source))
	{
		double v = *source++ - '0';
		while (isdigit(*source))
		{
			v = v * 10 + (*source - '0');
			source++;
		}
		t.type = TK_NUMBER;
		t.value = v;
		return t;
	}
	if (*source == '+')
	{
		t.type = TK_ADD;
		source++;
		return t;
	}
	if (*source == '-')
	{
		t.type = TK_SUB;
		source++;
		return t;
	}
	if (*source == '*')
	{
		t.type = TK_MUL;
		source++;
		return t;
	}
	if (*source == '/')
	{
		t.type = TK_DIV;
		source++;
		return t;
	}
	printf("[ERR] Lex: invalid char: %c\n", *source);
	exit(-1);
}

double parse_primary_expr()
{
	// token t;
	// t = yylex();
	// if (t.type != TK_NUMBER)
	// {
	// 	printf("[ERR] Parser: token type is not number\n");
	// 	exit(-1);
	// }
	// return t.value;
}

double parse_term()
{
	double left, right;
	token t;

	left = parse_primary_expr();
	t = yylex();
	if (t.type == TK_MUL)
	{
		right = parse_term();
		return left * right;
	}
	if (t.type == TK_DIV)
	{
		right = parse_term();
		return left / right;
	}
	source--;
	return left;
}

double parse_expr()
{
	double left, right;
	token token;

	left = parse_term();
	token = yylex();
	if (token.type == TK_ADD)
	{
		right = parse_expr();
		return left + right;
	}
	if (token.type == TK_SUB)
	{
		right = parse_expr();
		return left - right;
	}
	source--;
	return left;
}

double parse_line()
{
	return parse_expr();
}

int main()
{
	char buf[1024];
	double value;

	while (fgets(buf, 1024, stdin) != NULL)
	{
		init_lex(buf);
		value = parse_line();
		printf(">>%f\n", value);
	}
	return 0;
}