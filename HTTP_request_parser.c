#include "HTTP_request_parser.h"
#include "web_str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline int is_blank(char input){
  return input == ' '   || 
    input == '\n'  ||
    input == '\0'  ||
    input == '\r';
}
static char* get_word(char* input){
  char* ret;
  int i;
  for(i = 0; !is_blank(input[i]); i++ ) {}
  
  ret = (char*)malloc(i * sizeof(char));
  for (int j = 0; j < i; j++) {
    ret[j] = input[j];  
  }
  return ret;
}
static char* next_word(char* input){
  char* ret;
  int i;
  for(i = 0; !is_blank(input[i]); i++) {}
  
  ret = input + i + 1;
  return ret;
}
static enum http_method get_request_method(web_str request) {
 char* first_word = get_word(request.str);

  if (strcmp(first_word, "GET")) {
    return GET; 
  } else if(strcmp(first_word, "POST")) {
    return POST;
  } else {   
    return INVALID_METHOD;
  }
}

static char* get_resource_path(web_str request){
  char* second = next_word(request.str);
  char* ret = get_word(second);

  fprintf(stdout, "RESOURCE PATH [%s]\n", ret);
  return ret; 
}
static char* next_line(char* buff){
  int i;
  for (i = 0; (buff[i] != '\0') && (buff[i] != '\r' || buff[i + 1] != '\n'); i++) {
  
  }

  if(buff[i] == '\0') {
    return buff + i;
  }
  return buff + i + 2;
}

static float get_http_version(web_str request) {
  char* third = next_word(request.str);
  third = next_word(third);
  char* version_header = get_word(third);
  fprintf(stdout, "HTTP VERSION [%s]\n", version_header);

  if(strcmp(third, "HTTP/0.9")) {
    return 0.9;
  } else if (strcmp(third, "HTTP/1.0")) {
    return 1.0;
  } else if (strcmp(third, "HTTP/1.1")) {
    return 1.1;
  } else if (strcmp(third, "HTTP/2.0")) {
    return 2.0;
  } else if (strcmp(third, "HTTP/3.0")){
    return 3.0;
  } else {
    return -1;
  }
}

static char* get_host(web_str request){
  char* curr = next_line(request.str);
  curr = next_word(curr);

  char* ret = get_word(curr);
  fprintf(stdout, "host is [%s]\n",ret);
  return ret;
}


http_request* allocate_http_request(int client_fd, int client_addr, web_str request) {
  http_request* ret;
  
  ret = (http_request*)malloc(sizeof(http_request));
  ret->client_fd    = client_fd;
  ret->client_addr  = client_addr; 
  ret->request      = request;


  ret->request_method = get_request_method(request);
  ret->path           = get_resource_path(request);
  ret->version        = get_http_version(request);
  ret->host           = get_host(request);

  return ret;


};
