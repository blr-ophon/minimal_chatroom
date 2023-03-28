#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <unistd.h>

int try_connection(struct addrinfo *const addresses);

void print_addr(struct addrinfo *addr);

#endif
