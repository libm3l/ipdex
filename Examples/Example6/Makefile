PATHL  = $(shell pwd)

all:

	ln -sf ../../../libm3l/Source/data_util/libm3l.* .
	ln -sf ../../Source/liblsipdx.so* .
	ln -sf ../../Source/lsipdx.h .

	gcc -g -o S1.out S1.c -L$(PATHL)  -llsipdx -Wl,-rpath=$(PATHL)  -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL)  -lpthread -lm
	gcc -g -o S2.out S2.c -L$(PATHL)  -llsipdx -Wl,-rpath=$(PATHL)  -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL)  -lpthread -lm
	gcc -g -o S3.out S3.c -L$(PATHL)  -llsipdx -Wl,-rpath=$(PATHL)  -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL)  -lpthread -lm
	gcc -g -o S4.out S4.c -L$(PATHL)  -llsipdx -Wl,-rpath=$(PATHL)  -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL)  -lpthread -lm
	gcc -g -o S5.out S5.c -L$(PATHL)  -llsipdx -Wl,-rpath=$(PATHL)  -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL)  -lpthread -lm


clean:
	rm *.o *.out
