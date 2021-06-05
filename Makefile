CC=gcc
SOURCES=main.o
LIBS=-lusb-1.0
RES=usb

$(RES): $(SOURCES)
	$(CC) $(SOURCES) $(LIBS) -o $(RES)

main.o:	main.c
	$(CC) main.c -c
	
clean:
	rm *.o
clean_all:
	rm *.o $(RES)
