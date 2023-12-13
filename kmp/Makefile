SRC := ${wildcard *.c}
EXE := ${patsubst %.c, %, ${SRC}}

all: ${EXE}

%: %.c
	@gcc -Wall -Wextra -pedantic -g -o $@ $<

.PHONY: clean
clean:
	@rm -vf ${EXE}