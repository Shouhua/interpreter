#include "mastache.h"

void lex_error(const char *fmt, ...)
{
	char msgbuf[256];

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(msgbuf, 256, fmt, ap);
	va_end(ap);

	fprintf(stderr, "[ERROR] [LEX] (%ld, %ld): %s\n", row, column, msgbuf);
	exit(EXIT_FAILURE);
}

void print_error(const char *fmt, ...)
{
	char msgbuf[256];

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(msgbuf, 256, fmt, ap);
	va_end(ap);

	fprintf(stderr, "[ERROR] %s\n", msgbuf);
	exit(EXIT_FAILURE);
}

void print_errno(const char *fmt, ...)
{
	char msgbuf[256];

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(msgbuf, 256, fmt, ap);
	va_end(ap);

	fprintf(stderr, "[ERROR] %s: %s\n", msgbuf, strerror(errno));
	exit(EXIT_FAILURE);
}

struct node *new_node(NType type)
{
	struct node *n = (struct node *)malloc(sizeof(struct node));
	n->type = type;
	n->value = NULL;
	n->linfo = NULL;
	n->spair = NULL;
	n->slevel = 0;
	n->sprev = NULL;
	n->next = NULL;
	return n;
}

void next()
{
	source++;
	column++;
}

struct node *lex_tag()
{
	char *start = source;
	next();
	next();
	while (*source != '}' || *(source + 1) != '}')
	{
		if (iswhitespace(*source) || isnewline(*source) || iseof(*source))
			lex_error("Variable名称不能包含字符'%d'", (int)(*source));
		next();
	}
	next();
	next();

	if ((source - start) == 4)
		lex_error("Variable名称不能为空");

	struct node *n;
	NType t;
	int index = 2;
	char flag = *(start + 2);

	if (flag == '#')
	{
		index++;
		t = SectionStart;
	}
	else if (flag == '/')
	{
		index++;
		t = SectionEnd;
	}
	else
	{
		t = Variable;
	}

	n = new_node(t);
	n->value = strndup(start + index, source - start - index - 2);
	return n;
}

struct node *lex()
{
	NType t;
	char *start, *v;
	struct node *current_node;

	start = source;
	if (iseof(*source))
		return new_node(End);
	if (accept('\n'))
	{
		row++;
		column = 0;
		t = Newline;
		v = strndup(start, source - start);
		goto ok;
	}
	if (iswhitespace(*source))
	{
		while (iswhitespace(*source))
		{
			next();
		}
		t = Whitespace;
		v = strndup(start, source - start);
		goto ok;
	}
	if (*source == '{' && *(source + 1) == '{')
		return lex_tag();

	while (!iseof(*source) && !isnewline(*source) && !iswhitespace(*source) && (*source != '{' || *(source + 1) != '{'))
		next();
	t = Literal;
	v = strndup(start, source - start);
ok:
	current_node = new_node(t);
	current_node->value = v;
	return current_node;
}

struct line *new_line()
{
	struct line *new_line = (struct line *)malloc(sizeof(struct line));
	new_line->type = 0;
	new_line->num = 1;
	new_line->is_root = 0;
	new_line->is_standalone = 1;
	new_line->children = NULL;
	new_line->child_num = 0;
	new_line->last_child = NULL;
	new_line->next = NULL;
	new_line->first_real_child = NULL;
	return new_line;
}

void add_line_child(struct node *child)
{
	current_line->child_num++;
	child->linfo = current_line;

	if (child->type == Literal || child->type == Variable)
		current_line->is_standalone = 0;

	if (child->type == SectionStart)
	{
		slist->level++;
		if (slist->level > 1) // section 嵌套
			child->sprev = slist->section;
		child->slevel = slist->level; // section嵌套链表，0表示当前层
		slist->section = child;
	}

	if (child->type == SectionEnd)
	{
		if (slist->level < 1)
			print_error("没有出现SectionStart就开始SectionEnd");
		if (!slist->section)
			print_error("不知道为什么没有sectionstart了，一般到不了这里");
		child->spair = slist->section;
		slist->section->spair = child;
		slist->level--;
		slist->section = slist->section->sprev; // 指向上一个secion start节点
	}
	if (current_line->children == NULL)
		current_line->children = child;
	else
	{
		struct node *last = current_line->children;
		while (last->next != NULL)
			last = last->next;
		last->next = child;
	}
	current_line->type |= child->type;
	if (!current_line->first_real_child && child->type != Whitespace && child->type && Newline && child->type != End)
		current_line->first_real_child = child;
}

void init_parse()
{
	row = 1;
	column = 0;

	slist = (struct section_list *)malloc(sizeof(struct section_list));
	slist->level = 0;
	slist->section = NULL;
}

