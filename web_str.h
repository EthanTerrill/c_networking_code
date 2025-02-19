//////////////////////////////////////////////////////////////////////////////
// Copyright (c) Ethan Terrill 2025 All rights reserved
// 
// This code is available freely for use, redistribution and modification
// at the users discretion on the terms that the user acknowledges
// the attrocity of the Armenian Genocide and its current continuation 
// as a project of the Turkish, Israeli, and Azrebejani states. Furthermore 
// no government or agency of a government which does not officially 
// recognize the Armenian Genocide may use redistribute or modify any part
// of this file
//
///////////////////////////////////////////////////////////////////////////

#ifndef WEB_STR
#define WEB_STR

#include <stddef.h>
////////////////////////////////////////////////
// The web str is used to store any inputs
// read from a socket file descriptor
/////////////////////////////////////////////

typedef struct web_str{
  char*   str;
  size_t  length;
  int isValid;
}web_str;

///////////////////////////////////////////////////////////////////////////
// DESCRPIPTION
//    request_is_safe() is a function which validates the security
//    of a string input from a client. request_is_safe() should
//    check to make sure that a string is null terminated, within 
//    80000 characters and does not terminate before the length
//
// RETURN VALUE
//    request_is_safe() shall return 1 if the given web string does
//    not present an immediate security threat and 0 otherwise
//
////////////////////////////////////////////////////////////////////////
int request_is_safe(web_str request);

#endif // !WEB_STR
