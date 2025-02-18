#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFF_LEN 2048
void handle_error(const char* error_message){ 
  fprintf(stderr, "%s", error_message);
  fprintf(stderr, "%s\n", strerror(errno));
  exit(EXIT_FAILURE);
}


int main() {
  int                 socket_fd, ret, status, bytes_left;
  struct sockaddr_in  server_addr;
  const char*         message = "Hello from the client!\n";
  char                buff[BUFF_LEN] = {0};
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_fd == -1) {
    handle_error("Error failed to call socket function\n");
  }
  
  server_addr.sin_family      = AF_INET;
  server_addr.sin_port        = htons(PORT);
  
  ret = inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
  if (ret != 1) {
    handle_error("could not resolve server address\n");
  }

  status = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(status == -1) {
    handle_error("Could not establish connection to server\n");
  }
 
  bytes_left = strlen(message);
  while (bytes_left) {
    
    ret = write(socket_fd, message, bytes_left);
    if (ret == -1) {
      if(errno != EAGAIN && errno != EINTR) {
        close(socket_fd);
        handle_error("failed to send message to server\n");
      }
    } else {
      bytes_left -= ret;
    }
  }

  ret = read(socket_fd, &buff, BUFF_LEN - 1);
  fprintf(stdout, "[message recieved]\n%s", buff);



  close(socket_fd);
  exit(EXIT_SUCCESS);

}
