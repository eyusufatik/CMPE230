make && make clean

./matlang2c examples/ex$1.mat
gcc examples/ex$1.c -o examples/output -lm
./examples/output > deneme.txt

diff -w deneme.txt expected_outputs/output$1.txt
