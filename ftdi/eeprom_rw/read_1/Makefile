include ./Rules.make

APP = read.x

all: $(APP)

$(APP): read.c
	$(CC) read.c -o $(APP) $(CFLAGS)
	
clean:
	-rm -f *.o ; rm $(APP)
