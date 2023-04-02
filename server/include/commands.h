#ifndef COMMANDS_H
#define COMMANDS_H

#include <string.h>

#include "fdlist.h"

int is_command(char *msg);

void command_handler(fdNode *client, char *msg);

void update_nick(fdNode *client, char *nick);

#endif
