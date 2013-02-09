#FILES = $(shell find ./ -name "*.c" | sed 's/.\///' )
FILES = $(shell ls  *.c )
PATHL  = $(shell pwd )

OBJS=$(FILES:%.c=%.o)

CFLAGS = -fbounds-check -fstack-check -g 
LFLAGS = -fbounds-check -fstack-check -g 
#CFLAGS =
#LFLAGS =

#Autodependencies with GNU make
#Scott McPeak, November 2001 
#
# link
#
main: $(OBJS)

	gcc -g -o Server_Main.out  $(OBJS) -L$(PATHL)  -lm3l -Wl,-rpath=$(PATHL) -lpthread

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
