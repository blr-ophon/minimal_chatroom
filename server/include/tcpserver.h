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

#include "fdlist.h"


fdNode *comm_sock_init(void);

void handle_connections(fdNode *monitored_fds, fd_set ready_fds);

void broadcast_msg(char *msg, fdNode *fdlist, fdNode *source);

int try_addresses(struct addrinfo *const addresses, struct addrinfo *returned);

void print_addr(struct addrinfo *addr);

#endif
