CC = gcc
CCOPTS = -Wall -c -g -ggdb
LINKOPTS = -Wall -g -ggdb -pthread

EXEC=scheduler
OBJECTS=scheduler.o sched_impl.o list.o dummy_impl.o smp4_tests.o testrunner.o

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(LINKOPTS) -o $@ $^

#%.o:%.c
#	$(CC) $(CCOPTS) -o $@ $^

scheduler.o: scheduler.c scheduler.h sched_impl.h
	$(CC) $(CCOPTS) -o $@ scheduler.c

sched_impl.o: sched_impl.c scheduler.h sched_impl.h
	$(CC) $(CCOPTS) -o $@ sched_impl.c

list.o: list.c list.h
	$(CC) $(CCOPTS) -o $@ list.c

dummy_impl.o: dummy_impl.c scheduler.h
	$(CC) $(CCOPTS) -o $@ dummy_impl.c

smp4_tests.o: smp4_tests.c list.h scheduler.h testrunner.h
	$(CC) $(CCOPTS) -o $@ smp4_tests.c

testrunner.o: testrunner.c testrunner.h
	$(CC) $(CCOPTS) -o $@ testrunner.c

test: scheduler
	- ./scheduler -test -f10 fifo
	- ./scheduler -test -f10 rr
	- killall scheduler

clean:
	- $(RM) $(EXEC)
	- $(RM) $(OBJECTS)
	- $(RM) *~
	- $(RM) core.*
	- $(RM) smp4.in smp4.out smp4.err
