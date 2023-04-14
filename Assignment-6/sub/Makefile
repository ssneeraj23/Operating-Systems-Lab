run: library mergesort

library: goodmalloc.o
	ar rcs -o libgoodmalloc.a goodmalloc.o

goodmalloc.o: goodmalloc.cpp goodmalloc.hpp
	g++ -Wall -Werror -std=c++11 -c  goodmalloc.cpp

mergesort: mergesort.cpp
	g++ -Wall mergesort.cpp -o mergesort -L. -lgoodmalloc
clean:
	rm -f goodmalloc.o libgoodmalloc.a mergesort