#ifndef PARSEIP_H
#define PARSEIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define isdigit(x) (x >= '0' && x <= '9')
#define isspace(x) (x == '\x20' || x == '\x09')

typedef enum
{
    TK_NUMBER,
    TK_DOT,
    TK_NEWLINE
} TK_TYPE;

typedef struct
{
    TK_TYPE type;
    int value;
} token_t;

char *source;
int pos;

void remove_space();
token_t lex();
int parse_number();

#endif