#include "tcpserver.h"

#define PORT "8080"

int main(void){
    fdNode *monitored_fds = comm_sock_init();
    if(monitored_fds == NULL){
        printf("Failed creating communication socket\n");
        exit(EXIT_FAILURE);
    }

    fd_set ready_fds;
    FD_ZERO(&ready_fds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    for(;;){
        fdlist_to_fdset(&ready_fds, monitored_fds);
        select(fdlist_getmax(monitored_fds)+1, &ready_fds, 0, 0, &timeout);

        //new connections
        if(FD_ISSET(monitored_fds->fd, &ready_fds)){
            handle_new_conn(monitored_fds);

        }else{
            handle_connections(monitored_fds, ready_fds);
        }
    }
    //TODO: free monitored_fds
    fdlist_free(monitored_fds);
}

void handle_new_conn(fdNode *monitored_fds){
    struct sockaddr client_addr;
    socklen_t client_addr_len = (socklen_t) sizeof(client_addr);
    int client_sockfd = accept(monitored_fds->fd, &client_addr, &client_addr_len);
    if(client_sockfd < 0){
        printf("Failed accepting connection\n");
    }

    /*
    char nick_buf[16];
    printf("Client connected\n");
    int bytes_recv = recv(client_sockfd, nick_buf, sizeof(nick_buf), 0);
    nick_buf[bytes_recv-1] = '\0';
    if(bytes_recv < 1){
        printf("Client connection closed\n");
    }else{
    */
    fdlist_fd_set(client_sockfd, client_addr, client_addr_len, "default", &monitored_fds);
}

void command_handler(char *msg){
    if(msg[0] != '/'){
        return;
    }
}

void update_nick(fdNode *client, char *nick){
    strncpy(client->nick, nick, strlen(nick));
}

fdNode *comm_sock_init(void){
    struct addrinfo *address_list;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rv = getaddrinfo(NULL, PORT, &hints, &address_list);
    if(rv < 0){
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        return NULL;
    }

    struct addrinfo valid_addr;
    int comm_sockfd = try_addresses(address_list, &valid_addr);
    if(comm_sockfd < 0){
        printf("Unable to bind address to socket\n");
        return NULL;
    }
    freeaddrinfo(address_list);

    fdNode *monitored_fds = NULL;
    fdlist_fd_set(comm_sockfd, *valid_addr.ai_addr, valid_addr.ai_addrlen, " ", &monitored_fds);

    return monitored_fds;
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
            printf("> %s", recv_msg_buf);

            //send message to all clients
            broadcast_msg(recv_msg_buf, fdlist, p);
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
    sprintf(formatted_msg, "[%s(%s)] ", source->nick, origin_str);
    //strncat(formatted_msg, origin_str, strlen(origin_str));

    strncat(formatted_msg, msg, strlen(msg));
    fdNode *p = fdlist->nextNode;
    for(; p != NULL; p = p->nextNode){
        int bytes_sent = send(p->fd, formatted_msg, strlen(formatted_msg), 0);
        if(bytes_sent < 0){
            printf("Failed broadcasting message\n");
        }
    }
}

//Traverses an addrinfo linked list, stoping at the first one able to bind
//returns socket file descriptor or -1 if it failed
int try_addresses(struct addrinfo *const addresses, struct addrinfo *returned){
    int sockfd = -1;
    struct addrinfo *p;
    for(p = addresses; p != NULL; p = p->ai_next){
        printf("Trying:\n");
        print_addr(p);

        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(sockfd < 0){
            sockfd = -1;
            printf("socket() failed\n");
            perror("socket");
            continue;
        }

        int yes = 1;
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0){
            sockfd = -1;
            printf("setsockopt() failed\n");
            perror("setsockopt");
            continue;
        }

        if(bind(sockfd, p->ai_addr, p->ai_addrlen) < 0){
            sockfd = -1;
            printf("bind() failed\n");
            perror("bind");
            continue;
        }
        if(listen(sockfd, 20) < 0){
            sockfd = -1;
            printf("listen() failed\n");
            perror("listen");
            continue;
        }
        break;
    }
    if(sockfd < 0){
        return -1;
    }

    printf("Address bind successful\n");
    memcpy(returned, p, sizeof(struct addrinfo));
    return sockfd;
}

void print_addr(struct addrinfo *addr){
        char host_buf[100]; 
        char serv_buf[100];
        int rv = getnameinfo(addr->ai_addr, addr->ai_addrlen, host_buf, sizeof(host_buf),
                serv_buf, sizeof(serv_buf), NI_NUMERICHOST | NI_NUMERICSERV);
        if(rv < 0){
            printf("getnameinfo() failed\n");
        }
        printf("%s\n%s\n", host_buf, serv_buf);
}
