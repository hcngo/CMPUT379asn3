all: saucer
helpers.o: helpers.c helpers.h saucer.h
	gcc -pthread -Wall -c helpers.c
saucer: saucer.o helpers.o
	gcc -pthread -Wall -o saucer saucer.o helpers.o -lm -lcurses
saucer.o: saucer.c saucer.h helpers.h
	gcc -pthread -Wall -c saucer.c
clean:
	-rm -f *.o saucer core 
