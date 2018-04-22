/* 
This code primarily comes from 
http://www.prasannatech.net/2008/07/socket-programming-tutorial.html
and
http://www.binarii.com/files/papers/c_sockets.txt
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "read_usb.h"

int run_server(int PORT_NUMBER, int write_fd, char* htmlpage)
{

  // structs to represent the server and client
  struct sockaddr_in server_addr, client_addr;    
  
  int sock; // socket descriptor

  // 1. socket: creates a socket descriptor that you later use to make other system calls
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
  	perror("Socket");
  	exit(1);
  }

  int temp;
  if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int)) == -1) {
  	perror("Setsockopt");
  	exit(1);
  }

  // configure the server
  server_addr.sin_port = htons(PORT_NUMBER); // specify port number
  server_addr.sin_family = AF_INET;         
  server_addr.sin_addr.s_addr = INADDR_ANY; 
  bzero(&(server_addr.sin_zero),8); 
  
  // 2. bind: use the socket and associate it with the port number
  if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
  	perror("Unable to bind");
  	exit(1);
  }

  // 3. listen: indicates that we want to listen to the port to which we bound; second arg is number of allowed connections
  if (listen(sock, 1) == -1) {
  	perror("Listen");
  	exit(1);
  }
      
  // once you get here, the server is set up and about to start listening
  printf("\nServer configured to listen on port %d\n", PORT_NUMBER);
  fflush(stdout);

  // 4. accept
  int sin_size = sizeof(struct sockaddr_in);

  int fd = -1;
  
  // Initial GET, all HTML
  fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);

  int bytes_received;

  // process request
  if (fd != -1) {
    printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

    // buffer to read data into
    char request[1024];

    // 5. recv: read incoming message (request) into buffer
    bytes_received = recv(fd,request,1024,0);
    // null-terminate the string
    request[bytes_received] = '\0';
    // print it to standard out
    printf("This is the incoming request:\n%s\n", request);

    char html[10000];
    FILE *fp;
    char file_read_buffer[BUFFER_SIZE];

    html[0] = '\0';
    strcat(html, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n");
    fp = fopen(htmlpage,"rb");
    while( fgets (file_read_buffer, 100, fp)!=NULL ) {
        strcat(html, file_read_buffer);
     }
    fclose(fp);
    strcat(html, "</p></html>");

    // 6. send: send the outgoing message (response) over the socket
    // note that the second argument is a char*, and the third is the number of chars	
    send(fd, html, strlen(html), 0);

    // 7. close: close the connection
    close(fd);
    printf("Server closed connection\n");
  } else {
   printf("some critical error\n");
    exit(1);
  }
  
  // wait for Arduino to reboot
  sleep(2);
  
  // Keep socket open, do Ajax stuff and communicate with client
  while (1) {
      
      fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);

      // process request
      if (fd != -1) {
      printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

      //send actual + computed high medium and low value 
      //need to 

      // buffer to read data into
      char request[1024];

      // // 5. recv: read incoming message (request) into buffer
      bytes_received = recv(fd,request,1024,0);
      // null-terminate the string
      request[bytes_received] = '\0';
      // print it to standard out
      printf("This is the incoming request:\n%s\n", request);
      
      //   //TODO send AJAX data
      // dummy write
//       pthread_mutex_lock(&write_lock);
//         if (msg_temp == 0) {
//           write_buffer[0] = 'b';
//           msg_temp = 1;
//         } else {
//           write_buffer[0] = 'r';
//           msg_temp = 0;
//         }
//       pthread_mutex_unlock(&write_lock);

      printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
      
      // 5. recv: read incoming message (request) into buffer
      int bytes_received = recv(fd,request,1024,0);
      // null-terminate the string
      request[bytes_received] = '\0';
      // print it to standard out
      printf("This is the incoming request:\n%s\n", request);

      char reply[1024];
      reply[0] = '\0';

      // send message
      strcat(reply, "HTTP/1.1 200 OK\nContent-Type: text/json\n\n{\"temp\": \"");
      pthread_mutex_lock(&read_lock);
        strcat(reply, http_message);
      pthread_mutex_unlock(&read_lock);
      strcat(reply, "\"}");

      // 6. send: send the outgoing message (response) over the socket
      // note that the second argument is a char*, and the third is the number of chars 
      send(fd, reply, strlen(reply), 0);
      
      // 7. close: close the connection
      // NOTE: if you don't do this, the page never loads
      close(fd);
      printf("Server closed connection\n");
    }
  }

  // 8. close: close the socket
  close(sock);
  printf("Server shutting down\n");

  return 0;
}

// for the purpose of starting a new thread with read_usb
void* run_read_usb(void* v) {

  read_usb(*(int*)v);
  return v;

}

int main(int argc, char *argv[])
{
  // check the number of arguments
  if (argc != 4) {
      printf("\nUsage: %s [port_number] [file_path] [html_path]\n", argv[0]);
      exit(-1);
  }

  memset(write_buffer, 0, BUFFER_SIZE);

  int port_number = atoi(argv[1]);
  if (port_number <= 1024) {
    printf("\nPlease specify a port number greater than 1024\n");
    exit(-1);
  }

  // open file
  int fd = open(argv[2], O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
  perror("Could not open file\n");
  exit(1);
  }
  else {
  printf("Successfully opened %s for reading and writing\n", argv[2]);
  }

  configure(fd);

  int ret_val;
  pthread_t t1;

  // TODO: check ret_val
  ret_val = pthread_create(&t1, NULL, &run_read_usb, &fd);

  run_server(port_number, fd, argv[3]);
}
