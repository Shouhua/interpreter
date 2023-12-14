#ifndef _MASTACHE_H_
#define _MASTACHE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include "cJSON.h"

typedef enum
{
    Root = 1 << 0,

    Literal = 1 << 1,
    Variable = 1 << 2,
    SectionStart = 1 << 3,
    SectionEnd = 1 << 4,

    Newline = 1 << 5,
    Whitespace = 1 << 6,
    End = 1 << 7
} NType;

struct node
{
    NType type;
    char *value;        // 代表节点的实际内容
    struct line *linfo; // 节点对应行信息

    struct node *spair; // section [start<->end]配对，解析时候填充
    int slevel;         // 当前属于section嵌套第几层
    struct node *sprev; // 上一个section start节点

    struct node *next;
};

struct line
{
    int type;          // 当前行所有node type的集合
    long num;          // 第几行
    int is_root;       // 是否是链表头
    int is_standalone; // 是否是独立行

    struct node *children;         // node节点链表
    int child_num;                 // node节点个数
    struct node *last_child;       // 快速定位到最后一行，用于reander section中
    struct node *first_real_child; // reander section中快速定位到渲染的node, 非standalone行为NULL

    struct line *next;
};

struct stack
{
    cJSON *context;
    struct stack *prev;
    struct stack *next;
};

// 全局记录当前secion start状态, 间接形成section链表, 主要用于node中pair信息填充, sprev
struct section_list
{
    struct node *section; // 当前section start节点
    int level;            // 当前嵌套的secion层数
};

char *source;
char *json;

// 解析全局变量
struct line *root;
struct line *current_line;  // 解析时当前行
struct node *lookahead;     // 解析式当前节点
struct section_list *slist; // 解析时填充和使用

// 运行全局变量
struct stack *root_stack;
struct stack *current_stack;
// 运行时使用到的当前行和节点
struct line *rline;
struct node *rnode;

long row;
long column;

#define iseof(c) (c == '\0')
#define isnewline(c) (c == '\n')
#define iswhitespace(c) (c == '\t' || c == ' ')
#define isalpha(c) (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
#define isdigit(c) (c >= '0' && c <= '9')

#define accept(c) (*source == c ? (next(), 1) : 0)
#define expect(c)   \
    if (!accept(c)) \
        fprintf(stderr, "expected '%c'", c);

void render_section(struct node *start);
void render_node(struct node *n);

#endif