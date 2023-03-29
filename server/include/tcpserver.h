#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>

typedef struct fd_node{
    int fd;
    struct fd_node *nextNode;
}fdNode;

void fdlist_fd_set(int fd, fdNode *fd_list);

void fdlist_fd_clr(int fd, fdNode *fd_list);

void fdlist_to_fdset(fd_set *ready_fds, fdNode *fd_list);

int fdlist_getmax(fdNode *fd_list);

int try_addresses(struct addrinfo *const addresses);

void print_addr(struct addrinfo *addr);

#endif
