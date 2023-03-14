.PHONY: all dump clean clean_dump

all: bin/start
	bin/./start base.txt

dump: bin/start_dump
	bin/./start_dump base.txt
	dot -Tpng tree.gv -o tree.png

bin/start: bin/akinator_functions.o bin/main.o bin/stack.o
	g++ -Wall -Wextra bin/akinator_functions.o bin/main.o bin/stack.o -o bin/start

bin/start_dump: bin/akinator_functions_dump.o bin/main_dump.o bin/stack.o
	g++ -Wall -Wextra bin/akinator_functions_dump.o bin/main_dump.o bin/stack.o -o bin/start_dump

bin/akinator_functions.o: src/akinator_functions.cpp includes/akinator_functions.h
	if [ ! -d bin ]; then mkdir bin; fi
	g++ -Wall -Wextra -I includes/ -c src/akinator_functions.cpp -o bin/akinator_functions.o

bin/akinator_functions_dump.o: src/akinator_functions.cpp includes/akinator_functions.h
	if [ ! -d bin ]; then mkdir bin; fi
	g++ -Wall -Wextra -I includes/ -D DUMP -c src/akinator_functions.cpp -o bin/akinator_functions_dump.o

bin/main.o: src/main.cpp
	g++ -Wall -Wextra -I includes/ -c src/main.cpp -o bin/main.o

bin/main_dump.o: src/main.cpp
	g++ -Wall -Wextra -I includes/ -D DUMP -c src/main.cpp -o bin/main_dump.o

bin/stack.o: src/stack_functions.cpp includes/stack_functions.h 
	g++ -Wall -Wextra -I includes/ -c src/stack_functions.cpp -o bin/stack.o

clean: 
	rm -r bin/

clean_dump: clean
	rm tree.gv tree.png

