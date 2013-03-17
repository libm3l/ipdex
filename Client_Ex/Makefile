PATHL  = $(shell pwd)

all:

	ln -sf ../../libm3l/Source/libm3l.* .
	gcc -c -g ACK.c
	gcc -g -o Client1.out Client1.c ACK.o -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL)


clean:
	rm *.o *.out
