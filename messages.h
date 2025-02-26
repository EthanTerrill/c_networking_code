#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdio.h>
#include <stdarg.h>

void message(const char* message, ...) {
  va_list args;
  va_start(args, message);
  printf(message, args);
  va_end(args);
};
#ifdef DEBUG

void message(const char* message, ...) {
  va_list args;
  va_start(args, message);
  vprintf(message, args);
  va_end(args);

};

#endif // DEBUG

#ifndef DEBUG

#endif // !DEBUG

#endif // !MESSAGES_H
