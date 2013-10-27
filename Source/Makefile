#FILES = $(shell find ./ -name "*.c" | sed 's/.\///' )
FILES  = $(shell ls  *.c )
PATHL  = $(shell pwd )
HFILES = $(shell ls  *.h  |   sed '/libm3l.h/d' | sed '/ipdex.h/d'  |sed '/Server_Header.h/d'  | sort -df)

#OBJS=$(FILES:%.c=%.o)
OBJS = $(shell ls *.c | sed 's/\.c/\.o/'  |   sed '/Server_Main.o/d' | sort -df)

CFLAGS = -fbounds-check -fstack-check -g 
LFLAGS = -fbounds-check -fstack-check -g 
#CFLAGS =
#LFLAGS =
#
#Autodependencies with GNU make
#Scott McPeak, November 2001 
#
# link
#
main:

	ln -sf ../../libm3l/Source/libm3l.* .
	
	@cat COPYRIGHT_NOTICE > ipdex.h
	@echo >> ipdex.h
	@echo "#ifndef   __IPDEX_H__" >> ipdex.h
	@echo "#define   __IPDEX_H__" >> ipdex.h

	@echo "#include " \"$$PWD/Server_Header.h\">>ipdex.h
	@echo "#include " \"$$PWD/ipdex_header.h\" >>ipdex.h

	@$(foreach file,$(HFILES),  echo "#include " \"$$PWD/$(file)\">>ipdex.h;)
	@echo "#endif" >> ipdex.h
	
	make prog

prog: $(OBJS)

	gcc -shared -Wl,-soname,ipdex.so.1.0 -o libipdex.so.1.0   $(OBJS)
	ln -sf libipdex.so.1.0 libipdex.so

	gcc -g -o Server_Main.out Server_Main.c $(OBJS) -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL) -lpthread
# 	gcc -g -o Server_Main.out  $(OBJS) libm3l.a   -lpthread

-include $(OBJS:.o=.d)

%.o: %.c
	gcc -c -g -fPIC $*.c -o $*.o
	gcc -MM -g -fPIC $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

clean:
	rm -f Server_Main.out *.o *.d
