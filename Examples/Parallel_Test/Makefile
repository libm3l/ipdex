PATHL = /home/jka/Cprograms/Sources/lsipdx/Examples/Parallel_Test
LIBM3LPATH = /home/jka/Cprograms/Sources/libm3l/Source
LSIPDXPATH =  /home/jka/Cprograms/Sources/lsipdx/Source

all:
	gcc -g -o test.out test.c  -L$(PATHL) -lm3l -Wl,-rpath=$(PATHL) -L$(PATHL)  -llsipdx -Wl,-rpath=$(PATHL) -lpthread -lm
	gcc -O -o Client1_FakedSTRIPE.out Client1_FakedSTRIPE.c -L$(LSIPDXPATH)  -llsipdx -Wl,-rpath=$(LSIPDXPATH) -L$(LIBM3LPATH)  -lm3l -Wl,-rpath=$(LIBM3LPATH) -lpthread -lm
	gcc -O -o Client1_FakedSTRIPE_1.out Client1_FakedSTRIPE_1.c -L$(LSIPDXPATH)  -llsipdx -Wl,-rpath=$(LSIPDXPATH) -L$(LIBM3LPATH)  -lm3l -Wl,-rpath=$(LIBM3LPATH) -lpthread -lm
	gcc -O -o Client1_FakedSTRIPE_2.out Client1_FakedSTRIPE_2.c -L$(LSIPDXPATH)  -llsipdx -Wl,-rpath=$(LSIPDXPATH) -L$(LIBM3LPATH)  -lm3l -Wl,-rpath=$(LIBM3LPATH) -lpthread -lm
