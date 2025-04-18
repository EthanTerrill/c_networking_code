//////////////////////////////////////////////////////////////////////////////////////
// Copyright 2025 Ethan Terrill open source license
//
//
////////////////////////////////////////////////////////////////////////
#include "HTTP_request_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "web_str.h"



static inline int is_blank(char input) {
  return input == ' '   ||
    input == '\n'  ||
    input == '\0'  ||
    input == '\r';
}

static char* get_word(char* input) {
  char* ret;
  int i;

  for (i = 0; !is_blank(input[i]); i++ ) {}

  ret = (char*)malloc((i + 1) * sizeof(char));
  for (int j = 0; j < i; j++) {
    ret[j] = input[j];
  }
  ret[i] = '\0';
  return ret;
}


static char* get_line(char* input) {
  char* ret;
  int i;
  for (
    i = 0; (input[i] != '\0') &&
    (input[i] != '\r' || input[i + 1] != '\n'); i++
  ) {}

  ret = (char*)malloc(i * sizeof(char));
  for (int j = 0; j < i; j++) {
    ret[j] = input[j];
  }
  return ret;
}




static char* next_word(char* input) {
  char* ret;
  int i;
  for (i = 0; !is_blank(input[i]); i++) {}

  ret = input + i + 1;
  return ret;
}



static enum http_method get_request_method(web_str request) {
  char* first_word = get_word(request.str);

  enum http_method ret;
  if (!strcmp(first_word, "GET")) {
    ret = GET;
  } else if (!strcmp(first_word, "POST")) {
    ret = POST;
  } else {
    ret = INVALID_METHOD;
  }

  free(first_word);
  return ret;
}

static char* get_resource_path(web_str request) {
  char* second = next_word(request.str);
  char* ret = get_word(second);
  return ret;
}


static char* next_line(char* buff) {
  int i;
  for (
    i = 0;
    (buff[i] != '\0') &&
    (buff[i] != '\r' ||
    buff[i + 1] != '\n');
    i++
  ) {}

  if (buff[i] == '\0') {
    return buff + i;
  }
  return buff + i + 2;
}

static float get_http_version(web_str request) {
  char* third = next_word(request.str);
  if (third[0] == '\0') {
    return -1;
  }
  third = next_word(third);
  if (third[0] == '\0') {
    return -1;
  }
  char* version_header = get_word(third);

  float ret;
  if (strcmp(version_header, "HTTP/0.9")) {
    ret = 0.9;
  } else if (strcmp(version_header, "HTTP/1.0")) {
    ret = 1.0;
  } else if (strcmp(version_header, "HTTP/1.1")) {
    ret = 1.1;
  } else if (strcmp(version_header, "HTTP/2.0")) {
    ret = 2.0;
  } else if (strcmp(version_header, "HTTP/3.0")) {
    ret = 3.0;
  } else {
    ret = -1;
  }

  free(version_header);
  return ret;
}
static char* get_attrib(char* str, const char* attrib) {
  char* curr;
  char* ret;

  curr = next_line(str);
  ret = get_word(curr);
  while (strcmp(ret, attrib)) {
    free(ret);
    curr = next_line(curr);
    if (curr[0] == '\0') {
      fprintf(stderr, "failed to find attribute %s\n", attrib);
      return NULL;
    }
    ret = get_word(curr);
  }

  free(ret);
  return curr;
}



static char* get_host(web_str request) {
  char* curr;
  char* ret;

  curr = get_attrib(request.str, "Host:");
  if (curr == NULL) {
    return NULL;
  }
  curr = next_word(curr);
  ret = get_word(curr);

  return ret;
}


static char* get_user_agent(web_str request) {
  char* curr;
  char* ret;

  curr = get_attrib(request.str, "User-Agent:");
  if (curr == NULL) {
    return NULL;
  }
  curr = next_word(curr);
  ret = get_line(curr);

  return ret;
}


http_request* allocate_http_request(
  int client_fd,
  int client_addr,
  web_str request
) {
  http_request* ret;

  ret = (http_request*)malloc(sizeof(http_request));
  ret->client_fd    = client_fd;
  ret->client_addr  = client_addr;
  ret->request      = request;


  ret->request_method = get_request_method(request);
  ret->path           = get_resource_path(request);
  ret->version        = get_http_version(request);
  ret->host           = get_host(request);
  ret->user_agent     = get_user_agent(request);


  return ret;
}


void free_http_request(http_request *request) {
  if (request != NULL) {
    if (request->request.str != NULL)
      free(request->request.str);
    if (request->path != NULL)
      free(request->path);
    if (request->host != NULL)
      free(request->host);
    if (request->user_agent != NULL)
      free(request->user_agent);
    free(request);
  }
}

