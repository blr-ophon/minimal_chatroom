#ifndef SOCKHANDLER_H
#define SOCKHANDLER_H

#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "fdlist.h"
#include "commands.h"

void handle_new_conn(fdNode *monitored_fds);

void handle_connections(fdNode *monitored_fds, fd_set ready_fds);

void broadcast_msg(char *msg, fdNode *fdlist, fdNode *source);

#endif
