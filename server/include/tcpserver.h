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
#include "sockhandler.h"
#include "commands.h"


fdNode *comm_sock_init(void);

int try_addresses(struct addrinfo *const addresses, struct addrinfo *returned);

void print_addr(struct addrinfo *addr);

#endif
