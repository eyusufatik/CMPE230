HEADERS = vector.h makers.h matchers.h utils.h vars.h expressions.h

default: matlang2c

program.o: matlang2c.c $(HEADERS)
	gcc -c matlang2c.c -o matlang2c.o -lm

program: matlang2c.o
	gcc matlang2c.o -o matlang2c -lm

clean:
	-rm -f matlang2c.o