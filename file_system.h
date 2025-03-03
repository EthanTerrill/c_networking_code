#ifndef FILE_SYSTEM

#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include "LinkedList.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// Abstract repesentation of a unix file containts the names contents and unix 
// time stamp of a given file
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct file_t{
  time_t unix_ts;
  char* file_path;
  char* name;
  char* contents; 
} file_t;



/////////////////////////////////////////////////////////////////////////////////////////
// Abstract 
///////////////////////////////////////////////////////////////////////
typedef struct FileSystem {
  char* file_path;
  char* name;
  LinkedList* files;
  LinkedList* subdirectories;
} FileSystem;


int populate_file_system(char* directory, FileSystem** fs);

int update_file_system(FileSystem* fs);

int update_file(file_t* f);

char* get_file_contents(char* filename);

void print_file_system(FileSystem* fs);

char* search_for_file(char* path, FileSystem* fs);

void clean_file_system(FileSystem** fs);

#endif // !FILE_SYSTEM
