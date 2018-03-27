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

// read buffer
char buffer[100];

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

  int bytes_read, bytes_written;

  while (1) {

    bytes_read = read(fd, buffer, 100);

    if (bytes_read == -1) {

      continue;

    } else if (bytes_read > 0) {

      // store the full temperature strings in a variable for the server part
      buffer[bytes_read] = '\0';
      printf("%s", buffer);

    }

  }

}
