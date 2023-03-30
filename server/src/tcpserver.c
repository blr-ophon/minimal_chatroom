#include "tcpserver.h"

#define PORT "8080"

int main(void){
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *address_list;
    int rv = getaddrinfo(NULL, PORT, &hints, &address_list);
    if(rv < 0){
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }
    int comm_sockfd = try_addresses(address_list);
    if(comm_sockfd < 0){
        printf("Unable to bind address to socket\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(address_list);
    
    fdNode *monitored_fds = NULL;
    fdlist_fd_set(comm_sockfd, &monitored_fds);

    fd_set ready_fds;
    for(;;){
        fdlist_to_fdset(&ready_fds, monitored_fds);
        select(fdlist_getmax(monitored_fds)+1, &ready_fds, 0, 0, 0);

        //new connections
        if(FD_ISSET(comm_sockfd, &ready_fds)){
            struct sockaddr client_addr;
            socklen_t client_addr_len = (socklen_t) sizeof(client_addr);
            int client_sockfd = accept(comm_sockfd, &client_addr, &client_addr_len);
            if(client_sockfd < 0){
                printf("Failed accepting connection\n");
            }
            printf("New client connected\n");
            fdlist_fd_set(client_sockfd, &monitored_fds);
            //TODO: save sockaddrs on fd nodes to monitor users
        }else{
            fdNode *p = monitored_fds->nextNode;
            for(; p != NULL; p = p->nextNode){
                if(FD_ISSET(p->fd, &ready_fds)){
                    char recv_msg_buf[4096];
                    int bytes_recv = recv(p->fd, recv_msg_buf, sizeof(recv_msg_buf), 0);
                    if(bytes_recv < 1){
                        printf("Client connection closed\n");
                        fdlist_fd_clr(p->fd, monitored_fds);
                        continue;
                    }
                    recv_msg_buf[bytes_recv] = '\0';
                    printf("%d bytes received\n", bytes_recv);
                    printf("%s", recv_msg_buf);

                    //send message to all clients
                    broadcast_msg(recv_msg_buf, monitored_fds, p->fd);
                }
            }
        }
    }
    close(comm_sockfd);
}

void broadcast_msg(char *msg, fdNode *fdlist, int senderfd){
    //expects the communication node to be the first on the list
    //and skips it
    fdNode *p = fdlist->nextNode;
    for(; p != NULL; p = p->nextNode){
        if(p->fd == senderfd) continue;
        int bytes_sent = send(p->fd, msg, strlen(msg), 0);
        if(bytes_sent < 0){
            printf("Failed broadcasting message\n");
        }
    }
}

//Traverses an addrinfo linked list, stoping at the first one able to bind
//returns socket file descriptor or -1 if it failed
int try_addresses(struct addrinfo *const addresses){
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
    return sockfd;
}

void print_addr(struct addrinfo *addr){
        char host_buf[100]; 
        char serv_buf[100];
        int rv = getnameinfo(addr->ai_addr, addr->ai_addrlen, host_buf, sizeof(host_buf),
                serv_buf, sizeof(serv_buf), NI_NUMERICHOST);
        if(rv < 0){
            printf("getnameinfo() failed\n");
        }
        printf("%s\n%s\n", host_buf, serv_buf);
}

void fdlist_fd_set(int fd, fdNode **fd_list){
    fdNode *previous_node = NULL;
    fdNode *p;
    for(p = *fd_list; p != NULL; p = p->nextNode){
        previous_node = p;
    }
    p = malloc(sizeof(fdNode));
    p->fd = fd;
    p->nextNode = NULL;

    if(previous_node == NULL){ //empty list
        *fd_list = p;
    }else{
        previous_node->nextNode = p;
    }
}

void fdlist_fd_clr(int fd, fdNode *fd_list){
    fdNode *p;
    fdNode *previous_node = fd_list;
    for(p = fd_list; p->fd != fd; p = p->nextNode){
        if(p == NULL){
            return;
        }
        previous_node = fd_list;
    }
    previous_node->nextNode = p->nextNode;
    free(p);
}

void fdlist_to_fdset(fd_set *ready_fds, fdNode *fd_list){
    fdNode *p;
    for(p = fd_list; p != NULL; p = p->nextNode){
        FD_SET(p->fd, ready_fds);
    }
}

int fdlist_getmax(fdNode *fd_list){
    fdNode *p;
    int max = 0;
    for(p = fd_list; p != NULL; p = p->nextNode){
        if(p->fd >= max){
            max = p->fd;
        }
    }
    return max;
}
