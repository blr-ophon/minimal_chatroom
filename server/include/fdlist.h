#ifndef FDLIST_H
#define FDLIST_H

#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>

typedef struct fd_node{
    int fd;
    struct sockaddr adr;
    socklen_t addrlen;
    char nick[16];
    struct fd_node *nextNode;
}fdNode;


void fdlist_fd_set(int fd, struct sockaddr adr, socklen_t len, char *nick, fdNode **fd_list);

void fdlist_fd_clr(int fd, fdNode **fd_list);

void fdlist_to_fdset(fd_set *ready_fds, fdNode *fd_list);

int fdlist_getmax(fdNode *fd_list);

void fdlist_free(fdNode *topnode);

#endif
