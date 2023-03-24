main: main.o guest.o cleaner.o
	g++ guest.o cleaner.o main.o -lpthread -o app

main.o : main.cpp main.hpp
	g++ -c main.cpp

guest.o : guest.cpp 
	g++ -c guest.cpp

cleaner.o : cleaner.cpp 
	g++ -c cleaner.cpp	

clean: 
	rm -rf *.o