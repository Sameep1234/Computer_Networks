#include"../header.h"

void error_handle(char *msg_to_display, int sock_fd_to_close);

int main(int argc, char *argv[])
{
    return 0;
}

void error_handle(char *msg_to_display, int sock_fd_to_close)
{
    close(sock_fd_to_close);
    perror(msg_to_display);
    exit(EXIT_FAILURE);
}