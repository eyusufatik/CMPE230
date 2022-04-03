HEADERS = vector.h makers.h matchers.h utils.h vars.h expressions.h

default: matlang2c

program: matlang2c.o $(HEADERS)
	gcc -c matlang2c.c -o matlang2c -lm

clean:
	-rm -f matlang2c.o