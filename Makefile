CC = clang
ARGS = -Wall

all: read_usb server

read_usb:
	$(CC) -c $(ARGS) read_usb.c

server: read_usb
	$(CC) -o server $(ARGS) server.c read_usb.c

clean: 
	rm -rf server
