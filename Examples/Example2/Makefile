PATHL  = $(shell pwd)

all:

	ln -sf ../../libm3l/Source/libm3l.* .
	ln -sf ../Source/libipdex.so* .
	ln -sf ../Source/ipdex.h .

	gcc -g -o Client1_Pressure_Sender.out Client1_Pressure_Sender.c -L$(PATHL)  -lipdex -Wl,-rpath=$(PATHL) -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL) -lpthread
	gcc -g -o Client1_Pressure.out Client1_Pressure.c -L$(PATHL)  -lipdex -Wl,-rpath=$(PATHL)  -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL) -lpthread


clean:
	rm *.o *.out
