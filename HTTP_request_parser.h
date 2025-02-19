#ifndef HTTP_REQUEST_PARSER
#define HTTP_REQUEST_PARSER
#include "web_str.h"



///////////////////////////////////////////////////////
// enum representation of the method at the 
// beginning of an HTTP request
//////////////////////////////////////////////////
enum http_method{
  INVALID_METHOD, 
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  TRACE,
  CONNECT
};


//////////////////////////////////////////////////////////////////////
// This is a struct containing all useful information
// related to a request made by the client
//
// request is a null terminated string 
/////////////////////////////////////////////////////////////////
typedef struct http_request {
  int           client_fd;
  int           client_addr;
  web_str         request;
  
  enum http_method  request_method;
  char*             path;
  float             version;
  char*             host;
  char*             user_agent;
  char*             accept; 
} http_request;



////////////////////////////////////////////////////////////////////////////////////////
// DESCRPIPTION
//    allocate_http_request should create and populate a valid http_request struct
//    using the following argument:
//
//    request: a web_str type that has been validated by the request_is_safe
//    function to check for potential security issues
//
// RETURN VALUE
//    upon succesfull completion allocate_http_request retrurns a fully
//    allocated and populated http_request struct if allocate_http_request
//    if allocate_http_request fails it will return NULL 
//
//////////////////////////////////////////////////////////////////////////////////
http_request* allocate_http_request(int client_fd, int client_addr, web_str request);

///////////////////////////////////////////////////////////////////////////////////////
// DESCRPIPTION
//    free_http_request() freees an http_request structure and all memory 
//    associated with the structure
//
/////////////////////////////////////////////////////////////////////////////////////
void free_http_request(http_request* request);


#endif // http_request
