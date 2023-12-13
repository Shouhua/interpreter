CFLAGS := -Wall -Wextra -pedantic -g -pg

.PHONY: mastache run clean ast gprof valgrind

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
	@rm -vf mastache

gprof: mastache
	@time ./mastache ${ARGS}
	@gprof mastache gmon.out

valgrind:
	valgrind ./mastache ${ARGS}

gdb: mastache
	gdb --args mastache test.txt test.json

#  gdb mastache ${COREDUMP_DIR}/$(ls ${COREDUMP_DIR} -t | head -n1)