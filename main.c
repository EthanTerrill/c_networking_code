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
#include <openssl/crypto.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#define DEBUG

#include "web_str.h"
#include "HTTP_request_parser.h"
#include "HTTP_response.h"

#include "file_system.h"

#define HTTPS_PORT 443


void print_ssl_error(int error){
  switch (error) {
    case SSL_ERROR_NONE:
      fprintf(stderr, "no error");
    break;
  case SSL_ERROR_ZERO_RETURN:
      fprintf(stderr, "zero return");
    break; 
  case SSL_ERROR_WANT_READ:
      fprintf(stderr, "want read");
    break; 
  case SSL_ERROR_WANT_CLIENT_HELLO_CB:
      fprintf(stderr, "want client hello cb");
    break; 
  case SSL_ERROR_WANT_CONNECT:
      fprintf(stderr, "want connect");
    break; 
  case SSL_ERROR_WANT_X509_LOOKUP:
      fprintf(stderr, "want x509 lookup");
    break; 
  case SSL_ERROR_WANT_ASYNC:
      fprintf(stderr, "want async");
    break; 
  case SSL_ERROR_WANT_ASYNC_JOB:
      fprintf(stderr, "want async job");
    break;
  case SSL_ERROR_SYSCALL:
      fprintf(stderr, "syscall");
    break;
  case SSL_ERROR_SSL:
    fprintf(stderr, "ERROR SSL");
  }
  fprintf(stderr, "------\n");
}


int main(int argnum, char** argv) {
  int                 socket_fd, new_socket;
  int                 result;
  struct sockaddr_in  my_sockaddr;
  socklen_t           addr_len = sizeof(my_sockaddr);
  int opt = 1;
  char                *default_opt = "content";



SSL_CTX* context = SSL_CTX_new(TLS_method());
    if (context == NULL) {
      fprintf(stderr, "could not create new SSL pointer\n");
      exit(EXIT_FAILURE); 
    }
  web_str client_input;

  SSL_library_init();
  OpenSSL_add_ssl_algorithms();
OpenSSL_add_all_algorithms();
  if (argnum != 2 && argnum != 1) {
    fprintf(stderr, "syntax is: main [path]\n");
    exit(EXIT_FAILURE);
  }

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
  my_sockaddr.sin_port    = htons(HTTPS_PORT);
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
  if(argnum == 2) {
    populate_file_system(argv[1], &fs);
  } else if(argnum == 1) {
    populate_file_system(default_opt, &fs);
  } else {
    exit(EXIT_FAILURE); 
  }


  print_file_system(fs);


  result = SSL_CTX_use_certificate_file(context, "cert.pem", SSL_FILETYPE_PEM);
  if (result != 1) {
    fprintf(stderr, "Could not use cert chain file %d\n", result);
    exit(EXIT_FAILURE); 
  }
  result = SSL_CTX_use_PrivateKey_file(context, "key.pem", SSL_FILETYPE_PEM);
  if (result != 1) {
    // fprintf(stderr, "Could not use private file %d\n", SSL_get_error(ssl, result));
    exit(EXIT_FAILURE); 
  } 

  while (1) {
    new_socket = accept(socket_fd, NULL, NULL);
    if (new_socket == -1) {
      fprintf(stderr, "accept socket \n");
      fprintf(stderr, "%s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

    fprintf(stdout, "accepted socket\n");
     


    SSL* ssl = SSL_new(context);
    if (ssl == NULL) {
      fprintf(stderr, "could not create new SSL pointer %d\n", SSL_get_error(ssl, result));
      exit(EXIT_FAILURE); 
    }
    
    result = SSL_set_fd(ssl, new_socket);
    if (result != 1) {
      fprintf(stderr, "Could not set SSL socket_fd %d\n", SSL_get_error(ssl, result));
      exit(EXIT_FAILURE); 
    }
    else {
      result = SSL_accept(ssl);
      if (result <= 0) {
        print_ssl_error(SSL_get_error(ssl, result));
        fprintf(stderr, "Could not perform SSL handshake %d\n", SSL_get_error(ssl, result));

        ERR_print_errors_fp(stderr);
        // exit(EXIT_FAILURE); 
      }

      fprintf(stdout, "accepted ssl\n");
      int size = 8000;
      client_input.length = size;
      client_input.str = (char*)malloc(sizeof(char) * size);

      result = SSL_read(ssl, client_input.str, size);

      fprintf(stdout, "read ssl\n");
      fprintf(stdout, "%d\n", size);
      if (result == -1) {
        fprintf(stderr, "failed to read from socket\n");
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
      response->content = search_for_file(req->path, fs);
      response->content_length = strlen(response->content);

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
      send_response(ssl, response);

      free(response);
      close(new_socket);
      free_http_request(req);
    }
  }
  fprintf(stderr, "Closing Sockets!\n");
  close(socket_fd);
  exit(EXIT_SUCCESS);
}
