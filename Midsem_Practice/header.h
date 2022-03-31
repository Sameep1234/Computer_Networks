#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/socket.h>
#include <stdlib.h>

#define SERVER_PORT 8080
#define MAX_LINE BUFSIZ
#define SERVER_IP "127.0.0.1"
#define MAX_PENDING 5