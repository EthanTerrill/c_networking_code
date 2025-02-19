//////////////////////////////////////////////////////////////////////////////////////
// Copyright 2025 Ethan Terrill open source license
//
//
//
//
//
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

#include "web_str.h"

#define HTTP_PORT 8080


char* get_file(char* filename) {
  int result = 1;
  char* buff;
  int fd = open(filename, O_RDONLY);
  off_t size = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);

  buff = (char*)malloc(sizeof(char) * (size + 1));
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
  buff[size] = '\0';
  return buff;
}

int main() {
  int                 socket_fd, new_socket;
  int                 result;
  struct sockaddr_in  my_sockaddr;
  socklen_t           addr_len = sizeof(my_sockaddr);
  int opt = 1;

  web_str client_input;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    fprintf(stderr, "Error failed to get socket file descriptor\n");
    fprintf(stderr, "%s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }


  if (
    setsockopt(socket_fd,
                 SOL_SOCKET,
                 SO_REUSEADDR | SO_REUSEPORT,
                 &opt, sizeof(opt)
                 )
  ) {
    fprintf(stderr, "Error failed to set socket options\n");
    fprintf(stderr, "%s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

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



  while (1) {
    new_socket = accept(socket_fd, (struct sockaddr*)&my_sockaddr, &addr_len);
    if (new_socket == -1) {
      fprintf(stderr, "accept socket \n");
      fprintf(stderr, "%s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

    int size = 8000;
    client_input.length = size;
    client_input.str = (char*)malloc(sizeof(char) * size);

    result = read(new_socket, client_input.str, size);
    fprintf(stdout, "%d\n", result);
    if (result == -1) {
      fprintf(stderr, "failed to read from socket");
      exit(EXIT_FAILURE);
    }
    client_input.str[result - 1] = '\0';
    client_input.length = result;
    result = request_is_safe(client_input);
    if (result == 0) {
      fprintf(stderr, "bad request\n");
    }


    fprintf(stdout, "[message recieved]\n");
    fprintf(stdout, "%s\n", client_input.str);
    fprintf(stdout, "[Sending repsonse]\n");
    char* response = get_file("main.html");
    fprintf(stdout, "[response sent]\n");

    int bytes_left = strlen(response);
    int read;
    while (bytes_left) {
      read = write(new_socket, response, bytes_left);
      if (read == -1) {
        if (errno != EAGAIN && errno != EINTR) {
          fprintf(stderr, "Error could write data to socket\n");
          exit(EXIT_FAILURE);
        }
      } else {
        bytes_left -= read;
      }
      fprintf(stdout, "Bytes left to read: %d\n", read);
    }
    free(response);
    close(new_socket);
    free(client_input.str);
  }

  fprintf(stderr, "Closing Sockets!\n");
  close(socket_fd);
  exit(EXIT_SUCCESS);
}
