CFLAGS := -Wall -Wextra -pedantic -g

.PHONY: mastache run clean ast gprof valgrind gdb

# make run ARGS="test.txt test.json"
run: mastache
	@./mastache ${ARGS}

ast: CFLAGS += -DTEST
ast: mastache
	@gcc ${CFLAGS} -o mastache mastache.c cJSON.c
	@./mastache ${ARGS}
	

mastache: mastache.c
	@gcc ${CFLAGS} -o mastache mastache.c cJSON.c


clean:
	@rm -vf mastache gmon.out

gprof: CFLAGS += -pg
gprof: mastache
	@gcc ${CFLAGS} -o mastache mastache.c cJSON.c
	@time ./mastache ${ARGS}
	@gprof mastache gmon.out

valgrind:
	valgrind ./mastache ${ARGS}

gdb: mastache
	gdb --args mastache test.txt test.json

# debug coredump
# gdb mastache ${COREDUMP_DIR}/$(ls ${COREDUMP_DIR} -t | head -n1)