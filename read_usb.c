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


/*
This code configures the file descriptor for use as a serial port.
*/
void configure(int fd) {
struct  termios pts;
tcgetattr(fd, &pts);
cfsetospeed(&pts, 9600);   
cfsetispeed(&pts, 9600);   
tcsetattr(fd, TCSANOW, &pts);
}

// buffers & message
char read_buffer[100], http_buffer[100], http_message[100];

int main(int argc, char* argv[]) {

  if (argc < 2) {
  printf("Please specify the name of the serial port (USB) device file!\n");
  exit(0);
  }

  // get the name from the command line
  char* file_name = argv[1];

  // try to open the file for reading and writing
  // you may need to change the flags depending on your platform
  int fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);

  if (fd < 0) {
  perror("Could not open file\n");
  exit(1);
  }
  else {
  printf("Successfully opened %s for reading and writing\n", file_name);
  }

  configure(fd);

  /*
  Write the rest of the program below, using the read and write system calls.
  */

  http_buffer[0] = '\0';

  int bytes_read, bytes_written, http_cursor = 0;

  // attempt to read indefinitely
  while (1) {

    bytes_read = read(fd, read_buffer, 100);

    if (bytes_read == -1) {

      continue;

    } else if (bytes_read > 0) {

      // store whatever was read into the read buffer
      read_buffer[bytes_read] = '\0';
      
      // add read bytes to http_buffer
      strcat(http_buffer, read_buffer);
      printf("%s\n", http_buffer);

      // iterate through http_buffer
      for (; http_cursor < http_cursor + bytes_read; http_cursor++) {

        // if newline character found
        if (http_buffer[http_cursor] == '\n') {

          // store new temperature string in http_message
          http_buffer[http_cursor + 1] = '\0';
          http_message[0] = '\0';
          strcpy(http_message, http_buffer);

          // reinitialize http_buffer and http_cursor
          http_buffer[0] = '\0';
          http_cursor = 0;

          printf("%s\n", http_message);

          break;
        }
      }
    }
  }
}
