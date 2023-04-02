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
