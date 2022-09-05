#pragma
#include <unistd.h>
#include <cstddef>
#include <sys/select.h>
#define ISspace(x) isspace((int)(x))

int sendbuffer(int fd, char* buf);
bool accept_request(int client);
int get_line(int sock, char* buf, int size);