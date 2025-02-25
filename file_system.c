#include "file_system.h"
#include <malloc.h>
#include <stdio.h>
#include <sys/stat.h>

#include "messages.h"

int populate_file_system(char* directory, FileSystem* fs) {
  int ret;
  struct stat sb; 

  message("Populating directory: %s\n", directory);
  fs = (FileSystem*)malloc(sizeof(FileSystem));
  if (fs == NULL) {
    return -1;
  }
  fs->files = LinkedList_allocate();
  fs->subdirectories = LinkedList_allocate(); 
  if(fs->files == NULL || fs->subdirectories == NULL) {
    return -1;
  }
  ret = stat(directory, &sb);

  
  

  return 0;
}
