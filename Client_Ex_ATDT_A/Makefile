PATHL  = $(shell pwd)

all:

	ln -sf ../../libm3l/Source/libm3l.* .
	gcc -c -g ACK.c
	gcc -g -o Client1_Pressure.out Client1_Pressure.c ACK.o client_functions_Prt.o -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL)
	gcc -g -o Client1_Pressure_Sender.out Client1_Pressure_Sender.c ACK.o client_functions_Prt.o -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL)

clean:
	rm *.o *.out
