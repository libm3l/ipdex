PATHL  = $(shell pwd)

all:

	ln -sf ../../../libm3l/Source/libm3l.* .
	ln -sf ../../Source/liblsipdx.so* .
	ln -sf ../../Source/lsipdx.h .

	gcc -g -o Client1_Pressure_Sender.out Client1_Pressure_Sender.c -L$(PATHL)  -llsipdx -Wl,-rpath=$(PATHL) -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL) -lpthread -lm
	gcc -g -o Client1_Pressure.out Client1_Pressure.c -L$(PATHL)  -llsipdx -Wl,-rpath=$(PATHL)  -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL) -lpthread -lm


clean:
	rm *.o *.out
