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

int try_addresses(struct addrinfo *const addresses, struct addrinfo *returned);

void print_addr(struct addrinfo *addr);

//connection handler
void handle_new_conn(fdNode *monitored_fds);

void handle_connections(fdNode *monitored_fds, fd_set ready_fds);

void broadcast_msg(char *msg, fdNode *fdlist, fdNode *source);

//command handler
int is_command(char *msg);

void command_handler(fdNode *client, char *msg);

void update_nick(fdNode *client, char *nick);

#endif