void parse()
{
	init_parse();

	root = new_line();
	root->is_root = 1;
	current_line = root;

	while (1)
	{
		long r = row;
		lookahead = lex();
		if (!lookahead || lookahead->type == End)
			break;
		struct line *next_line = new_line();
		next_line->num = r;
		current_line->next = next_line;
		current_line = next_line;

		while (1)
		{
			add_line_child(lookahead);
			if (lookahead->type == Newline || lookahead->type == End)
			{
				current_line->last_child = lookahead; // 填充last child
				// 矫正is_standalone
				if (current_line->is_standalone && !(current_line->type & (SectionStart | SectionEnd)))
					current_line->is_standalone = 0;
				if (!current_line->is_standalone)
					current_line->first_real_child = NULL;
				break;
			}
			lookahead = lex();
		}
	}

	if (slist->level > 0)
	{
		print_error("第%ld没有对应的section end", slist->section->linfo->num);
	}
}

// 初始化当前用于渲染section的上下文环境
void init_stack()
{
	root_stack = (struct stack *)malloc(sizeof(struct stack));
	root_stack->prev = NULL;
	root_stack->next = NULL;
	root_stack->context = cJSON_Parse(json);
	if (root_stack->context == NULL)
	{
		const char *error_ptr = cJSON_GetErrorPtr();
		if (error_ptr != NULL)
		{
			fprintf(stderr, "解析json出现问题: %s\n", error_ptr);
			cJSON_Delete(root_stack->context);
			root_stack->context = NULL;
			exit(EXIT_FAILURE);
		}
	}
	current_stack = root_stack;
	return;
}

void set_context(cJSON *new_context)
{
	struct stack *new_stack = (struct stack *)malloc(sizeof(struct stack));
	new_stack->context = new_context;
	current_stack->next = new_stack;
	new_stack->prev = current_stack;
	current_stack = new_stack;
}

char *format_json(const cJSON *value)
{
	if (cJSON_IsBool(value) || cJSON_IsNumber(value))
		return cJSON_PrintUnformatted(value);
	if (cJSON_IsString(value))
		return value->valuestring;
	if (cJSON_IsNull(value))
		return "";
	if (cJSON_IsObject(value))
		return "[object Object]";

	if (cJSON_IsArray(value))
	{
		char *str;
		unsigned int mem_len = 256;
		if ((str = (char *)malloc(mem_len)) == NULL)
			print_errno("解析array时候malloc问题");
		memset(str, 0, mem_len);

		const cJSON *item = NULL;
		cJSON_ArrayForEach(item, value)
		{
			char *buf = format_json(item);
			if ((strlen(buf) + strlen(str)) > mem_len)
			{
				mem_len = mem_len * 2 + strlen(buf);
				if ((str = realloc(str, mem_len)) == NULL)
					print_errno("解析array时候realloc问题");
				memset(str + strlen(str), 0, mem_len);
			}
			strcat(str, buf);
			strcat(str, ",");
		}
		str[strlen(str) - 1] = '\0';
#ifdef DEBUG
		fprintf(stdout, "[DEBUG] format_json 解析array分配内存大小: %d\n", mem_len);
#endif
		return str;
	}
#ifdef DEBUG
	fprintf(stdout, "[DEBUG] format_json 未知类型的json: %s\n", cJSON_Print(value));
#endif
	return NULL;
}

cJSON *recursive_context_search(const char *key)
{
	cJSON *result = NULL;
	struct stack *buf = current_stack;

	while (buf)
	{
		result = cJSON_GetObjectItemCaseSensitive(buf->context, key);
		if (result)
			return result;
		buf = buf->prev;
	}
	return result;
}

char *lookup(char *key)
{
	if (strlen(key) == 1 && *key == '.')
		return format_json(current_stack->context);

	cJSON *result = NULL;
	char *tok;

	tok = strtok(key, ".");
	result = recursive_context_search(tok);
	if (result == NULL)
	{
		fprintf(stderr, "全局找不到变量: %s\n", tok);
		exit(EXIT_FAILURE);
	}

	while (result != NULL && (tok = strtok(NULL, ".")) != NULL)
	{
		result = cJSON_GetObjectItemCaseSensitive(result, tok);
	}
	if (result == NULL)
		print_error("找不到对应的值key: %s", tok);
	return format_json(result);
}

// 如果buf为\n, 开始执行下一行，知道section_end
#define REANDER_SECTION_NODE                                                                          \
	while (rnode && rnode != end_node)                                                                \
	{                                                                                                 \
		render_node(rnode);                                                                           \
		/*渲染下一行*/                                                                           \
		if (rnode->next == NULL)                                                                      \
		{                                                                                             \
			struct line *next_line = rnode->linfo->next;                                              \
			/* 如果下一行是结束行*/                                                          \
			if (next_line->num == end_node->linfo->num && next_line->is_standalone)                   \
				rnode = end_node;                                                                     \
			else /* 如果下一行不是结束行*/                                                  \
				rnode = next_line->is_standalone ? next_line->first_real_child : next_line->children; \
		}                                                                                             \
		else                                                                                          \
			rnode = rnode->next;                                                                      \
	}

