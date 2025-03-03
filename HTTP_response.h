#ifndef HTTTP_RESPONSE
#define HTTTP_RESPONSE

#define CRLF \n\r 


#include <openssl/ssl.h>
#include "HTTP_request_parser.h"


///////////////////////////////////////////////////////////
// This is a struct containing all of the useful
// information related to the HTTP  
///////////////////////////////////////////////////////
typedef struct HTTP_response_header{
  char* response;

  //////////////////////////////////////
  // mandatory portion of response
  // HTTP_response_header
  ////////////////////////////////////
  float version;
  int   status_code;
  char* status_phrase;
  
  /////////////////////////////////////////
  // otpional information
  //////////////////////////////////////
  int time;
  int content_length;
  char* content_type;

  ///////////////////////////////////////////
  // this is the main portion of the http 
  // request and contains the requested 
  // content
  /////////////////////////////////////////
  char* content;
}HTTP_response_header;


///////////////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION
//    get_file()  is used to get the contents of a file usign the following arguments:
//
//    filename: a null terminated string with the given filename
// 
// RETURNS:
//    upon succesfully reading a file get_file will return a null terminated 
//    string containinf the contents of the given file. on failure
//    get_file() returns NULL
//
////////////////////////////////////////////////////////////////////////////////////
char* get_file(char* filename);



///////////////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION
//    get_https_reponse() takes as an input a valid http_request and in response 
//    builds a valid https response and fetched any necassary content
////////////////////////////////////////////////////////////////////////////////////
HTTP_response_header* get_https_reponse(http_request request);



///////////////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION
//    send_response() converts the given response header structure into a valid
//    response header, fetches any necassary content and sends it to the 
//    proper client
////////////////////////////////////////////////////////////////////////////////////
void send_response(SSL* ssl, HTTP_response_header* response);


///////////////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION
//    frees memory associated with the response header
////////////////////////////////////////////////////////////////////////////////////
void free_http_response(HTTP_response_header* response);

#endif // HTTTP_RESPONSE
