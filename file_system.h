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



/////////////////////////////////////////////////////////////////////////////
// Abstract representation of a unix file system
//
///////////////////////////////////////////////////////////////////////
typedef struct FileSystem {
  time_t unix_ts;
  char* file_path;
  char* name;
  LinkedList* files;
  LinkedList* subdirectories;
} FileSystem;


/////////////////////////////////////////////////////////////////////
//  DESCRIPTION:
//      populate_file_system creates a fully pupolated
//      file system pointer contaning all files and
//      subdirectories in a given directory
//
//  ARGUMENTS:
//      directory: the name of the path to be copied
//      
//      fs: the address of a FileSystem pointer
//          returned
//  RETURNS:
//        0 on success, 1 on failure
//////////////////////////////////////////////////////////
int populate_file_system(char* directory, FileSystem** fs);


/////////////////////////////////////////////////////////////////
// DESCRIPTION:
//    update_file_system takes a FileSystem pointer and
//    checks to see if that pointer has been updated
// 
// ARGUMENTS:
//    update_file_system only takes in the address of the 
//    pointer to be updated
//  
//  RETURNS
//    0 on success 1 on failure
//////////////////////////////////////////////////////////
int update_file_system(FileSystem** fs);


////////////////////////////////////////////////////////////
/// DESCRIPTION:
///   update_file takes a pointer to a file type struct 
///   and updates the file if it has been edited since
///   being loaded into memory
/// 
/// ARGUMENTS:
///   file_t: A file_t struct to be updated if necassary
//////////////////////////////////////////////////////////
int update_file(file_t* f);


/////////////////////////////////////////////////////////
/// DESCRIPTION: 
///   takes as an argument a 
///////////////////////////////////////////////////////
char* get_file_contents(char* filename);

void print_file_system(FileSystem* fs);

char* search_for_file(char* path, FileSystem* fs);

void clean_file_system(FileSystem** fs);

#endif // !FILE_SYSTEM
