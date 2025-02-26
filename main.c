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

#define DEBUG

#include "web_str.h"
#include "HTTP_request_parser.h"
#include "HTTP_response.h"

#include "file_system.h"

#define HTTP_PORT 8080



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

  FileSystem* fs;
  populate_file_system(".git", &fs);
  print_file_system(fs);

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
    if (strcmp(client_input.str, "0xDEADBEEF") == 0) {
      free(client_input.str);
      close(new_socket);
      break;
    }


    http_request* req = allocate_http_request(new_socket,
                                              my_sockaddr.sin_addr.s_addr,
                                              client_input);

    HTTP_response_header* response = get_https_reponse(*req);


    fprintf(stdout, "\n-------------------------------------------\n");
    fprintf(stdout, "[message recieved]\n");

    fprintf(stdout, "client addr: [");
    fprintf(stdout, "%d.",   (my_sockaddr.sin_addr.s_addr)       & 0xff);
    fprintf(stdout, "%d.",   (my_sockaddr.sin_addr.s_addr >> 8)  & 0xff);
    fprintf(stdout, "%d.",   (my_sockaddr.sin_addr.s_addr >> 16) & 0xff);
    fprintf(stdout, "%d]\n", (my_sockaddr.sin_addr.s_addr >> 24) & 0xff);

    fprintf(stdout, "%s\n", client_input.str);
    fprintf(stdout, "[Sending repsonse]\n");
    //char* response = get_file("content/main.html");
    fprintf(stdout, "[response sent]\n");
    fprintf(stdout, "\n-------------------------------------------\n");

    //int bytes_left = strlen(response);
    send_response(new_socket, response);

    free(response);
    close(new_socket);
    free_http_request(req);
  }
  fprintf(stderr, "Closing Sockets!\n");
  close(socket_fd);
  exit(EXIT_SUCCESS);
}
