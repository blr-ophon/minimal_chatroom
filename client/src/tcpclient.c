#include "tcpclient.h"

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("Usage: tcpclient <hostname> <port>\n");
        return -1;
    }

    char hostname_buf[100];
    char port_buf[100];
    strncpy(hostname_buf, argv[1], strlen(argv[1]));
    strncpy(port_buf, argv[2], strlen(argv[2]));
    hostname_buf[99] = '\0';
    port_buf[99] = '\0';

    struct addrinfo hints;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *addresses;
    int rv = getaddrinfo(hostname_buf, port_buf, &hints, &addresses);
    if(rv < 0){
        printf("Hostname can't be reached. DNS failed\n");
        return -1;
    }
    int sockfd = try_connection(addresses);


    freeaddrinfo(addresses);

    close(sockfd);
}

//Traverses an addrinfo linked list, stoping at the first one able to connect
//returns socket file descriptor or -1 if it failed
int try_connection(struct addrinfo *const addresses){
    int sockfd = -1;
    struct addrinfo *p;
    for(p = addresses; p != NULL; p = p->ai_next){
        print_addr(p);

        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(sockfd < 0){
            sockfd = -1;
            perror("socket()");
            continue;
        }
        if(connect(sockfd, p->ai_addr, p->ai_addrlen) < 0){
            sockfd = -1;
            perror("connect()");
            continue;
        }
        break;
    }
    if(sockfd < 0){
        printf("Hostname can't be reached. Connection failed\n");
    }
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
