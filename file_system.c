#include "file_system.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


char* get_file_contents(char* filename) {
  int result = 1;
  char* buff;
  int fd = open(filename, O_RDONLY);
  off_t size = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);

  buff = (char*)malloc(sizeof(char) * (size + 1));
  if (buff == NULL) {
    fprintf(stderr, "malloc failed\n");
    return NULL;
  }

  while (result) {
    result = read(fd, buff, size);
    if (result == -1) {
      if (errno != EINTR && errno != EAGAIN) {
        fprintf(stderr, "error reading file");
        return NULL;
      }
    }
  }
  buff[size] = '\0';
  return buff;
};

int populate_file_system(char* directory, FileSystem** fs) {
  int ret;
  DIR* dir;
  struct dirent* file;
  struct stat sb; 
  *fs = (FileSystem*)malloc(sizeof(FileSystem));
  if (fs == NULL) {
    return -1;
  }

  (*fs)->name = directory;
  (*fs)->file_path = directory;
  (*fs)->files = LinkedList_allocate();
  (*fs)->subdirectories = LinkedList_allocate(); 
  if((*fs)->files == NULL || (*fs)->subdirectories == NULL) {
    return -1;
  }

  

  ret = stat(directory, &sb); 
  if (ret == -1) {
    return -1;
  }


  if (!S_ISDIR(sb.st_mode)) {
    fprintf(stderr, "Uh oh, this isnt a directory\n");
  }

  dir = opendir(directory);
  if (dir == NULL) {
    return -1;
  }
  
  while((file = readdir(dir))) {
    FileSystem* subdir;
    file_t* fp;
    char* subdirname;
    subdirname = (char*)malloc(strlen(directory) + strlen(file->d_name) + 2);
    for (int i = 0; i < strlen(directory); i++) {
      subdirname[i] = directory[i];
    }
    subdirname[strlen(directory)] = '/';
    for (int i = 0; i < strlen(file->d_name) + 1; i++){
      subdirname[i + strlen(directory) + 1] = file->d_name[i];
    }
    switch (file->d_type) {
      case DT_REG:

        fp = (file_t*)malloc(sizeof(file_t));
        fp->name = subdirname + strlen(directory) + 1;
        fp->contents = "empty";//get_file_contents(subdirname);
        ll_push((*fs)->files, fp);
      break;
      case DT_DIR:
        if( strcmp("..", file->d_name) &&
            strcmp(".", file->d_name) ) {
          populate_file_system(subdirname, &subdir);
          //fprintf(stdout, "reading dir: %s\n", subdirname);
          ll_push((*fs)->subdirectories, subdir);

          int offset = 0;
          for(int i = 0; i < strlen(subdir->name); i++) {
            if (subdir->name[i] == '/') {
              offset = i + 1;     
            }
          }
          subdir->name += offset;
        }
      break; 
    }
  } 
  closedir(dir);
  return 0;
}

static void print_helper(FileSystem* fs, int depth) { 
  LinkedList* ll;
  LinkedList* files;
  file_t* file; 

  ll = fs->subdirectories;
  if(ll->val != NULL || 1) {
      for(int i = 0 ; i < depth - 1; i++) {
        fprintf(stdout, "│ ");
      }
      if (depth == 1) {

        fprintf(stdout, "┌");
      } else {
        fprintf(stdout, "├");
        if (ll->val != NULL) {
          fprintf(stdout, "─┬");
        } else {
          fprintf(stdout, "──");
        }
      }
  }
  fprintf(stdout, ">🖿 %s\n", fs->name);
  

  
  if(ll->val != NULL)
    while(ll != NULL) {
      print_helper((FileSystem*)(ll->val), depth + 1);
      ll = ll->next;
    }


  files = fs->files;
  if (files->val != NULL) {
    while(files != NULL) {

      for(int i = 0 ; i < depth; i++) {
        fprintf(stdout, "│ ");
      }
      
      fprintf(stdout, " ");
      for(int i = 0 ; i < depth; i++) {
        fprintf(stdout, " ");
      }
      file = files->val;
      fprintf(stdout, "%s\n", file->name);
      files = files->next;
    }
  }


  }


void print_file_system(FileSystem *fs) {
  print_helper(fs, 1);  
}



void clean_file_system(FileSystem** fs) {
  LinkedList* ll;
  LinkedList* next; 
  FileSystem* subdir;
  
  ll = (*fs)->subdirectories; 
  if(ll->val != NULL)
    while(ll != NULL) {
      clean_file_system((FileSystem**)&(ll->val));
      subdir = ll->val;
      free(subdir->file_path);
      free(ll->val); 
      next = ll->next;
      free(ll);
      ll->next = next;
    }
  
  ll = (*fs)->subdirectories; 
  if(ll->val != NULL)
    while(ll != NULL) {
      clean_file_system((FileSystem**)&(ll->val));
      free(ll->val); 
      next = ll->next;
      free(ll);
      ll->next = next;
    }


}


