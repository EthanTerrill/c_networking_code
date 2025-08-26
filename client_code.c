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




// ===================================== Struct definitions =========================================== //
#define CONNECTION_SSL
#define CONNECTION_HTTP

// ===================================== Struct definitions =========================================== //
typedef struct connection{
  int socket_fd;
  char* site_name;
  char* resource_name;
  struct addrinfo* addrinfo;
  int connection_type;
}
connection;


// ===================================== Function Prototypes =========================================== //
#define PORT 443
#define BUFF_LEN 2048


/////////////////////////////////////////////////////////////////////
/// Simple code for speeding up error handling
////////////////////////////////////////////////////////////
void handle_error(const char* error_message);

void send_error(const char* error_message);



int connect_to_addr(struct addrinfo* addr, int socket_fd);



int request_resource(int socket_fd, char* resource_name);

int main(int argnum, char** args) {
  int                 socket_fd;      // socket to server
  int                 ret;

  struct addrinfo*    my_addrinfo;
  const char*         message = "GET / \r\n\r\n";
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

    fprintf(stderr, "Success!\n");
    struct addrinfo* addr = my_addrinfo;

    
    ////////////////////////////////////////////////////////////////////
    /// attempt to connect to each available ip address
    //////////////////////////////////////////////////////////////
    while (addr != NULL) {

      socket_fd = socket(addr->ai_family, SOCK_STREAM, 0);
      if (socket_fd == -1) {
        fprintf(stderr, "%s\n", addr->ai_canonname);
        handle_error("Error failed to call socket function\n");
      }

      if (connect_to_addr(addr, socket_fd) == -1) {
        addr = addr->ai_next;
      } else {
        context = SSL_CTX_new(TLS_method());
        if (context == NULL) {
          handle_error("Could not set file descriptor to ssl\n");
        }

        ssl     = SSL_new(context);
        if (ssl == NULL) {
          handle_error("Could not set file descriptor to ssl\n");
        }


        ret = SSL_set_fd(ssl, socket_fd);
        if (ret == -1) {
          handle_error("Could not set file descriptor to ssl\n");
        }

        ret = SSL_connect(ssl);
        if (ret == -1) {
          handle_error("Could not perform SSL handshake\n");
        }

        break;
      }
      ///////////////////////////////////////////////////
      /// move on to the next address
      ///////////////////////////////////////////////////
    }

    fprintf(stderr, "writing message\n");
    fprintf(stderr, "%s\n", message);
    int bytes_left = strlen(message);
    int bytes_read = 0;
    while (bytes_left) {
      ret = SSL_write(ssl, message + bytes_read, bytes_left);
      if (ret == -1) {
        if (errno != EAGAIN && errno != EINTR) {
          close(socket_fd);
          handle_error("failed to send message to server\n");
        }
      } else {
        bytes_left -= ret;
        bytes_read += ret;
      }
    }
  

    fprintf(stderr, "[message recieved]\n");
    while (ret > 0) {
      ret = SSL_read(ssl, &buff, BUFF_LEN - 1);
      if(ret > 0) {
        fprintf(stderr, "[message recieved] %d\n", ret);
        fprintf(stdout, "%s", buff);
      }
          }
    fprintf(stderr, "\n");
    close(socket_fd);
    /////////////////////////////////////////////////////
    /// clean up clean up everybody, everywhere....
    /////////////////////////////////////////////////
    freeaddrinfo(my_addrinfo);
  } else {
    send_error("Could not find server\n Exiting...\n");
  }

}



void handle_error(const char* error_message) {
  fprintf(stderr, "%s", error_message);
  fprintf(stderr, "%s\n", strerror(errno));
  exit(EXIT_FAILURE);
}


void send_error(const char* error_message) {
  fprintf(stderr, "%s", error_message);
  exit(EXIT_FAILURE);
}


int connect_to_addr(struct addrinfo* addr, int socket_fd) {
  int ret;

  char* name = (char*)malloc(sizeof(char) * addr->ai_addrlen + 1);
  name[addr->ai_addrlen] = '\0';
 

  if(addr->ai_family == AF_INET) {
    struct sockaddr_in* saddress = (struct sockaddr_in*)addr->ai_addr;
    inet_ntop(addr->ai_family, &(saddress->sin_addr), name, addr->ai_addrlen);
    struct sockaddr_in  server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family      = addr->ai_family;
    server_addr.sin_port        = htons(PORT);
    ret = inet_pton(addr->ai_family, name, &server_addr.sin_addr);
    ret = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));
  } else {

    struct sockaddr_in6* saddress = (struct sockaddr_in6*)addr->ai_addr;
    inet_ntop(addr->ai_family, &(saddress->sin6_addr), name, addr->ai_addrlen);
  

    struct sockaddr_in6  server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family      = AF_INET6;
    server_addr.sin6_port        = htons(PORT);
    ret = inet_pton(AF_INET6, name, &(server_addr.sin6_addr));
    ret = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in6));
  }
  free(name);

  if (ret == -1) {
    fprintf(stderr, "error could not connect to socket \n%s\n", strerror(errno));
    close(socket_fd);
    return ret;
  } else { 
    fprintf(stderr, "\tSuccessfully connected to socket\n");
    return ret;
  }
}



