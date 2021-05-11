#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 512
#define BZERO(addr, size) memset(addr, 0, size)

#define __UNUSE__                                                              \
    __attribute__((unused)) // FIXME: it should to be moved i think.

void *recv_handle(void *pargs)
{
    int client_socket = *(int *)pargs;
    char recv_buf[BUFFER_SIZE];

    while (1) {
        int n = recv(client_socket, recv_buf, BUFFER_SIZE, 0);
        if (!n)
            goto CLOSE;

        recv_buf[n] = '\0';
        printf("%s", recv_buf);
    }

CLOSE:
    close(client_socket);

    return 0;
}

int main(int argc, char **argv)
{
    int client_socket = socket(PF_INET, SOCK_STREAM, 0);

    char send_buf[BUFFER_SIZE];

    struct sockaddr_in server_addr;

    BZERO(send_buf, sizeof(send_buf));
    BZERO(&server_addr, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = inet_addr("120.27.245.242");
    // server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(55559);

    connect(client_socket, (struct sockaddr *)&server_addr,
            sizeof(server_addr));

    pthread_t tid;
    pthread_create(&tid, NULL, recv_handle, &client_socket);
    pthread_detach(tid);

    __UNUSE__ int ret = mkfifo("./client-pipe", 0666);

    // FIXME: the mode of open should be 'READ ONLY',
    //        but it can't work, open both for read and write
    //        is ok.
    int in_fd = open("./client-pipe", O_RDWR);

    while (1) {
        // __UNUSE__ char *dmy = fgets(send_buf, BUFFER_SIZE, stdin);
        __UNUSE__ size_t count = read(in_fd, send_buf, BUFFER_SIZE);
        if (count <= 0)
            goto CLOSE;

        send_buf[count] = '\0';

        int n = send(client_socket, send_buf, strlen(send_buf), 0);
        if (n <= 0)
            goto CLOSE;

        memset(send_buf, 0, count);
    }

CLOSE:
    close(client_socket);
    close(in_fd);
    return 0;
}
