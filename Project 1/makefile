HEADERS = headers/vector.h headers/makers.h headers/matchers.h headers/utils.h headers/vars.h headers/expressions.h


matlang2c: matlang2c.o $(HEADERS)
	gcc matlang2c.o -o matlang2c -lm

matlang2c.o: matlang2c.c $(HEADERS)
	gcc -c -I headers/ matlang2c.c -o matlang2c.o -lm

clean:
	-rm -f matlang2c.o