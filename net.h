#ifndef NET_H
#define NET_H

#include <stddef.h>

int host_listen(int port);
int host_recieve(int host_fd, char *buffer, size_t capacity);
int client_send(const char *ip, int port, const char *message);
#endif
