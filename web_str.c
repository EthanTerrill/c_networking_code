#include "web_str.h"



int request_is_safe(web_str request) {

  if (request.str == NULL) {
    return 0;
  }
  if (request.length <= 0 || request.length > 8000) {
    return 0;
  }
  
  for (int i = 0; i < request.length - 1; i++) {
    if (request.str[i] == '\0') {
      return 0; 
    }
  }

  if (request.str[request.length - 1] != '\0') {
    return 0; 
  }

  return 1;
};
