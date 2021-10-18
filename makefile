lib/libmyOwnthread.so: obj/thread.o
	gcc -o lib/libmyOwnthread.so obj/thread.o -shared
# ./libmyOwnthread.so: obj/thread.o
# 	gcc -o ./libmyOwnthread.so obj/thread.o -shared
obj/thread.o: src/thread.c
	gcc -Wall -fpic -c src/thread.c -o obj/thread.o
# test: tests/test.c
# 	gcc test.c -o test -L./ -lmyOwnthread
tests: tests/matmul.c tests/locks.c lib/libmyOwnthread.so
	gcc tests/matmul.c -o bin/matmul -L./lib -lmyOwnthread -Wl,-rpath=./lib
	gcc tests/locks.c -o bin/locks -L./lib -lmyOwnthread -Wl,-rpath=./lib
clean:
	rm -f bin/* obj/* lib/*
