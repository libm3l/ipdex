PATHL  = $(shell pwd)

all:

	ln -sf ../../libm3l/Source/libm3l.* .
	gcc -c -g ACK.c
	gcc -g -o Client1.out Client1.c ACK.o -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL)
	gcc -g -o Client1_Density.out Client1_Density.c ACK.o -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL)
	gcc -g -o Client1_Pressure.out Client1_Pressure.c ACK.o -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL)
	gcc -g -o Client1_Pressure_Sender.out Client1_Pressure_Sender.c ACK.o -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL)
	gcc -g -o Client1_Temperature.out Client1_Temperature.c ACK.o -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL)


clean:
	rm *.o *.out
