#TOPDIR  := $(shell cd ..; cd ..; pwd)
#include $(TOPDIR)/Rules.make
include ./Rules.make

APP = eeprom-read

all: $(APP)

$(APP): eeprom-read.c
	$(CC) eeprom-read.c -o $(APP) $(CFLAGS)
	
clean:
	-rm -f *.o ; rm $(APP)
