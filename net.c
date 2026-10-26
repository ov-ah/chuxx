#include "net.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

int init_socket()
{
    int host_fd;
    int opt = 1;

    if ((host_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed!");
        return -1;
    }
    if (setsockopt(host_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("Setsockopt failed!");
        close(host_fd);
        return -1;
    }

    return host_fd;
}

int host_listen(int port)
{
    int host_fd = init_socket();
    if (host_fd < 0)
    {
        perror("Socket error!");
        return -1;
    }
    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(host_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed!");
        close(host_fd);
        return -1;
    }

    if (listen(host_fd, 3) < 0)
    {
        perror("Listen failed!");
        close(host_fd);
        return -1;
    }

    return host_fd;
}

int host_accept(int host_fd)
{
    int client_fd;
    if ((client_fd = accept(host_fd, NULL, NULL)) < 0)
    {
        perror("Accept error! Could not connect to client.");
        return -1;
    }
    return client_fd;
}

int client_connect(const char *ip, int port)
{
    int client_fd = init_socket();
    if (client_fd < 0)
    {
        perror("Socket error!");
        return -1;
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    int result = inet_pton(AF_INET, ip, &address.sin_addr);
    if (result != 1)
    {
        if (result == 0)
            fprintf(stderr, "Invalid IPv4 address: %s\n", ip);
        else
            perror("inet_pton");
        close(client_fd);
        return -1;
    }

    if ((connect(client_fd, (struct sockaddr *)&address, sizeof(address))) < 0)
    {
        perror("Connection failed!");
        close(client_fd);
        return -1;
    }

    return client_fd;
}

int send_line(int fd, const char *msg)
{
    size_t length = strlen(msg);
    size_t sent = 0;

    while (sent < length)
    {
        ssize_t n = send(fd, msg + sent, length - sent, MSG_NOSIGNAL);
        if (n < 0)
        {
            perror("Send invalid!");
            return -1;
        }
        sent += (size_t)n;
    }
    if (send(fd, "\n", 1, MSG_NOSIGNAL) < 0)
    {
        perror("Send newline failed!");
        return -1;
    }

    return 0;
}

int recv_line(int fd, char *buf, int cap)
{
    if (buf == NULL || cap < 2)
        return -1;

    int len = 0;

    while (len < cap - 1)
    {
        char ch;
        ssize_t n = read(fd, &ch, 1);

        if (n == 0)
            return 0;
        if (ch == '\r')
            continue;
        if (ch == '\n')
        {
            if (len == 0)
                continue;
            buf[len] = '\0';
            return len;
        }
        buf[len++] = ch;
    }
    fprintf(stderr, "Line too long!\n");
    return -1;
}
