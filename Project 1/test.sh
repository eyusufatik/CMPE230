make && make clean

./matlang2c examples/ex$1.mat
gcc output.c -o output -lm
./output > deneme.txt

diff -w deneme.txt expected_outputs/output$1.txt
