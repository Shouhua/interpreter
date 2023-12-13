SHELL = /usr/bin/bash
EXE = parseip

# flex规则匹配调试
# FLEX_FLAG = -d
FLEX_FLAG = 
# --header产生头文件，-t tracing，-v 输出ouput文件，包括解析后的规则
BISON_FLAG = --header -t -v
C_LIBS = -lfl
C_FLAGS = -Wall -Wextra -pedantic
CC = gcc

all: ${EXE} parseip_hand

${EXE}: parseip.l parseip.y  
	@bison ${BISON_FLAG} parseip.y
# flex -d parseip.l 
	@flex ${FLEX_FLAG} parseip.l
	@${CC} ${C_FLAGS} -o $@ parseip.tab.c lex.yy.c ${C_LIBS}

parseip_hand: parseip_hand.c
	@${CC} ${C_FLAGS} -o $@ $<

.PHONY: test run clean
test: ${EXE} test.txt
	@echo "命令行测试"
	@./parseip <<< $$'172	 .	 168.5.1\n172.168.5.1'
	@echo "测试文件测试"
	@./parseip < <(cat test.txt)

run: ${EXE}
	@./parseip
clean:
	@rm -vf lex.yy.c parseip.tab.* parseip parseip.output parseip_hand