CC=gcc
CFLAGS=-lpthread
DEPS=dberror.h storage_mgr.h buffer_mgr_stat.h buffer_mgr.h dt.h test_helper.h expr.h record_mgr.h tables.h 

OBJ1=dberror.o storage_mgr.o
OBJ2=dberror.o storage_mgr.o buffer_mgr_stat.o buffer_mgr.o
OBJ3=dberror.o storage_mgr.o buffer_mgr_stat.o buffer_mgr.o record_mgr.o expr.o rm_serializer.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

all: test_assign1_1 test_assign1_2 test_assign2_1_modified test_assign2_2 test_assign3_1

# assign 1
# test_assign1_1
test_assign1_1:	$(OBJ1)	test_assign1_1.o
	$(CC) -o $@ $^

dberror.o: dberror.c
	$(CC) -c dberror.c

storage_mgr.o: storage_mgr.c
	$(CC) -c storage_mgr.c

test_assign1_1.o: test_assign1_1.c
	$(CC) -c test_assign1_1.c

# test_assign1_2
test_assign1_2:	$(OBJ1)  test_assign1_2.o
	$(CC) -o $@ $^

test_assign1_2.o: test_assign1_2.c
	$(CC) -c test_assign1_2.c


# assign 2
# test_assign2_1_modified
test_assign2_1_modified: $(OBJ2)	test_assign2_1_modified.o
	$(CC) -o $@ $^ $(CFLAGS)

buffer_mgr_stat.o: buffer_mgr_stat.c
	$(CC) -c buffer_mgr_stat.c

buffer_mgr.o: buffer_mgr.c
	$(CC) -c buffer_mgr.c

test_assign2_1_modified.o: test_assign2_1_modified.c
	$(CC) -c test_assign2_1_modified.c

# test_assign2_2
test_assign2_2: $(OBJ2)	test_assign2_2.o
	$(CC) -o $@ $^ $(CFLAGS)

test_assign2_2.o: test_assign2_2.c
	$(CC) -c test_assign2_2.c

# assign 2
# test_assign3_1
test_assign3_1: $(OBJ3)  test_assign3_1.o
	$(CC) -o $@ $^ $(CFLAGS)

expr.o: expr.c
	$(CC) -c expr.c

record_mgr.o: record_mgr.c
	$(CC) -c record_mgr.c

rm_serializer.o: rm_serializer.c
	$(CC) -c rm_serializer.c

test_assign3_1.o: test_assign3_1.c
	$(CC) -c test_assign3_1.c


.PHONY: clean
clean:
	-rm -f test_assign1_1 test_assign1_2 test_assign2_1_modified test_assign2_2 test_assign3_1 *.o *.log

