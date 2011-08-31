
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <time.h>

#define BUFSIZE (65536+100)
void SendStrCmd(int fd, char *buf);
unsigned char* ReadResp(int fd);
int InitConn(int speed);
void CloseConn(int fd);