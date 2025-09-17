//////////////////////////////////////////////////////////////////////////////////////
// Copyright 2025 Ethan Terrill open source license
//
//
//
//
//
////////////////////////////////////////////////////////////////////////


#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>
#include <openssl/ssl.h>

#include "HTTP_response.h"
#include "HTTP_request_parser.h"


// int get_file2(char* filename, HTTP_response_header* response) {
//   int result = 1;
//   int fd = open(filename, O_RDONLY);
//   off_t size = lseek(fd, 0, SEEK_END);
//   lseek(fd, 0, SEEK_SET);
//
//   response->content_length = size;
//   response->content = (char*)malloc(sizeof(char) * (size + 1));
//   if (response->content == NULL) {
//     fprintf(stderr, "malloc failed\n");
//     return -1;
//   }
//
//   while (result) {
//     result = read(fd, response->content, size);
//     if (result == -1) {
//       if (errno != EINTR && errno != EAGAIN) {
//         fprintf(stderr, "error reading file");
//         free(response->content);
//         response->content=NULL;
//         return -1;
//       }
//     }
//   }
//   response->content[size] = '\0';
//
//   for (int i = 0; i < size; i++) {
//
//     fprintf(stdout, "%x ", response->content[i]);
//   }
//   fprintf(stdout, "%s\n", response->content);
//   return 0;
// }


//////////////////////////////////////////////////////
//
///////////////////////////////////////////////////
static int handle_get_request(HTTP_response_header* response, char* resource) {
  if (resource == NULL) {
    response->content = NULL;
    return -1;
  }

  char* ret = "Hey this isnt a page dude, go away";

  if (!strcmp(resource, "/")) {
    response->content_length = strlen(ret);
    response->content = (char*)malloc(strlen(ret) + 1);
    for (int i = 0; i < strlen(ret) + 1; i++) {
      response->content[i] = ret[i];
    }
    response->content[strlen(ret)] = '\0';
    return 1;
  } else {
    if (resource[0] == '/') {
    } else {
    }
  }

  return -1;
}

HTTP_response_header* get_https_reponse(http_request request) {
  HTTP_response_header* ret;


  ret = (HTTP_response_header*)malloc(sizeof(HTTP_response_header));
  ret->version = 1.1;
  if (request.request_method == GET) {
    handle_get_request(ret, request.path);
  } else {
    fprintf(stdout, "request type is not get %d\n", request.request_method);
  }

  return ret;
}




void send_response(SSL* client_ssl, HTTP_response_header* response) {
  int read;
  int bytes_left = response->content_length;

  char* header = "HTTP/1.0 200 OK\r\n\r\n";
  SSL_write(client_ssl, header, strlen(header));

  if (response->content != NULL) {
    while (bytes_left) {
        read = SSL_write(client_ssl, response->content, bytes_left);
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
  } else {
    fprintf(stdout, "lllslkdfjlsdjkfklsdfjlsdkfjljk\n");
  }
}
