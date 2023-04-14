#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>

#include <unistd.h>

#include "chatgui.h"

bool isQuitMessage(char *msg);

int try_connection(struct addrinfo *const addresses);

void print_addr(struct addrinfo *addr);

#endif
