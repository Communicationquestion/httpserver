#pragma
#include<arpa/inet.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include <sys/select.h>

int initserver(int port);
int listenfun(int sockserver);