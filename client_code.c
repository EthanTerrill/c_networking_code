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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>



#define PORT 8080
#define BUFF_LEN 2048
void handle_error(const char* error_message) {
  fprintf(stderr, "%s", error_message);
  fprintf(stderr, "%s\n", strerror(errno));
  exit(EXIT_FAILURE);
}


int main() {
  int                 socket_fd, ret, bytes_left;
  struct sockaddr_in  server_addr;
  const char*         message = "Hello from the client!\n";
  char                buff[BUFF_LEN] = {0};


  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    handle_error("Error failed to call socket function\n");
  }



  server_addr.sin_family      = AF_INET;
  server_addr.sin_port        = htons(PORT);
  ret = inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
  if (ret != 1) {
    handle_error("could not resolve server address\n");
  }


  ret = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (ret == -1) {
    handle_error("Could not establish connection to server\n");
  }



  bytes_left = strlen(message);
  while (bytes_left) {
    ret = write(socket_fd, message, bytes_left);
    if (ret == -1) {
      if (errno != EAGAIN && errno != EINTR) {
        close(socket_fd);
        handle_error("failed to send message to server\n");
      }
    } else {
      bytes_left -= ret;
    }
  }

  ret = read(socket_fd, &buff, BUFF_LEN - 1);
  fprintf(stdout, "[message recieved]\n%s", buff);



  close(socket_fd);
  exit(EXIT_SUCCESS);
}
