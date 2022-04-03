HEADERS = vector.h makers.h matchers.h utils.h vars.h expressions.h

matlang2c: matlang2c.o
	gcc matlang2c.o -o matlang2c -lm

matlang2c.o: matlang2c.c $(HEADERS)
	gcc -c -I headers/ matlang2c.c -o matlang2c.o -lm

clean:
	-rm -f matlang2c.o