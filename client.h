#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define HEADER_SIZE 256
#define BODY_SIZE 1024 

extern int errno ;

int client(char *file_name, char *host, int port)
{
  FILE *f;

  ssize_t bytes_read;

  size_t bytes = 0;

  struct sockaddr_in serv_addr;

  int errnum, valread;
  int sock = 0;
  unsigned long int stream_size;

  char(*header_ptr)[HEADER_SIZE];
  char buffer[BODY_SIZE] = {0};
  char file_buffer[BODY_SIZE] = {0};
  char stream[BODY_SIZE + HEADER_SIZE] = {0};
  char newLineChar = '\n';
  char *newLinePtr = &newLineChar;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Socket creation error \n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0)
  {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("\nConnection Failed \n");
    return -1;
  }

  // send(sock, file, BODY_SIZE, 0);
  printf("%s", strcpy(file_buffer, file_name));
  send(sock, strcpy(file_buffer, file_name), sizeof(file_buffer), 0);
  printf("a %s", buffer);

  file_buffer[strcspn(file_buffer, newLinePtr)] = '\0';
  f = fopen(file_name, "wb+");
  printf("%s\n", file_name);
  do{
     bytes_read = read(sock, stream, BODY_SIZE + HEADER_SIZE);

     if (bytes_read == -1){

      errnum = errno;
      fprintf(stderr, "Value of errno: %d\n", errno);
      perror("Error printed by perror");
      fprintf(stderr, "The error is: %s\n", strerror(errnum));
    }

    if(bytes_read < (HEADER_SIZE + BODY_SIZE)){
      bytes_read = bytes_read + read(sock, stream + bytes_read, (HEADER_SIZE + BODY_SIZE) - bytes_read);
    }
    // printf("stream read: %s\n", stream);

    header_ptr = &stream;
    stream_size = atol(*header_ptr);

    bytes = bytes + fwrite(stream + HEADER_SIZE, stream_size, 1, f) * stream_size;
    fseek(f, bytes, SEEK_SET);

    printf("Stream size: %ld\n", stream_size);
    printf("Bytes received of stream: %ld\n", bytes);
    printf("Bytes read: %ld\n", bytes_read);
    bzero(stream, (HEADER_SIZE + BODY_SIZE));
    // bytes_read = read(sock, stream, BODY_SIZE + HEADER_SIZE);
  }while (bytes_read > 0);

  printf("vou fechar o socket");

  fclose(f);
  close(sock);

  printf("%s\n", buffer);
  return 0;
}
