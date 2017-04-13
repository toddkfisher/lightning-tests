im : im.o intmath.o
	gcc -o im im.o intmath.o -llightning

im.c : intmath.leg
	leg intmath.leg > im.c

im.o : im.c
	gcc -c im.c

intmath.o: intmath.h intmath.c
	gcc -c intmath.c
