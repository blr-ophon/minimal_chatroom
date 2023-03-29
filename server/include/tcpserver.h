#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>


int try_addresses(struct addrinfo *const addresses);

void print_addr(struct addrinfo *addr);

#endif
