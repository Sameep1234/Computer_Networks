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

struct radio_stn_info_request
{
    uint8_t type;
};

struct site_info
{

    uint8_t type;
    uint8_t site_name_size;
    char site_name[200];
    uint8_t site_desc_size;
    char site_desc[200];
    uint8_t radio_stn_count;
    radio_stn_list[2];
};

struct radio_stn_info
{
    uint8_t radio_stn_number;
    uint8_t radio_stn_name_size;
    char radio_stn_name[200];
    uint32_t multicast_address;
    uint16_t data_port;
    uint16_t info_port;
    uint32_t bit_rate;
};
struct song_info
{

    uint8_t type;
    uint8_t song_name_size;
    char song_name[200];
    uint16_t remaining_time_in_sec;
    uint8_t next_song_name_size;
    char next_song_name[200];
};
