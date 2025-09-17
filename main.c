//////////////////////////////////////////////////////////////////////////////////////
// Copyright 2025 Ethan Terrill open source license
//
//
//
//
//
////////////////////////////////////////////////////////////////////////



// ============================ Inlcudes ============================== //
#include <arpa/inet.h>
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


#include "web_str.h"
#include "HTTP_request_parser.h"
#include "HTTP_response.h"
#include "file_system.h"



// ========================== Macros ================================= //
#define HTTPS_PORT 4442


void print_ssl_error(int err);

void OpenSSL_init(SSL_CTX* ctx, SSL);


int main(int argnum, char** argv) {
  int                 socket_fd, new_socket;
  int                 result;
  struct sockaddr_in6  my_sockaddr;
  socklen_t           addr_len = sizeof(my_sockaddr);
  int opt = 1;
  char                *default_opt = "content";
  web_str client_input;



  ////////////////////////////////////////////////////////////////
  /// generate SSL context and initialize the OpenSSL
  /// library
  /////////////////////////////////////////////////////////////
  SSL_CTX* context = SSL_CTX_new(TLS_method());
  if (context == NULL) {
    fprintf(stderr, "could not create new SSL pointer\n");
    SSL_CTX_free(context);
    exit(EXIT_FAILURE);
  }


  SSL_library_init();
  OpenSSL_add_ssl_algorithms();
  OpenSSL_add_all_algorithms();
  if (argnum != 2 && argnum != 1) {
    fprintf(stderr, "syntax is: main [path]\n");
    SSL_CTX_free(context);
    exit(EXIT_FAILURE);
  }
  ////////////////////////////////////////////////////////////////
  /// Finish setting up OpenSSL
  /////////////////////////////////////////////////////////////
  result = SSL_CTX_use_certificate_file(context, "cert.pem", SSL_FILETYPE_PEM);
  if (result != 1) {
    fprintf(stderr, "Could not use cert chain file %d\n", result);
    SSL_CTX_free(context);
    exit(EXIT_FAILURE);
  }
  result = SSL_CTX_use_PrivateKey_file(context, "key.pem", SSL_FILETYPE_PEM);
  if (result != 1) {
    SSL_CTX_free(context);
    exit(EXIT_FAILURE);
  }

  ////////////////////////////////////////////////////////////////
  /// create socket for contact with clients
  ///
  /////////////////////////////////////////////////////////////
  socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    fprintf(stderr, "Error failed to get socket file descriptor\n");
    fprintf(stderr, "%s\n", strerror(errno));
    SSL_CTX_free(context);
    exit(EXIT_FAILURE);
  }

  //////////////////////////////////////////////////////////////
  /// Set the socket_fd to use the specified options
  ///////////////////////////////////////////////////////////
  if (
    setsockopt(socket_fd,
                 SOL_SOCKET,
                 SO_REUSEADDR | SO_REUSEPORT,
                 &opt, sizeof(opt)
                 )
  ) {
    fprintf(stderr, "Error failed to set socket options\n");
    fprintf(stderr, "%s\n", strerror(errno));
    SSL_CTX_free(context);
    exit(EXIT_FAILURE);
  }


  ////////////////////////////////////////////////////////////////
  /// bind socket and listen for connections over ipv6
  ///
  /////////////////////////////////////////////////////////////
  memset((void*)&my_sockaddr, 0, sizeof(my_sockaddr));
  my_sockaddr.sin6_family  = AF_INET6;
  my_sockaddr.sin6_port    = htons(HTTPS_PORT);
  my_sockaddr.sin6_addr = in6addr_any;

  result = bind(socket_fd, (struct sockaddr*)&my_sockaddr, addr_len);
  if (result == -1) {
    fprintf(stderr, "Error failed to bind socket\n");
    fprintf(stderr, "%s\n", strerror(errno));
    SSL_CTX_free(context);
    exit(EXIT_FAILURE);
  }

  result = listen(socket_fd, 3);
  if (result == -1) {
    fprintf(stderr, "Error failed to mark socket for connection mode\n");
    fprintf(stderr, "%s\n", strerror(errno));
    SSL_CTX_free(context);
    exit(EXIT_FAILURE);
  }


  ////////////////////////////////////////////////////////////////
  /// create a File system with a buffer containing the data
  /// on out https server
  /////////////////////////////////////////////////////////////
  FileSystem* fs;
  if (argnum == 2) {
    populate_file_system(argv[1], &fs);
  } else if (argnum == 1) {
    populate_file_system(default_opt, &fs);
  } else {
    exit(EXIT_FAILURE);
  }


  print_file_system(fs);



  while (1) {
    struct sockaddr_in6 sa;
    size_t sl = sizeof(sa);
    memset((void*)&sa, 0, sizeof(sa));
    new_socket = accept(socket_fd, (struct sockaddr*)&sa, (socklen_t*)&sl);
    if (new_socket == -1) {
      fprintf(stderr, "accept socket \n");
      fprintf(stderr, "%s\n", strerror(errno));
      SSL_CTX_free(context);
      close(new_socket);
    }
    fprintf(stdout, "accepted socket\n");


    SSL* ssl = SSL_new(context);
    if (ssl == NULL) {
      fprintf(stderr, "could not create new SSL pointer %d\n",
              SSL_get_error(ssl, result));
    }


    result = SSL_set_fd(ssl, new_socket);
    if (result != 1) {
      fprintf(stderr, "Could not set SSL socket_fd %d\n",
              SSL_get_error(ssl, result));
    }
    result = SSL_accept(ssl);
    if (result <= 0) {
      print_ssl_error(SSL_get_error(ssl, result));
      fprintf(stderr, "Could not perform SSL handshake %d\n",
              SSL_get_error(ssl, result));
      ERR_print_errors_fp(stderr);
    } else {
      fprintf(stdout, "accepted ssl\n");
      int size = 8000;
      client_input.length = size;
      client_input.str = (char*)malloc(sizeof(char) * size);

      result = SSL_read(ssl, client_input.str, size);

      fprintf(stdout, "read ssl\n");
      fprintf(stdout, "%d\n", size);
      if (result == -1) {
        fprintf(stderr, "failed to read from socket\n");
        close(new_socket);
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


      http_request* req =
        allocate_http_request(new_socket,
                              my_sockaddr.sin6_addr.s6_addr[0],
                              client_input);

      HTTP_response_header* response = get_https_reponse(*req);
      response->content = search_for_file(req->path, fs);
      response->content_length = strlen(response->content);

      fprintf(stdout, "\n-------------------------------------------\n");
      fprintf(stdout, "[message recieved]\n");

      char ipv_name[INET6_ADDRSTRLEN];
      inet_ntop(AF_INET6, &sa.sin6_addr, ipv_name, INET6_ADDRSTRLEN);



      fprintf(stdout, "client addr: [%s]\n", ipv_name);
      fprintf(stdout, "%s\n\n\n", client_input.str);
      fprintf(stdout, "[Sending repsonse]\n");
      fprintf(stdout, "[response sent]\n");
      fprintf(stdout, "\n-------------------------------------------\n");

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


void print_ssl_error(int error) {
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
