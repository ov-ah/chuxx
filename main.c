#include "net.h"
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc >= 2 && strcmp(argv[1], "listen") == 0)
    {
        int host_fd = host_listen(8080);
        if (host_fd < 0)
            return 1;

        printf("Listening on port 8080\n");

        char buffer[BUF_SIZE];
        int n = host_recieve(host_fd, buffer, sizeof(buffer));
        if (n < 0)
            return 1;

        printf("Recieved %d bytes: %s\n", n, buffer);
        return 0;
    }
    if (argc >= 4 && strcmp(argv[1], "send") == 0)
    {
        if (client_send(argv[2], 8080, argv[3]) < 0)
            return 1;

        printf("Sent.\n");
        return 0;
    }
    fprintf(stderr, "Usage:\n  %s listen\n  %s send <ip> <message>\n", argv[0], argv[0]);
    return 1;
}
