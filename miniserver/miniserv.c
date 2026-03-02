#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

typedef struct s_client
{
    int id;
    char msg[100000];
} t_client;

t_client clients[2048];

fd_set readfds;
fd_set writefds;
fd_set activefds;
int max_fd = 0;
int increment_id = 0;
char buffread[100000];
char buffwrite[100000];

void ft_putstr_fd(char *msg)
{
    write(STDERR_FILENO, msg, strlen(msg));
    exit(1);
}

void ft_broadcast(int sender_fd)
{
    for (int fd = 0; fd <= max_fd; fd++)
    {
        if ((fd != sender_fd) && FD_ISSET(fd, &writefds))
        {
            send(fd, buffwrite, strlen(buffwrite), 0);
        }
    }
}

void ft_disconnect_client(int fd)
{
    sprintf(buffwrite, "server: client %d just left\n", clients[fd].id);
    ft_broadcast(fd);
    FD_CLR(fd, &activefds);
    close(fd);
}

void ft_get_new_client(int serverfd)
{
    int clientfd = accept(serverfd, NULL, NULL);
    if (clientfd == -1)
        return;
    if (clientfd > max_fd)
        max_fd = clientfd;
    FD_SET(clientfd, &activefds);
    clients[clientfd].id = increment_id++;
    memset(&clients[clientfd].msg, 0, sizeof(clients[clientfd].msg));
    sprintf(buffwrite, "server: client %d just arrived\n", clients[clientfd].id);
    ft_broadcast(clientfd);
}

void ft_response(int fd)
{
    int bytes;
    int start; 
    bytes = recv(fd, buffread, sizeof(buffread), 0);
    if (bytes <= 0)
    {
        ft_disconnect_client(fd);
        return;
    };


    start = strlen(clients[fd].msg);
    for (int i = 0; i < bytes && start < sizeof(clients[fd].msg); i++, start++)
    {
        clients[fd].msg[start] = buffread[i];
        if (clients[fd].msg[start] == '\n')
        {
            clients[fd].msg[start] = '\0';
            sprintf(buffwrite, "client %d: %s\n", clients[fd].id, clients[fd].msg);
            ft_broadcast(fd);
            memset(&clients[fd].msg, 0, sizeof(clients[fd].msg));
            start = -1;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
        ft_putstr_fd("Wrong number of arguments\n");

    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1)
        ft_putstr_fd("Fatal error\n");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
    servaddr.sin_port = htons(atoi(argv[1]));

    if ((bind(serverfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
        ft_putstr_fd("Fatal error\n");
    if (listen(serverfd, 10) != 0)
        ft_putstr_fd("Fatal error\n");

    max_fd = serverfd;
    FD_ZERO(&activefds);
    FD_SET(serverfd, &activefds);
    while (1)
    {
        readfds = activefds;
        writefds = activefds;

        if (select(max_fd + 1, &readfds, &writefds, NULL, NULL) < 0)
            continue;
        for (int fd = 0; fd <= max_fd; fd++)
        {
            if (!FD_ISSET(fd, &readfds))
            {
                continue;
            }
            if (fd == serverfd)
            {
                ft_get_new_client(serverfd);
                break;
            }
            else
            {
                ft_response(fd);
                break;
            }
        }
    }
    return (0);
}