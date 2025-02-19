#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>

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
