#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef struct fd_node{
    int fd;
    struct sockaddr adr;
    socklen_t addrlen;
    struct fd_node *nextNode;
}fdNode;

void fdlist_fd_set(int fd, struct sockaddr adr, socklen_t len, fdNode **fd_list);

void fdlist_fd_clr(int fd, fdNode *fd_list, fd_set *fdset);

void fdlist_to_fdset(fd_set *ready_fds, fdNode *fd_list);

int fdlist_getmax(fdNode *fd_list);

void broadcast_msg(char *msg, fdNode *fdlist, fdNode *source);

int try_addresses(struct addrinfo *const addresses, struct addrinfo *returned);

void print_addr(struct addrinfo *addr);

#endif
