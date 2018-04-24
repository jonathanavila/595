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
#include <pthread.h>

#include "read_usb.h"

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

// int read_usb(char* file_name) {
int read_usb(char* file_name) {

  // global variable to count failed reads
  unsigned long int failed_reads = 0;

  int fd = open(file_name, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
  perror("Could not open file\n");
  exit(1);
  }
  else {
  printf("Successfully opened %s for reading and writing\n", file_name);
  }

  configure(fd);

  // memset buffers and http_message
  memset(http_buffer, 0, BUFFER_SIZE);
  memset(http_message, 0, BUFFER_SIZE);

  int bytes_read, http_cursor = 0;  

  // attempt to read indefinitely
  while (1) {

    memset(read_buffer, 0, BUFFER_SIZE);

    if (failed_reads >= FAILED_READ_LIMIT) {
      sleep(3);
      fd = open(file_name, O_RDWR | O_NOCTTY | O_NDELAY);
      if (fd < 0) continue;
      configure(fd);
    }

    // write message if it exists
    pthread_mutex_lock(&write_lock);
      if (failed_reads == 0 && write_buffer[0] != '\0') {

        int bytes_written = write(fd, &write_buffer[0], sizeof(char) * 7);
        if (bytes_written < 7) {
          perror("Write usb");
          exit(1);
        }

        write_buffer[0] = '\0';
      }
    pthread_mutex_unlock(&write_lock);

    // read from usb file
    bytes_read = read(fd, read_buffer, BUFFER_SIZE - 1);

    if (bytes_read == -1) { // something went wrong

      failed_reads++;
      if (failed_reads == FAILED_READ_LIMIT) {
        http_message[0] = '\0';
        strcat(http_message, READ_FAILURE_FLAG);
        printf("\n\n\tArduino disconnected!\nTemperature data transmission suspended.\n\n");
      } else if (failed_reads > FAILED_READ_LIMIT) {
        failed_reads = FAILED_READ_LIMIT + 1;
      }
      continue;

    } else if (bytes_read > 0) {

      if (failed_reads > (FAILED_READ_LIMIT - 1)) {
        failed_reads = 0;
        printf("\n\n\tArduino reconnected.\nResuming temperature data transmission.\n\n");
      }

      // store whatever was read into the read buffer
      read_buffer[bytes_read] = '\0';

      // set cursor per http_buffer
      if (strlen(http_buffer) > 0) http_cursor = strlen(http_buffer) - 1;
      else http_cursor = 0;
      
      // add read bytes to http_buffer
      strcat(http_buffer, read_buffer);

      // iterate through http_buffer
      for (int i = http_cursor; i < http_cursor + bytes_read; i++) {

        // if newline character found
        if (http_buffer[i] == '\n') {

          // store new temperature string in http_message
          http_buffer[4] = '\0'; // XX.X

          pthread_mutex_lock(&read_lock);
            http_message[0] = '\0';
            strcpy(http_message, http_buffer);
            // printf("%s\n", http_message); // for testing
          pthread_mutex_unlock(&read_lock);

          // reinitialize http_buffer
          memset(http_buffer, 0, BUFFER_SIZE);

          break;
        }
      }
    }
  }
}
