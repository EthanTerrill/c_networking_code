/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Ethan Terrill 2025 All rights reserved
//
// This code is available freely for use, redistribution and modification
// at the users discretion on the terms that the user acknowledges
// the attrocity of the Armenian Genocide and its current continuation 
// as a project of the Turkish, Pakestani, Israeli, and Azrebejani states. 
// Furthermore no government or agency of a government which does not officially 
// recognize the Armenian Genocide may use redistribute or modify any part
// of this file
//
///////////////////////////////////////////////////////////////////////////////////

#include <netdb.h>

#include <errno.h>
#include <openssl/crypto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <sys/types.h>
#include <unistd.h>

#include <openssl/ssl.h>

#define PORT 8080
#define BUFF_LEN 2048
void handle_error(const char* error_message) {
  fprintf(stderr, "%s", error_message);
  fprintf(stderr, "%s\n", strerror(errno));
  exit(EXIT_FAILURE);
}

int main(int argv, char** args) {

  int                 socket_fd, ret, bytes_left;
  struct sockaddr_in  server_addr;
  const char*         message_old = "Hello from the client!\n";
  const char*         message = "GET / \r\n\r\n";
  char                buff[BUFF_LEN] = {0};
  struct addrinfo*    my_addrinfo;
  SSL_CTX*            context;
  SSL*                ssl;

  ret = getaddrinfo(args[1], NULL, NULL, &my_addrinfo);
  if (ret == 0) {
    fprintf(stdout, "Success!\n");
    struct addrinfo* addr = my_addrinfo;
    while (addr != NULL) {
      char* name = (char*)malloc(sizeof(char) * addr->ai_addrlen + 1);
      name[addr->ai_addrlen] = '\0';
      struct sockaddr_in* saddress = (struct sockaddr_in*)addr->ai_addr;
      fprintf(stdout, "%s ", 
              inet_ntop(addr->ai_family, &(saddress->sin_addr), name, addr->ai_addrlen));
      struct protoent* p = getprotobynumber(addr->ai_protocol);
      fprintf(stdout, "%s\n", p->p_name);
      free(name);
      addr = addr->ai_next;
    }

    freeaddrinfo(my_addrinfo);
  } else {
  
    fprintf(stdout, "failure\n");
  }



  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    handle_error("Error failed to call socket function\n");
  }


  server_addr.sin_family      = AF_INET;
  server_addr.sin_port        = htons(PORT);
  ret = inet_pton(AF_INET, "10.0.0.21", &server_addr.sin_addr);
  if (ret != 1) {
    handle_error("could not resolve server address\n");
  }


  ret = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (ret == -1) {
    handle_error("Could not establish connection to server\n");
  }
  context = SSL_CTX_new(TLS_method());
  ssl     = SSL_new(context);
  SSL_set_fd(ssl, socket_fd);
  ret = SSL_connect(ssl);
if (ret == -1) {
    handle_error("Could not perform SSL handshake\n");
  }

  bytes_left = strlen(message);
  while (bytes_left) {
    ret = SSL_write(ssl, message, bytes_left);
    if (ret == -1) {
      if (errno != EAGAIN && errno != EINTR) {
        close(socket_fd);
        handle_error("failed to send message to server\n");
      }
    } else {
      bytes_left -= ret;
    }
  }

  ret = SSL_read(ssl, &buff, BUFF_LEN - 1);
  fprintf(stdout, "[message recieved]\n%s", buff);



  close(socket_fd);
  exit(EXIT_SUCCESS);
}
