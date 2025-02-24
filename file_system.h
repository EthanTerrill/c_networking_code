#include <time.h>
#include <sys/stat.h>
#include "LinkedList.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// Abstract repesentation of a unix file containts the names contents and unix 
// time stamp of a given file
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct file{
  time_t unix_ts;
  char* name;
  char* contents; 
} file;

typedef struct FileSystem {
  LinkedList* files;
  LinkedList* subdirectories;
} FileSystem;


int populate_file_system(char* directory, FileSystem* fs);

int update_file_system(FileSystem* fs);

int update_file(file* f);
