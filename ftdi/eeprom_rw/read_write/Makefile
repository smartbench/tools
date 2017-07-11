#TOPDIR  := $(shell cd ..; cd ..; pwd)
include Rules.make

APP = rw.x

all: $(APP)

$(APP): main.c	
	$(CC) main.c -o $(APP) $(CFLAGS)
	
clean:
	-rm -f *.o ; rm $(APP)
