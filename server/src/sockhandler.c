#include "sockhandler.h"

void handle_new_conn(fdNode *monitored_fds){
    struct sockaddr client_addr;
    socklen_t client_addr_len = (socklen_t) sizeof(client_addr);
    int client_sockfd = accept(monitored_fds->fd, &client_addr, &client_addr_len);
    if(client_sockfd < 0){
        printf("Failed accepting connection\n");
    }

    fdlist_fd_set(client_sockfd, client_addr, client_addr_len, "default", &monitored_fds);
}

void handle_connections(fdNode *fdlist, fd_set ready_fds){
    fdNode *previous_node = fdlist;
    fdNode *p = fdlist->nextNode;
    for(; p != NULL; p = p->nextNode){
        if(FD_ISSET(p->fd, &ready_fds)){
            char recv_msg_buf[4096];
            int bytes_recv = recv(p->fd, recv_msg_buf, sizeof(recv_msg_buf), 0);
            if(bytes_recv < 1){
                printf("Client connection closed\n");
                fdlist_fd_clr(p->fd, &fdlist);
                p = previous_node;
                continue;
            }
            recv_msg_buf[bytes_recv] = '\0';
            printf("%d bytes received\n", bytes_recv);
            printf("> %s\n", recv_msg_buf);

            if(is_command(recv_msg_buf)){
                command_handler(p, recv_msg_buf);
            }else{
                //send message to all clients
                broadcast_msg(recv_msg_buf, fdlist, p);
            }
        }
        previous_node = p;
    }
}

void broadcast_msg(char *msg, fdNode *fdlist, fdNode *source){
    //expects the communication node to be the first on the list
    //and skips it
    char origin_str[32];
    int rv = getnameinfo(&source->adr, source->addrlen, origin_str, sizeof(origin_str),
            0, 0, NI_NUMERICHOST);
    if(rv < 0){
        printf("getnameinfo Failed\n");
    }

    char formatted_msg[4096 + 32 + 16 + 5]; //msg + maxaddr + nick + brackets;
    
    //[nickname(ip)] on the start of the message
    sprintf(formatted_msg, "[%s(%s)] ", source->nick, origin_str);

    //append message
    strncat(formatted_msg, msg, strlen(msg));

    //send to all peers
    fdNode *p = fdlist->nextNode;
    for(; p != NULL; p = p->nextNode){
        int bytes_sent = send(p->fd, formatted_msg, strlen(formatted_msg), 0);
        if(bytes_sent < 0){
            printf("Failed broadcasting message\n");
        }
    }
}
