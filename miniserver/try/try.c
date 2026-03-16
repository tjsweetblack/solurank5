#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include<sys/select.h>
#include <netinet/in.h>
#include <string.h>

void error(char *msg)
{
    write(2, msg, strlen(msg));
}

int main(int argc, char **argv)
{
    if(argc != 2)
        error("invalid arguments");
    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd  == -1)
        error("socket error");
    
    struct sockaddr_in sock_address;
    int port = atoi(argv[1]);
    memset(&sock_address, 0, sizeof(sock_address));
    sock_address.sin_family = AF_INET;
    sock_address.sin_addr.s_addr = htonl(2130706433);
    sock_address.sin_port = htons(port);

    if(bind(serverfd, (const struct sockaddr*)&sock_address, sizeof(sock_address)) != 0)
        error("bind failed");
    if(listen(serverfd, 10) != 0)
        error("failed to litsen");
    error("sucess");

    
}
