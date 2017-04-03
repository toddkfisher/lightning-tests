im : intmath.leg intmath.o
	leg intmath.leg > im.c
	gcc -o im im.c intmath.o

intmath.o: intmath.h intmath.c
	gcc -c intmath.c
