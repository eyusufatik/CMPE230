HEADERS = vector.h makers.h matchers.h utils.h vars.h expressions.h

default: matlang2c

program.o: matlang2c.c $(HEADERS)
	gcc -c matlang2c.c -lm -o matlang2c.o

program: matlang2c.o
	gcc matlang2c.o -o matlang2c

clean:
	-rm -f matlang2c.o