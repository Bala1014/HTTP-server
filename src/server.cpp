#include <stdio.h>  // for std i/o
#include <string.h> // for string io and functions
#include <time.h> // for time in c++
#include "proxy_parse.h"  // parsing lib MOST IMP
#include <pthread.h> // for thread and locks
#include <semaphore.h>  //semaphone
#include <stdlib.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>  //socket lib
#include <netinet/in.h>  // ipv4 and ipv6 addresses
#include <netdb.h> 
#include <arpa/inet.h>  //internet family for assigning address and ports
#include <unistd.h>
#include <fcntl.h> 
#include <errno.h>


#define MAX_CLIENTS 10

// typedef struct cache_element cache_element;


class cache_element{
    char*  data;   //storing data in char stream
    int len; //length of data
    char* url; //req goes to which url, so that when req comes again i can find it easily with that url

    time_t timeTrack;  // time bounded LRU
    cache_element* next;  // each element of LRU is a node of linked list 
};



cache_element* find_in_cache(char* url){  //list 

}


int add_to_cache(char* data, int size, char* url){  //function to add to cache

}

void remove_from_cache(){  //function to remove from cache

}


int port = 3000;

int proxy_socketID ;   // socket id
pthread_t tid[MAX_CLIENTS];   // threads for each client req,  thread id is stored in them
sem_t semaphore;
pthread_mutex_t lock;


cache_element* HEAD;

int cache_size;


int main(int argc, char* argv[]){
    int client_socketID, client_len;  //for storing len of address of client and socket id for a new client

    class sockaddr server_Add, client_Add;   // ???

    sem_init(&semaphore, 0,MAX_CLIENTS);   //initializing semaphores with min val = 0,  and max value MAX_CLIENTS

    pthread_mutex_init(&lock, NULL);  //initializing pthread lock with address lock and value NULL

    if(argc == 2){     // why argc ??
        port = atoi(argv[1]);
    }else{
        printf("port not given, too few argunments\n");
        exit(1);  // to exit the whole program 
        //why not use cout
        
    }

    printf("listening to req on %d", port);
    

}




