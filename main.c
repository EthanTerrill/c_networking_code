//////////////////////////////////////////////////////////////////////////////////////
/// Copyright 2025 Ethan Terrill open source license
///
///
///
///
///
////////////////////////////////////////////////////////////////////////


#include <asm-generic/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

#define HTTP_PORT 8080


typedef struct http_request{
  char* request;
  int request_type;
  char* path;
  char* user_agent;
  char* accept; 
} http_request;



void parse_http_request(char * buff, size_t buff_size){
  
}


#define BUFF_LEN 2048


char* get_file(char* filename){
  int result = 1;
  char* buff;
  int fd = open(filename, O_RDONLY);
  off_t size = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  
  buff = (char*)malloc(sizeof(char) * size);
  if (buff == NULL) {
    fprintf(stderr, "malloc failed\n");
    exit(EXIT_FAILURE);
  }

  while (result) {
    result = read(fd, buff, size);
    if (result == -1) {
      if (errno != EINTR && errno != EAGAIN) {
        fprintf(stderr, "error reading file");
        exit(EXIT_FAILURE);
      }
    }
  }
  
  return buff;
}

int main() {
  int                 socket_fd, new_socket;
  int                 result;
  struct sockaddr_in  my_sockaddr;
  socklen_t           addr_len = sizeof(my_sockaddr);
  int opt = 1;

  ssize_t val;
  char buffer[BUFF_LEN] = {0};
  

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    fprintf(stderr, "Error failed to get socket file descriptor\n");
    fprintf(stderr, "%s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  

  if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    fprintf(stderr, "Error failed to set socket options\n");
    fprintf(stderr, "%s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  memset(&my_sockaddr, 0, sizeof(my_sockaddr));
  my_sockaddr.sin_family  = AF_INET;
  my_sockaddr.sin_port    = htons(HTTP_PORT);
  my_sockaddr.sin_addr.s_addr = INADDR_ANY;


  result = bind(socket_fd, (struct sockaddr*)&my_sockaddr, addr_len);
  if (result == -1) {
    fprintf(stderr, "Error failed to bind socket\n");
    fprintf(stderr, "%s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  result = listen(socket_fd, 3);
  if (result == -1) {
    fprintf(stderr, "Error failed to mark socket for connection mode\n");
    fprintf(stderr, "%s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
 
  while(1) {
    new_socket = accept(socket_fd, (struct sockaddr*)&my_sockaddr, &addr_len);  
    if (new_socket == -1) {
      fprintf(stderr, "accept socket \n");
      fprintf(stderr, "%s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    val = read(new_socket, buffer, BUFF_LEN - 1);

    fprintf(stdout, "[message recieved]\n");
    fprintf(stdout, "%s\n", buffer);
    fprintf(stdout, "[Sending repsonse]\n");
    char* response = get_file("main.html");
    fprintf(stdout, "[response sent]\n");

    int bytes_left = strlen(response);
    int read;
    while(bytes_left) {
      read = send(new_socket, response, bytes_left, 0);
      if(read == -1) {
        if(errno != EAGAIN && errno != EINTR) {
          fprintf(stderr, "Error could write data to socket\n");
          exit(EXIT_FAILURE);
        }
      }
      else {
        bytes_left -= read;
      }
      fprintf(stdout, "Bytes left to read: %d\n", read);
    }
    free(response);
    close(new_socket);
    for(int i = 0; i < BUFF_LEN; i++) {
      buffer[i] = 0;
    }
  }

    fprintf(stderr, "Closing Sockets!\n");
  close(socket_fd);


  exit(EXIT_SUCCESS);
}

