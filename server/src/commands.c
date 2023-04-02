#include "commands.h"

int is_command(char *msg){
    return msg[0] == '/';
}

//returns 1 if message should not be broadcasted
void command_handler(fdNode *client, char *msg){
    //commands 
    if(strstr(msg, "/nick ") == &msg[0]){
        update_nick(client, &msg[5]); //sends address of whitespace after /nick
    }
}

void update_nick(fdNode *client, char *nick){
    int nick_len = strlen(nick);
    if(nick_len > 16) nick_len = 16;
    if(nick_len > 0){
        for(int i = 1; i < nick_len; i ++){
            //TODO: test for other invalid characters
            if(nick[i] == '\n'){
                nick[i] = '\0';
            }
        }
        //gets whatever is after whitespace
        strncpy(client->nick, &nick[1], nick_len);
    }
}
