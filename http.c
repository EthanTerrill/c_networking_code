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

int main(int argnum, char** argv) {
  char                *default_opt = "content";
  int                 result;
  web_str             client_input;
  int                 output;

  ////////////////////////////////////////////////////////////////
  /// create a File system with a buffer containing the data
  /// on out https server
  /////////////////////////////////////////////////////////////
  FileSystem* fs;
  populate_file_system(default_opt, &fs);

  print_file_system(fs);

  int size = 8000;
  client_input.length = size;
  client_input.str = argv[2];
  output = atoi(argv[1]);

  fprintf(stdout, "\n\n%s\n\n", client_input.str);
  result = size;
  // client_input.str[result - 1] = '\0';
  client_input.length = result;
  result = request_is_safe(client_input);
  if (result == 0) {
    fprintf(stderr, "bad request\n");
  }

  http_request* req = allocate_http_request(0,
                                            0,
                                            client_input);

  HTTP_response_header* response = get_https_reponse(*req);
  response->content = search_for_file(req->path, fs);
  response->content_length = strlen(response->content);

  // char* response = get_file("content/main.html");
  // int bytes_left = strlen(response);
  // send_response(ssl, response);

  write(output, response->content, response->content_length);
  // free(response->content);
  // free_http_request(req);
}
