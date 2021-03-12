lib/libmyOwnthread.so: obj/thread.o
	gcc -o lib/libmyOwnthread.so obj/thread.o -shared
# ./libmyOwnthread.so: obj/thread.o
# 	gcc -o ./libmyOwnthread.so obj/thread.o -shared
obj/thread.o: src/thread.c
	gcc -Wall -fpic -c src/thread.c -o obj/thread.o
# test: tests/test.c
# 	gcc test.c -o test -L./ -lmyOwnthread
test: tests/test.c lib/libmyOwnthread.so
	gcc tests/test.c -o bin/test -L./lib -lmyOwnthread -Wl,-rpath=./lib