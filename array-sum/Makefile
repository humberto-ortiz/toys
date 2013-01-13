CPP=g++
FLAGS=-Wall -ansi -pedantic --std=c++11

default: test

summer_test: Summer.o tests.o
	$(CPP) Summer.o tests.o -o summer_test

Summer.o: Summer.cc
	$(CPP) -c $(FLAGS) Summer.cc

tests.o: tests.cc
	$(CPP) -c $(FLAGS) tests.cc

test: summer_test
	./summer_test

clean:
	rm -f tests.o Summer.o summer_test
