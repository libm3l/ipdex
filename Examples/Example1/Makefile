PATHL  = $(shell pwd)

all:

	ln -sf ../../../libm3l/Source/data_util/libm3l.* .
	ln -sf ../../Source/liblsipdx.so* .
	ln -sf ../../Source/lsipdx.h .

	gcc -g -o Client1_Density.out Client1_Density.c -L$(PATHL)  -llsipdx -Wl,-rpath=$(PATHL)  -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL)  -lpthread -lm
	gcc -g -o Client1_Density_Sender.out Client1_Density_Sender.c  -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL) -L$(PATHL)  -llsipdx -Wl,-rpath=$(PATHL)  -lpthread -lm
	gcc -g -o Client1_Pressure.out Client1_Pressure.c  -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL) -L$(PATHL)  -llsipdx -Wl,-rpath=$(PATHL)  -lpthread -lm
	gcc -g -o Client1_Pressure_Sender.out Client1_Pressure_Sender.c -L$(PATHL) -lm3l -Wl,-rpath=$(PATHL) -L$(PATHL)  -llsipdx -Wl,-rpath=$(PATHL)  -lpthread -lm
	gcc -g -o Client1_Temperature.out Client1_Temperature.c  -L$(PATHL)   -lm3l -Wl,-rpath=$(PATHL) -L$(PATHL)  -llsipdx -Wl,-rpath=$(PATHL)  -lpthread -lm
	gcc -g -o Client1_Temperature_Sender.out Client1_Temperature_Sender.c -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL) -L$(PATHL)  -llsipdx -Wl,-rpath=$(PATHL)  -lpthread -lm


clean:
	rm *.o *.out
