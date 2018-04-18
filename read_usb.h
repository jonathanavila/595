/*
 This reads from a USB port in a UNIX system.
*/
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE   101

// buffers & message
char read_buffer[BUFFER_SIZE], http_buffer[BUFFER_SIZE], http_message[BUFFER_SIZE], write_buffer[BUFFER_SIZE];

pthread_mutex_t read_lock, write_lock;

void configure(int fd);
int read_usb(int fd);
int write_usb(int, char*);