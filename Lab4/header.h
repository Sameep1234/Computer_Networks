#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
#define SERVER_PORT 8080
#define MAX_LINE BUFSIZ
#define SERVER_IP "127.0.0.1"
#define MAX_PENDING 5

struct File_request
{
    uint8_t type;
    uint8_t filename_size;
    char filename[100];
};

struct ACK
{
    uint8_t type;
    uint8_t num_sequences;
    uint16_t sequence_no[100];
};

struct File_info_and_data
{
    uint8_t type;
    uint16_t sequence_number;
    uint8_t filename_size;
    char filename[100];
    uint32_t file_size;
    uint16_t block_size;
    char data[BUFSIZ];
};

struct Data
{
    uint8_t type;
    uint16_t sequence_number;
    uint16_t block_size;
    char data[BUFSIZ];
};

struct File_not_found
{
    uint8_t type;
    uint8_t filename_size;
    char filename[100];
};