void render_section(struct node *start)
{
	struct node *section_end, *start_node, *end_node;
	int is_standalone;

	is_standalone = start->linfo->is_standalone;
	rnode = start_node = is_standalone ? (start->linfo->next->is_standalone ? start->linfo->next->first_real_child : start->linfo->next->children)
									   : start->next; // standalone跳过SectionStart行
	section_end = start->spair;
	// 大概率不会到这里，编译阶段已经处理
	if (!section_end)
		print_error("Section start没有section end: %s", start_node->value);

	end_node = section_end->linfo->is_standalone ? section_end->linfo->children : section_end;

	// 添加新的context
	// cJSON *new_context = cJSON_GetObjectItemCaseSensitive(current_stack->context, start->value);
	cJSON *new_context = recursive_context_search(start->value);
	// 没有内容, 上下文为null, false, 或者找不到上下文
	if (!new_context || start_node == section_end || cJSON_IsNull(new_context) || cJSON_IsFalse(new_context))
	{
		goto end;
	}
	set_context(new_context);

	if (cJSON_IsArray(new_context))
	{
		cJSON *item = NULL;
		cJSON_ArrayForEach(item, new_context)
		{
			rnode = start_node; // 重置起始点，跳过SectionStart自己
			current_stack->context = item;
			REANDER_SECTION_NODE
		}
	}
	else
	{
		if (new_context && !(cJSON_IsBool(new_context) && cJSON_IsFalse(new_context)) && !cJSON_IsNull(new_context))
		{
			REANDER_SECTION_NODE
		}
	}

	struct stack *d = current_stack;
	current_stack = current_stack->prev;
	free(d);
	d = NULL;

end:
	rline = end_node->linfo;
	rnode = end_node->linfo->is_standalone ? end_node->linfo->last_child : end_node;
}

void render_node(struct node *n)
{
	if (n->type == SectionStart)
	{
		render_section(n);
	}
	else if (n->type == SectionEnd)
	{
	}
	else if (n->type == Variable)
	{
		char *result;
		if ((result = lookup(n->value)) != NULL)
			fprintf(stdout, "%s", result);
	}
	else
	{
		fprintf(stdout, "%s", n->value);
	}
}

void render_line(struct line *l)
{
	if (l->is_standalone)
		rnode = l->first_real_child;
	else
		rnode = l->children;
	while (rnode && rnode->type != End)
	{
		render_node(rnode);
		// 因为有可能standalone或者section start渲染修改了rline和rnode
		if (rnode)
			rnode = rnode->next;
	}
}

void init_run()
{
	rline = root->next;
	rnode = NULL;

	init_stack();
}

void run()
{
	init_run();

	while (rline != NULL)
	{
		render_line(rline);
		// 因为有可能standalone或者section start渲染修改了rline和rnode
		if (rline)
			rline = rline->next;
	}
}

void print_ast()
{
	struct line *l = root->next;
	while (l != NULL)
	{
		int num = 0;
		fprintf(stdout, "LINE %ld, is standalone: %d, children number: %d, last child: %p, first real child: %p\n",
				l->num, l->is_standalone, l->child_num, (void *)(l->last_child), (void *)(l->first_real_child));
		struct node *n = l->children;
		while (n != NULL)
		{
			num++;
			fprintf(stdout, "\ttype: %d, value: %s, next: %p\n", n->type, n->value, (void *)(n->next));
			n = n->next;
		}
		l = l->next;
	}
}

char *read_file(const char *f)
{
	FILE *m;
	long len;
	char *s;

	m = fopen(f, "r");
	if (!m)
	{
		fprintf(stderr, "打开文件(%s)失败: %s\n", f, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fseek(m, 0, SEEK_END) == -1)
	{
		fprintf(stderr, "fseek文件(%s)SEEK_END失败: %s\n", f, strerror(errno));
		exit(EXIT_FAILURE);
	}
	len = ftell(m);
	if (len == -1)
	{
		fprintf(stderr, "ftell文件(%s)失败: %s\n", f, strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (fseek(m, 0, SEEK_SET))
	{
		fprintf(stderr, "fseek文件(%s)SEEK_SET失败: %s\n", f, strerror(errno));
		exit(EXIT_FAILURE);
	}

	s = (char *)malloc(len + 1);
	if (fread(s, len, 1, m) < (unsigned int)len)
	{
		if (ferror(m))
		{
			print_errno("fread发生错误");
		}
	}
	s[len] = '\0';

	fclose(m);

	return s;
}

void init_file(const char *mfile, const char *jfile)
{
	source = read_file(mfile);
	json = read_file(jfile);
}

int main(int argc, char *argv[])
{
	if (argc != 1 && argc != 3)
	{
		fprintf(stderr, "USAGE: ./mastache [mastache_file] [json_file]\n");
		exit(EXIT_FAILURE);
	}
	init_file(argv[1], argv[2]);
	parse();
#ifdef TEST
	print_ast();
#else
	run();
#endif

	if (root_stack && root_stack->context)
	{
		cJSON_Delete(root_stack->context);
		root_stack = NULL;
	}
	// 释放json
	// 释放run
	// 释放parse
	// 释放lex
	// 释放其他
	return 0;
}