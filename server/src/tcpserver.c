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
    freeaddrinfo(address_list);
    

    fd_set all_fds, afds_cpy;
    FD_ZERO(&all_fds);
    FD_SET(comm_sockfd, &all_fds);

    for(;;){
        //new connections
        struct sockaddr client_addr;
        socklen_t client_addr_len = (socklen_t) sizeof(client_addr);
        int client_sockfd = accept(comm_sockfd, &client_addr, &client_addr_len);

        //receive from peers...
        char recv_msg_buf[4096];
        recv(client_sockfd, recv_msg_buf, sizeof(recv_msg_buf), 0);
        //...and sent to all clients
        
        
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
            perror("socket");
            continue;
        }

        int yes = 1;
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0){
            sockfd = -1;
            perror("setsockopt");
            continue;
        }

        if(bind(sockfd, p->ai_addr, p->ai_addrlen) < 0){
            sockfd = -1;
            perror("bind");
            continue;
        }
        if(listen(sockfd, 20) < 0){
            sockfd = -1;
            perror("listen");
            continue;
        }
        break;
    }
    if(sockfd < 0){
        printf("Failed creating server. Can't bind socket\n");
        return -1;
    }
    
    printf("Server Initialized\n");
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


