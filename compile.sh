gcc bch.c -c
gcc bchlib.c -c
gcc test.c -c
gcc bch.o bchlib.o test.o -o test
