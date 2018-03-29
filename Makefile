CC = clang
ARGS = -Wall

all: read_usb server

read_usb:
	$(CC) -o read_usb $(ARGS) read_usb.c

server:
	$(CC) -o server $(ARGS) server.c

clean: 
	rm -rf read_usb server
