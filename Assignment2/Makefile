CC=clang
CFLAGS=-I.
DEPS =

TestAssn2: test_assign2_1.o dberror.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o
	cc -o TestAssn2 test_assign2_1.o dberror.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o
test_assign2_1.o: test_assign2_1.c test_helper.h dberror.h storage_mgr.h buffer_mgr.h buffer_mgr_stat.h
	cc -c test_assign2_1.c
dberror.o: dberror.c dberror.h
	cc -c dberror.c
storage_mgr.o: storage_mgr.c storage_mgr.h dberror.h
	cc -c storage_mgr.c
buffer_mgr.o: buffer_mgr.c buffer_mgr.h dberror.h
	cc -c buffer_mgr.c
buffer_mgr_stat.o: buffer_mgr_stat.c buffer_mgr_stat.h dberror.h
	cc -c buffer_mgr_stat.c

clean:	
	rm -f *.o TestAssn2
