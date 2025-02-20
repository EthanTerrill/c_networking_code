#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>

#include "HTTP_response.h"
#include "HTTP_request_parser.h"

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

int get_file2(char* filename, HTTP_response_header* response) {
  int result = 1;
  int fd = open(filename, O_RDONLY);
  off_t size = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);

  response->content_length = size;
  response->content = (char*)malloc(sizeof(char) * (size + 1));
  if (response->content == NULL) {
    fprintf(stderr, "malloc failed\n");
    return -1;
  }

  while (result) {
    result = read(fd, response->content, size);
    if (result == -1) {
      if (errno != EINTR && errno != EAGAIN) {
        fprintf(stderr, "error reading file");
        free(response->content);
        response->content=NULL;
        return -1;
      }
    }
  }
  response->content[size] = '\0';

  for (int i = 0; i < size; i++) {

    fprintf(stdout, "%x ", response->content[i]);
  }
  fprintf(stdout, "%s\n", response->content);
  return 0;
}
//////////////////////////////////////////////////////
//
///////////////////////////////////////////////////
static int handle_get_request(HTTP_response_header* response, char* resource){

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
    //get_file2("content/main.html", response);
    return 1;
  } else {
    if(resource[0] == '/') {
      get_file2(resource + 1, response);
    } else {
      get_file2(resource, response);
    }
  }

  return -1;
}

HTTP_response_header* get_https_reponse(http_request request) {
  HTTP_response_header* ret;


  ret = (HTTP_response_header*)malloc(sizeof(HTTP_response_header));
  
  ret->version = 1.1;
  if (request.request_method == GET) {
    fprintf(stdout, "parsing get request\n");
    fprintf(stdout, "looking for file: %s\n", request.path); 
    handle_get_request(ret, request.path);

    fprintf(stderr, "%s\n", ret->content);
  } else {
    fprintf(stdout, "request type is not get %d\n", request.request_method);
  }

  return ret;
}




void send_response(int client_fd, HTTP_response_header* response) {
  int read;
  int bytes_left = response->content_length;

  char* header = "HTTP/1.0 200 OK\r\n\r\n";
  write(client_fd, header, strlen(header));

  if (response->content != NULL) {
    while (bytes_left) {
        read = write(client_fd, response->content, bytes_left);
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



};



