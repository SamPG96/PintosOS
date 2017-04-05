#include <stdio.h>
#include <syscall.h>


int main (int argc, char *argv[]) {
  int fd;
  //char *buffer;

  char buffer1[10];
  char buffer2[10];

  printf("Opening file: %s\n", argv[1]);
  fd = open(argv[1]);

  printf("Read first two bytes\n");
  read(fd, buffer1, 2);

  printf("Printing bytes\n");
  char* buf = buffer1;
  printf("Byte 1: %c\tByte2: %c\n", (char)*(buf+0), (char)*(buf+1));

  printf("Seek to pos: %i\n", atoi(argv[2]));
  seek(fd, atoi(argv[2]));

  printf("Read next two bytes at seeked position\n");
  read(fd, buffer2, 2);
  char* buf2 = buffer2;
  printf("Byte 1: %c\tByte2: %c\n", (char)*(buf2+0), (char)*(buf2+1));

  printf("Next byte to read is: %i\n", tell(fd));
}
