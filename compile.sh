gcc bch.c -c
gcc libbch.c -c
gcc test.c -c
gcc bch.o libbch.o test.o -o test
