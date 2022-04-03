HEADERS = headers/vector.h headers/makers.h headers/matchers.h headers/utils.h headers/vars.h headers/expressions.h

<<<<<<< HEAD

matlang2c: matlang2c.o $(HEADERS)
=======
matlang2c: matlang2c.o
>>>>>>> 6c600508a4e7328e24dad23ec9a65c0a361c3826
	gcc matlang2c.o -o matlang2c -lm

matlang2c.o: matlang2c.c $(HEADERS)
	gcc -c -I headers/ matlang2c.c -o matlang2c.o -lm

clean:
	-rm -f matlang2c.o