CXX=g++
RM=rm -f
CPPFLAGS=-std=c++11 -Wall -pedantic -Wextra  -O3
LDFLAGS=-std=c++11 -lpthread
MAKEFLAGS=-j 8

SRCS=
OBJS=$(subst .cc,.o,$(SRCS))

default: $(OBJS) codejam/boilerplate.o boilerplate_nomt
	ar rvs library.a $(OBJS) codejam/boilerplate.o
	ar rvs library_nomt.a $(OBJS) codejam/boilerplate_nomt.o

boilerplate_nomt:
	$(CXX) $(CPPFLAGS) -c codejam/boilerplate.cc -DNOMT -o codejam/boilerplate_nomt.o

clean:
	rm -f $(OBJS) $(subst .cc,,$(TEST_OBJS)) codejam/boilerplate.o codejam/boilerplate_nomt.o library.a library_nomt.a
