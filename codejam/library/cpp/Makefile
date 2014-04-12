# Coding contest library (C++)
# Alex Roper
# alex@aroper.net
#
# Note: Relies on Boost (http://www.boost.org)
#
# All code available under the terms of the GPLv3, and in particular you are
# welcome to use this library in your own contest coding. See
# https://github.com/calmofthestorm/toys/codejam/library for the latest version
# or to submit issues/pull requests.
#
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
