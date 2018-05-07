#FILES = $(shell find ./ -name "*.c" | sed 's/.\///' )
FILES = $(shell ls  *.c )
PATHL  = $(shell pwd )
HFILES = $(shell ls  *.h  |   sed '/lsipdx_header.h/d'  |   sed '/lsipdx.h/d' | sed '/libm3l.h/d' | sort -df)
PATHLIBM3L = 
EMPTYL = $(shell find $(PATHLIBM3L) \( -name libm3l.so -o -iname libm3l.h -o -iname libm3l.so.1.0 \) | wc -l )

CC = gcc
# OBJS=$(FILES:%.c=%.o)
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
#
#  delete empty links
#
	@find -L -type l -delete

ifeq ($(EMPTYL),3)

	@ln -sf $(PATHLIBM3L)/libm3l.so .
	@ln -sf libm3l.so libm3l.so.1.0
	@ln -sf $(PATHLIBM3L)/libm3l.h .

	@cat COPYRIGHT_NOTICE > lsipdx.h
	@echo >> lsipdx.h
	@echo "#ifndef   __IPDEX_H__" >> lsipdx.h
	@echo "#define   __IPDEX_H__" >> lsipdx.h

	@echo "#include " \"$$PWD/lsipdx_header.h\" >>lsipdx.h
	
	@$(foreach file,$(HFILES),  echo "#include " \"$$PWD/$(file)\">>lsipdx.h;)
	@echo "#endif" >> lsipdx.h
	
	make prog
else
#
#  did not find libm3l files, posibly PATHLIBM#L wrong
#
	@echo ERROR: Did not find libml3 libraries and header file
	@echo
	@echo Check PATHLIBM3L variable setting
	@echo
endif


prog: $(OBJS)
	$(CC) -shared -Wl,-soname,liblsipdx.so.1.0 -o liblsipdx.so.1.0   $(OBJS)
	ln -sf liblsipdx.so.1.0 liblsipdx.so
	
	$(CC) -shared -Wl,-soname,libm3lsipdx.so.1.0 -o libm3lsipdx.so.1.0   $(OBJS) $(PATHLIBM3L)/*.o
	ln -sf libm3lsipdx.so.1.0 libm3lsipdx.sos
	
	$(CC) -g -o Server_Main.out Server_Main.c $(OBJS) -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL) -lpthread -lm
	cp Server_Main.out lsipdx_server.out
# 	$(CC) -g -o Server_Main.out  $(OBJS) libm3l.a   -lpthread

-include $(OBJS:.o=.d)

%.o: %.c
	$(CC) -c -g -Wall -fPIC $*.c -o $*.o
	$(CC) -MM -g -Wall -fPIC $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

clean:
	rm -f Server_Main.out lsipdx_server.out *.o *.d  libm3l.so libm3l.h libm3l.so.1.0 libm3lsipdx.so.1.0 libm3lsipdx.so

