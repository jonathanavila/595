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

#define NUM_THREADS   50

void* t_handle_connection(void*);
void handle_connection(int);

// client handler thread variables
pthread_t client_threads[NUM_THREADS];
int client_fds[NUM_THREADS], t_current;
pthread_mutex_t client_lock;

// main server thread variables
int port_number = 0;
char* html_file_path = NULL;

int run_server()
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
  server_addr.sin_port = htons(port_number); // specify port number
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
  printf("\nServer configured to listen on port %d\n", port_number);
  fflush(stdout);

  // 4. accept
  int sin_size = sizeof(struct sockaddr_in);

  // accept connections and create threads as appropriate
  int fd = -1;
  while (1) {

    fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);

    if (fd == -1) {
      perror("accept connection");
      exit(1);
    }

    printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

    // iterate through client threads, create new thread with first available id
    for (int i = 0; i < NUM_THREADS; i++) {
      if (client_threads[i] == NULL) {
        client_threads[i] = malloc(sizeof(pthread_t));
        client_fds[i] = fd;
        t_current = i;
        pthread_create(&client_threads[i], NULL, &t_handle_connection, &t_current);
        break;
      }
    }

    // iterate through client threads, setting all completed threads to null
    pthread_mutex_lock(&client_lock);
      for (int i = 0; i < NUM_THREADS; i++) {
        if (client_fds[i] == -2) {
          client_fds[i] = -1;
          // free(client_threads[i]); // Causes error. Do they free themselves?
          client_threads[i] = NULL;
        }
      }
    pthread_mutex_unlock(&client_lock);
  }
  
  // 8. close: close the socket
  close(sock);
  printf("Server shutting down\n");

  return 0;
}

// handle client communication (only to be run in new thread)
void handle_connection(int t_id) {

  // buffer to read data into
  char request[2048];

  // printf("Client Thread %d: fd: %d\n", t_id, client_fds[t_id]); // TESTING

  // 5. recv: read incoming message (request) into buffer
  int bytes_received = recv(client_fds[t_id],request,1024,0);
  // null-terminate the string
  request[bytes_received] = '\0';
  // printf("Client Thread %d: This is the incoming request:\n%s\n", t_id, request); // TESTING

  // process request
  if (request[0] == 'G') { // GET

    // read in html page
    char html[10000];
    FILE *fp;
    char file_read_buffer[BUFFER_SIZE];

    html[0] = '\0';
    file_read_buffer[0] = '\0';

    strcat(html, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n");

    fp = fopen(html_file_path,"rb");
    if (fp == NULL) {
      perror("Read HTML file");
      exit(1);
    }
    while( fgets(file_read_buffer, BUFFER_SIZE - 1, fp) != NULL ) {
        strcat(html, file_read_buffer);
        file_read_buffer[0] = '\0';
    }
    fclose(fp);

    // 6. send: send the outgoing message (response) over the socket
    // note that the second argument is a char*, and the third is the number of chars 
    send(client_fds[t_id], html, strlen(html), 0);

  } else if (request[0] == 'P') { // POST

    char request_buffer[BUFFER_SIZE];
    
    // process incoming request
    memset(request_buffer, 0, BUFFER_SIZE);

    // extract first line of request (contains query string)
    int request_length = strlen(request);
    for (int i = 0; i < request_length; i++) {
      if (request[i] == '\n') {
        request_buffer[i] = '\0';
        break;
      }
      else request_buffer[i] = request[i];
    }

    // extract query string from first line of request
    request_length = strlen(request_buffer);
    for (int i = 0; i < request_length; i++) {
      if (request_buffer[i] == '?') { // query string starts with '?'
        for (int j = 0; j < 6; j++) {
          request_buffer[j] = request_buffer[i + j + 1];
        }
        request_buffer[6] = '\0'; // query always 6 characters long
        break;
      }
    }

    printf("Client Thread %d: Processed request: %s\n", t_id, request_buffer); // TESTING

    // write querty string (client state) to usb file
    pthread_mutex_lock(&write_lock);
      write_buffer[0] = '\0';
      strcpy(write_buffer, request_buffer);
    pthread_mutex_unlock(&write_lock);

    // create HTTP reply
    char reply[1024];
    reply[0] = '\0';

    strcat(reply, "HTTP/1.1 200 OK\nContent-Type: text/plain\n\n");
    pthread_mutex_lock(&read_lock);
        strcat(reply, http_message); // http_message is global, from read_usb.h
        printf("Client Thread %d: http_message: %s\n", t_id, http_message); // TESTING

        // 6. send: send the outgoing message (response) over the socket
        // note that the second argument is a char*, and the third is the number of chars 
        send(client_fds[t_id], reply, strlen(reply), 0);
    pthread_mutex_unlock(&read_lock);

  } else { // unhandled request
    printf("Client Thread %d: Unhandled request: \n%s\n", t_id, request); // TESTING
  }

  // 7. close: close the connection
  close(client_fds[t_id]);
  printf("Client Thread %d: Server closed connection\n", t_id);

  // flag this thread's id for completion
  pthread_mutex_lock(&client_lock);
    client_fds[t_id] = -2;
  pthread_mutex_unlock(&client_lock);
}

// for the purpose of starting a new thread with run_server
void* t_run_server(void* v) {

  run_server();
  return v;

}

// for the purpose of starting a new thread for client communication
void* t_handle_connection(void* v) {

  handle_connection(*(int*)v);
  return v;
}

// for the purpose of starting a new thread with read_usb
void* t_read_usb(void* v) {

  read_usb((char*)v);
  return v;

}

int main(int argc, char *argv[])
{
  // check the number of arguments
  if (argc != 4) {
      printf("\nUsage: %s [port_number] [usb_file_path] [html_file_path]\n", argv[0]);
      exit(-1);
  }

  // memset global arrays where necessary
  memset(write_buffer, 0, BUFFER_SIZE);
  memset(http_buffer, 0, BUFFER_SIZE);
  memset(http_message, 0, BUFFER_SIZE);
  memset(client_threads, 0, NUM_THREADS);
  memset(client_fds, -1, NUM_THREADS);
  // for (int i = 0; i < NUM_THREADS; i++) {
  //   client_threads[i] = NULL;
  // }

  // set global port number
  port_number = atoi(argv[1]);
  if (port_number <= 1024) {
    printf("\nPlease specify a port number greater than 1024\n");
    exit(-1);
  }

  // set global html_file_path
  html_file_path = malloc(strlen(argv[3]));
  html_file_path[0] = '\0';
  strcpy(html_file_path, argv[3]);

  // thread variables
  int ret_val;
  pthread_t t_usb, t_server;

  // run read_usb
  ret_val = pthread_create(&t_usb, NULL, &t_read_usb, &argv[2][0]);
  if (ret_val != 0) {
    perror("read_usb thread creation");
    exit(1);
  }

  // wait for Arduino to start up
  sleep(3);

  // start server
  ret_val = pthread_create(&t_server, NULL, &t_run_server, NULL);
  if (ret_val != 0) {
    perror("run_server thread creation");
    exit(1);
  }

  // wait for user input
  char c;
  while (1) {
    c = getc(stdin);
    if (c == 'q') {
      printf("Server shutting down...\n");
      pthread_mutex_lock(&read_lock);
      pthread_mutex_lock(&write_lock);
      free(html_file_path);
      exit(0);
    }
  }
}
