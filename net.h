#ifndef NET_H
#define NET_H

int host_listen(int port);
int host_accept(int host_fd);
int client_connect(const char *ip, int port);
int send_line(int fd, const char *msg);
int recv_line(int fd, char *buf, int cap);

#endif
