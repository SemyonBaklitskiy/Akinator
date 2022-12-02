.PHONY: clean clean_def compile definition_mode

all: bin/akinator_functions.o bin/main.o
	g++ -Wall -Wextra bin/akinator_functions.o bin/main.o -o bin/start
	bin/./start base.txt

bin/akinator_functions.o: src/akinator_functions.cpp includes/akinator_functions.h
	g++ -Wall -Wextra -Iincludes/ -c src/akinator_functions.cpp -o bin/akinator_functions.o

bin/main.o: src/main.cpp
	g++ -Wall -Wextra -Iincludes/ -c src/main.cpp -o bin/main.o

bin/stack.o: src/stack_functions.cpp includes/stack_functions.h 
	g++ -Wall -Wextra -Iincludes/ -c src/stack_functions.cpp -o bin/stack.o

clean:
	rm bin/main.o bin/akinator_functions.o bin/start

compile: bin/akinator_functions.o bin/main.o bin/stack.o
	g++ -Wall -Wextra bin/akinator_functions.o bin/main.o bin/stack.o -o bin/start

bin/def_akinator_functions.o: src/akinator_functions.cpp includes/akinator_functions.h
	g++ -Wall -Wextra -Iincludes/ -D DEFINITION -c src/akinator_functions.cpp -o bin/def_akinator_functions.o

bin/def_main.o: src/main.cpp
	g++ -Wall -Wextra -Iincludes/ -D DEFINITION -c src/main.cpp -o bin/def_main.o

definition_mode: bin/def_main.o bin/def_akinator_functions.o bin/stack.o
	g++ -Wall -Wextra bin/def_main.o bin/def_akinator_functions.o bin/stack.o -o bin/def

clean_def:
	rm bin/def_main.o bin/def_akinator_functions.o bin/stack.o bin/def