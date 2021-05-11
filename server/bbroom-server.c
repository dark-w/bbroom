#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "list.h"

#include "thpool.h"

#define __UNUSE__ __attribute__((unused))

#define BUFFER_SIZE 1024

#define ERR_BIND 1
#define ERR_LISTEN 2

/* online list */
static LIST_HEAD(g_id_list);

#define ID_MAX 10

struct person {
    char id[ID_MAX];
    int client_fd;
    struct list_head person_node;
};

#define CMD_HANDLE_RET_CONTINUE 0
#define CMD_HANDLE_RET_OFF 1
#define CMD_HANDLE_RET_GOON 2

/* command 'list' handle */

/* the msg will looks like this:
*  ----------
*
*  dark
*  yuri
*  troy
*  jay
*  wick
*  nick
* 
*  ----------
*/
static int command_list_handle(const struct person *_person)
{
    const char *line = "\n----------\n\n";

    send(_person->client_fd, line, strlen(line), 0);
    struct list_head *itr;
    list_for_each (itr, &g_id_list) {
        struct person *p = container_of(itr, struct person, person_node);

        send(_person->client_fd, p->id, strlen(p->id), 0);
    }
    send(_person->client_fd, line, strlen(line), 0);

    return CMD_HANDLE_RET_CONTINUE;
}

/* online broadcast
*  everybody who onlines will recive a msg from the server:
*  somebody is coming!
*
* offline broadcast
*  everybody who onlines will recive a msg from the server:
*  somebody is leaving!
*/

#define ONLINE_REMIND 0
#define OFFLINE_REMIND 1

static void msg_broadcast_on_off_line(const struct person *_p, int flag)
{
    const char *online_remind_msg_base;
    if (ONLINE_REMIND == flag)
        online_remind_msg_base = " is coming!\n";
    else if (OFFLINE_REMIND == flag)
        online_remind_msg_base = " is leaving!\n";

    char *online_remind_msg =
        (char *)malloc(strlen(online_remind_msg_base) + strlen(_p->id) + 1);

    strcat(online_remind_msg, _p->id);
    online_remind_msg[strlen(_p->id) - 1] = '\0'; /* del the '\n' */
    strcat(online_remind_msg, online_remind_msg_base);

    printf("%s\n", online_remind_msg);

    struct list_head *itr;
    list_for_each (itr, &g_id_list) {
        struct person *p = container_of(itr, struct person, person_node);

        send(p->client_fd, online_remind_msg, strlen(online_remind_msg), 0);
    }

    free(online_remind_msg);
}

/* command 'off' handle */
/* warning: this function will free your memory of pointer p*/
static int command_off_handle(struct person *_p)
{
    msg_broadcast_on_off_line(_p, OFFLINE_REMIND);

    list_del(&_p->person_node);
    free(_p);

    return CMD_HANDLE_RET_OFF;
}

/* the msg will looks like: somebody: hello! */
/* FIXME: there is so many useless operation */
static int msg_broadcast(const struct person *_p, const char *buff)
{
    char *message = (char *)malloc(strlen(buff) + ID_MAX);

    memcpy(message, _p->id, strlen(_p->id));
    message[strlen(_p->id) - 1] = ':'; // '\n' -> ':'
    message[strlen(_p->id)] = ' ';
    memcpy(message + strlen(_p->id) + 1, buff, strlen((char *)buff));
    message[strlen(_p->id) + strlen((char *)buff) + 1] = '\0';

    struct list_head *itr;
    list_for_each (itr, &g_id_list) {
        struct person *person = container_of(itr, struct person, person_node);

        send(person->client_fd, message, strlen(message), 0);
    }

    free(message);
    
    return CMD_HANDLE_RET_CONTINUE;
}

/* FIXME: log file r/w */
static inline void server_log(const char *log_msg)
{
    printf("%s\n", log_msg);
}

/* FIXME: logic optimization */
static void tcp_server_handle(void *args)
{
    int client_fd = *(int *)args;
    uint8_t buff[BUFFER_SIZE];

    int n = recv(client_fd, buff, BUFFER_SIZE, 0);

    /* struct person initialize and put it into online list */
    struct person *person = (struct person *)malloc(sizeof(*person));
    memcpy(person->id, buff, n);
    person->client_fd = client_fd;
    list_add(&person->person_node, &g_id_list);

    const char *log_ok_msg = "login successful\n";
    send(client_fd, log_ok_msg, strlen(log_ok_msg), 0);

    msg_broadcast_on_off_line(person, ONLINE_REMIND);

    for (;;) {
        int n = recv(client_fd, buff, BUFFER_SIZE, 0);

        buff[n] = '\0';

        /* FIXME: when user input a msg which is "xxxx off xxxx", the command_off_handle still be working */

        /* When a stream socket peer has performed an orderly shutdown, the return value will be 0 (the traditional "end-of-file" return). */
        if (!n || strstr((const char *)buff, "off")) {
            __UNUSE__ int ret = command_off_handle(person);
            goto POWER_OFF;
        }

        if (strstr((const char *)buff, "list")) {
            __UNUSE__ int ret = command_list_handle(person);
            continue;
        }

        /* server temp log */
        server_log((const char *)buff);

        /* msg broadcast */
        msg_broadcast(person, (const char *)buff);
    }

POWER_OFF:
    close(client_fd);
}

static int tcp_server_run(const char *ip)
{
    /* thread pool initialize */
    threadpool thpool = thpool_init(1024);

    int server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in si;
    char buff[BUFFER_SIZE];

    memset(&si, 0, sizeof(si));
    memset(buff, 0, sizeof(buff));
    si.sin_family = PF_INET;
    si.sin_addr.s_addr = inet_addr(ip);
    si.sin_port = htons(55559);

    if (-1 == bind(server_socket, (struct sockaddr *)&si, sizeof(si)))
        return ERR_BIND;

    if (-1 == listen(server_socket, 5))
        return ERR_LISTEN;

    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_addr_size = sizeof(client_addr);

    /* FIXME: epoll or select model */
    for (;;) {
        int client_socket = accept(
            server_socket, (struct sockaddr *)&client_addr, &client_addr_size);

        thpool_add_work(thpool, tcp_server_handle, (void *)&client_socket);
    }

    thpool_wait(thpool);
    thpool_destroy(thpool);
    close(server_socket);
    return 0;
}

int main(int argc, char **argv)
{
    return tcp_server_run(argv[1]);
}
