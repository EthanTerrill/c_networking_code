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





#define PORT 443
#define BUFF_LEN 2048


/////////////////////////////////////////////////////////////////////
/// Simple code for speeding up error handling
////////////////////////////////////////////////////////////
void handle_error(const char* error_message) {
  fprintf(stderr, "%s", error_message);
  fprintf(stderr, "%s\n", strerror(errno));
  exit(EXIT_FAILURE);
}


void send_error(const char* error_message) {
  fprintf(stderr, "%s", error_message);
  exit(EXIT_FAILURE);
}




int main(int argnum, char** args) {
  int                 socket_fd;      // socket to server
  int                 ret;

  struct addrinfo*    my_addrinfo;
  const char*         message = "GET /main.html \r\n\r\n";
  char                buff[BUFF_LEN] = {0};
  SSL_CTX*            context;
  SSL*                ssl;
  struct addrinfo hints;





  if (argnum < 2) {
    send_error("no server name specified exiting\n");
  }


  ////////////////////////////////////////////////////////////////
  /// get the addressinfo structure for the server
  /// given through the argument
  /////////////////////////////////////////////////////////////
  memset(&hints, 0, sizeof(struct addrinfo));
  ret = getaddrinfo(args[1], NULL, NULL, &my_addrinfo);
  if (ret == 0) {

    fprintf(stdout, "Success!\n");
    struct addrinfo* addr = my_addrinfo;

    
    ////////////////////////////////////////////////////////////////////
    /// attempt to connect to each available ip address
    //////////////////////////////////////////////////////////////
    while (addr != NULL) {

      fprintf(stderr, "-----------------------------\n");
      struct sockaddr_in* saddress = (struct sockaddr_in*)addr->ai_addr;
      struct protoent* p = getprotobynumber(addr->ai_protocol);
      fprintf(stdout, "proto num: %d\n", ntohs(addr->ai_protocol));

      char* name = (char*)malloc(sizeof(char) * addr->ai_addrlen + 1);
      name[addr->ai_addrlen] = '\0';
      fprintf(stdout, "%s ",
              inet_ntop(addr->ai_family, &(saddress->sin_addr), name, addr->ai_addrlen));
      fprintf(stdout, "%s\n", p->p_name);
      


      
      socket_fd = socket(addr->ai_family, SOCK_STREAM, 0);
      if (socket_fd == -1) {
        fprintf(stderr, "%s\n", addr->ai_canonname);
        handle_error("Error failed to call socket function\n");
      }
      
      
      
      if (addr->ai_family == AF_INET) {
        struct sockaddr_in  server_addr;
        memset(&server_addr, 0, sizeof(struct sockaddr_in));
        server_addr.sin_family      = AF_INET;
        server_addr.sin_port        = htons(PORT);

        ret = inet_pton(AF_INET, name, &server_addr.sin_addr);
        if (ret != 1) {
          handle_error("could not resolve server address\n");
        }
        fprintf(stderr, "port %d\n", ntohs(server_addr.sin_port));
        

        ret = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));
        if (ret == -1) {
          fprintf(stderr, "error could not connect to socket \n%s\n", strerror(errno));
          close(socket_fd);
          addr = addr->ai_next;
          continue;
          handle_error("Could not establish connection to server\n");
        } else { 
          fprintf(stderr, "\tSuccessfully connected to socket\n");
        }
      } else {
        struct sockaddr_in6  server_addr;

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin6_family      = AF_INET6;
        server_addr.sin6_port        = htons(PORT);

        ret = inet_pton(AF_INET6, name, &server_addr.sin6_addr);
        fprintf(stderr, "port %d\n", ntohs(server_addr.sin6_port));
        if (ret != 1) {
          handle_error("could not resolve server address\n");
        }

        ret = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (ret == -1) {
          fprintf(stderr, "error could not connect to socket \n%s\n", strerror(errno));
          close(socket_fd);
          addr = addr->ai_next;
          continue;
          handle_error("Could not establish connection to server\n");
        } else { 
          fprintf(stderr, "\tSuccessfully connected to socket\n");
        }
      }


      context = SSL_CTX_new(TLS_method());
      if (context == NULL) {
        handle_error("Could not set file descriptor to ssl\n");
      }
      fprintf(stdout, "b\n"); 

      ssl     = SSL_new(context);
      if (ssl == NULL) {
        handle_error("Could not set file descriptor to ssl\n");
      }


      fprintf(stdout, "c\n"); 
      ret = SSL_set_fd(ssl, socket_fd);
      if (ret == -1) {
        handle_error("Could not set file descriptor to ssl\n");
      }



      fprintf(stdout, "d\n"); 
      ret = SSL_connect(ssl);
      if (ret == -1) {
        handle_error("Could not perform SSL handshake\n");
      }

      fprintf(stdout, "b\n"); 

      int bytes_left = strlen(message);
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
      fprintf(stdout, "[message recieved]\n%s\n", buff);

      close(socket_fd);




      ///////////////////////////////////////////////////
      /// move on to the next address
      ///////////////////////////////////////////////////
      addr = addr->ai_next;
    }



    /////////////////////////////////////////////////////
    /// clean up clean up everybody, everywhere....
    /////////////////////////////////////////////////
    freeaddrinfo(my_addrinfo);
  } else {
    send_error("Could not find server\n Exiting...\n");
  }

}
