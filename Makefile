all: main

main: main.c db_util.c
	gcc -Wall -pthread -lpthread main.c db_util.c -o main -l sqlite3 -lsqlite3

clean:
	rm -f *.o main
