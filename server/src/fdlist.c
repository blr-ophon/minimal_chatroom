#include "fdlist.h"

void fdlist_fd_set(int fd, struct sockaddr adr, socklen_t len, char *nick, fdNode **fd_list){
    fdNode *previous_node = NULL;
    fdNode *p;
    for(p = *fd_list; p != NULL; p = p->nextNode){
        previous_node = p;
    }
    p = malloc(sizeof(fdNode));
    p->fd = fd;
    p->adr = adr;
    p->addrlen = len;
    p->nick = nick;
    p->nextNode = NULL;

    if(previous_node == NULL){ //empty list
        *fd_list = p;
    }else{
        previous_node->nextNode = p;
    }
}

void fdlist_fd_clr(int fd, fdNode **fd_list){
    if((*fd_list)->fd == fd){ //top of the list
        fdNode *temp = (*fd_list)->nextNode;
        free(*fd_list);
        *fd_list = temp;
        return;
    }
    fdNode *p = *fd_list;
    fdNode *previous_node = p;
    for(; p->fd != fd; p = p->nextNode){
        if(p == NULL){
            return;
        }
        previous_node = p;
    }
    previous_node->nextNode = p->nextNode;
    free(p);
}

void fdlist_to_fdset(fd_set *ready_fds, fdNode *fd_list){
    FD_ZERO(ready_fds);
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

void fdlist_free(fdNode *topnode){
    fdNode *p, *temp;
    for(p = topnode; p != NULL; p = temp){
        temp = p->nextNode;
        close(p->fd);
        free(p);
    }
}

