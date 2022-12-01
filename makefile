.PHONY: clean compile

all: bin/functions.o bin/main.o
	g++ -Wall -Wextra bin/functions.o bin/main.o -o bin/start
	bin/./start base.txt

bin/functions.o: src/functions.cpp includes/functions.h
	g++ -Wall -Wextra -Iincludes/ -c src/functions.cpp -o bin/functions.o

bin/main.o: src/main.cpp
	g++ -Wall -Wextra -Iincludes/ -c src/main.cpp -o bin/main.o

clean:
	rm bin/main.o bin/functions.o bin/start

compile: bin/functions.o bin/main.o
	g++ -Wall -Wextra bin/functions.o bin/main.o -o bin/start