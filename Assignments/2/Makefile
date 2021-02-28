bin/prime: src/pthread.c
	gcc pthread.c -lm -lpthread -o bin/prime
run:
	bin/prime 1000000 10
clean:
	rm bin/prime outputStats/* plots/* -f
