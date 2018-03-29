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

void configure(int fd);
int read_usb(char* file_name);