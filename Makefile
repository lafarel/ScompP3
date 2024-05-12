SRCS=utils.cpp poisson.cpp DistributedMatrix.cpp SubMatrix.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

default: run

all: poisson.out test.out

runtests: test.out
	mpiexec -n 2 ./test.out

run: poisson.out
	mpiexec -n 4 ./poisson.out

%.o: %.cpp
	mpicxx -std=c++11 -g -o $@ -c $<

test.out: test.cpp $(OBJS)
	mpicxx -std=c++11 -g -Wall test.cpp $(OBJS) -o test.out

poisson.out: main.cpp $(OBJS)
	mpicxx -std=c++11 -g -Wall main.cpp $(OBJS) -o poisson.out

clean:
	$(RM) *.o *.out
