include ./Rules.make

APP = eeprom-read.x

all: $(APP)

$(APP): eeprom-read.c	
	$(CC) eeprom-read.c -o $(APP) $(CFLAGS)
	
clean:
	-rm -f *.o ; rm $(APP)
