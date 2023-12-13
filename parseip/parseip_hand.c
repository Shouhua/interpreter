/*
Convert an IPv4 address in the format of null-terminated C string into a 32-bit integer.
For example, given an IP address “172.168.5.1”, the output should be a 32-bit integer
with “172” as the highest order 8 bit, 168 as the second highest order 8 bit, 5 as the
second lowest order 8 bit, and 1 as the lowest order 8 bit. That is,

"172.168.5.1" => 2896692481


Requirements:

1. You can only iterate the string once.

2. You should handle spaces correctly: a string with spaces between a digit and a dot is
a valid input; while a string with spaces between two digits is not.
"172[Space].[Space]168.5.1" is a valid input. Should process the output normally.
"1[Space]72.168.5.1" is not a valid input. Should report an error.

3. Please provide unit tests
*/

/* RULE
ip_list
	: ip { printf("result: %u\n", ip)}
	| ip_list ip
ip
	: num num num num { $$ = $1 << 24 || $2 << 16 || $3 << 8 || $4 }
num
	: NUM. { $$ = $1 }
	| NUM\n { $$ = $1 }
*/
#include "parseip_hand.h"

#define STEP  \
	source++; \
	pos++

void remove_space()
{
	while (isspace(*source))
	{
		STEP;
		continue;
	}
}

token_t lex()
{
	token_t token;
	token.value = 0;

	remove_space();
	if (*source == '\n')
	{
		STEP;
		token.type = TK_NEWLINE;
	}
	else if (*source == '.')
	{
		STEP;
		token.type = TK_DOT;
	}
	else if (isdigit(*source))
	{
		token.type = TK_NUMBER;
		token.value = *source - '0';
		STEP;

		while (isdigit(*source))
		{
			token.value = token.value * 10 + (*source - '0');
			STEP;
		}
	}
	else
	{
		printf("Invalid character: %c at %d\n", *source, pos);
		exit(EXIT_FAILURE);
	}
	return token;
}

int parse_number()
{
	token_t token;
	int flag;
	int type;

	token = lex();
	if (token.type != TK_NUMBER)
	{
		flag = -1;
		goto fail;
	}
	type = lex().type;
	if (type != TK_DOT && type != TK_NEWLINE)
	{
		flag = -2;
		goto fail;
	}
	return token.value;
fail:
	printf("Invalid ip format: ");
	switch (flag)
	{
	case -1:
		printf("%d expect number\n", token.value);
		break;
	case -2:
		printf("%d expect dot\n", token.value);
		break;
	default:
		break;
	}
	exit(EXIT_FAILURE);
}

unsigned int parse_ip()
{
	unsigned int result = 0;
	int num, i;

	for (i = 0; i < 4; i++)
	{
		num = parse_number();
		result |= (num & 0xff) << ((3 - i) * 8);
	}
	return result;
}

int main(int argc, char *argv[])
{
	char buf[1024];
	unsigned int result;

	if (argc > 2)
	{
		printf("Usage: ./parseip [test]");
		exit(EXIT_FAILURE);
	}

	// test
	if (argc == 2 && !strcmp(argv[1], "test"))
	{
		int num;
		source = " 172  .  168.5.1\n";
		pos = 0;
		num = parse_number();
		assert(num == 172);
		num = parse_number();
		assert(num == 168);
		num = parse_number();
		assert(num == 5);
		num = parse_number();
		assert(num == 1);
		printf("SUCCESS...\n");
		return 0;
	}

	while (fgets(buf, 1024, stdin))
	{
		source = buf;
		pos = 0;
		result = parse_ip();
		printf("result: %u\n", result);
	}
}
