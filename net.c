#include "net.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080

int init_socket()
{
    int host_fd;
    int opt = 1;

    if ((host_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed!");
        return -1;
    }
    if (setsockopt(host_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
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
    address.sin_port = htons(PORT);

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

int host_recieve(int host_fd, char *buffer, size_t capacity)
{
    int client_fd;
    if (buffer == NULL || capacity < 2)
        return -1;

    if ((client_fd = accept(host_fd, NULL, NULL)) < 0)
    {
        perror("Accept error! Could not connect to client.");
        return -1;
    }

    ssize_t bytes = read(client_fd, buffer, capacity);
    close(client_fd);
    if (bytes < 0)
    {
        perror("Read failure! Recieved no bytes");
        return -1;
    }
    buffer[bytes] = '\0';
    return (int)bytes;
}

int client_send(const char *ip, int port, const char *message)
{
    int client_fd = init_socket();
    if (client_fd < 0)
    {
        perror("Socket error!");
        return -1;
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    int result = inet_pton(AF_INET, ip, &address.sin_addr) <= 0)
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

    size_t length = strlen(message);
    size_t sent = 0;

    while (sent < length)
    {
        ssize_t n = send(client_fd, message + sent, length - sent, 0);
        if (n < 0)
        {
            perror("Send invalid!");
            close(client_fd);
            return -1;
        }
        sent += (size_t)n;
    }

    close(client_fd);
    return 0;
}
