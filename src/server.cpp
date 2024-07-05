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
#define MAX_BYTES 4096

// typedef struct cache_element cache_element;


class cache_element{

public :
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
sem_t semaphore;  // max no of clients lock
pthread_mutex_t lock;  // 0/1 lock for the cache


cache_element* HEAD;  // global head of the linkedlist

int cache_size; // size of cache


void* thread_fn(void *socketNew){
    sem_wait(&semaphore);
    // int p;
    int* p;  

    sem_getvalue(&semaphore, p);  // only p gives error

    printf("semaphore value is : %d\n", *p);

    int *t = (int *)socketNew;   //??
    int socket = *t;    // ??
    int bytes_send_client, len;   // ???

    char * buffer = (char*)calloc(MAX_BYTES, sizeof(char));  // calloc ???

    // bzero
    memset(buffer,0, MAX_BYTES);

    bytes_send_client = recv(socket, buffer, MAX_BYTES, 0);  // receiving the input ???

    while(bytes_send_client > 0){
        len = strlen(buffer);
        if(strstr(buffer, "\r\n\r\n") == NULL){
          bytes_send_client = recv(socket, buffer + len, MAX_BYTES-len, 0); 
        }else{
            break;
        }
    }

    char *tempReq = (char *)malloc(strlen(buffer)*sizeof(char)+1); //mem alloct and ptr for it

    for(int i = 0; i < strlen(buffer); i++){
        tempReq[i] = buffer[i];

    }

    cache_element* temp = find_in_cache(tempReq);
    if(temp != NULL){
        int size = temp->len/sizeof(char);
        int pos = 0;
        char res[MAX_BYTES];
        while(pos < size){
            memset(res, 0, MAX_BYTES);

            for(int i = 0; i < MAX_BYTES; i++){
                res[i] = temp->data[i];
                pos++;
            }

            send(socket, res, MAX_BYTES, 0);

        }
        printf("data retrieved from the cache");
        printf("%s\n\n", res);

    }else if(bytes_send_client > 0 ){
        len = strlen(buffer);
        ParsedRequest *request = ParsedRequest_create();

        if(ParsedRequest_parse(request, buffer, len) < 2){
            printf("cannot parse request, PARSING FAILED");
        }else{
            memset(buffer, 0, MAX_BYTES);

            if(!strcmp(request->method, "GET")){   // if req method is equal to get
                if(request->host && request->path && checkHTTPversion(request->version) == 1){
                    bytes_send_client = handle_request(socket, request, tempReq);
                    if(bytes_send_client == -1){
                        sendErrorMsg(socket, 500);
                    }
                }else{
                    sendErrorMsg(socket, 500);
                }
            }else{
                printf("this req is not GET plz send GET req");
            }
        }
    }else if(bytes_send_client == 0){
        printf("client is DC");
    }   

    shutdown(socket, SHUT_RDWR);

    close(socket);
    free(buffer);
    sem_post(&semaphore);
    sem_getvalue(&semaphore, p);
    printf("semaphore for post value is %d\n", *p);
    free(tempReq);

    return NULL;
}


int main(int argc, char* argv[]){
    int client_socketID, client_len;  //for storing len of address of client and socket id for a new client

    struct sockaddr_in server_Add, client_Add;   // ???   struct or class  , make client addr as just sockaddr??

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

    proxy_socketID = socket(AF_INET, SOCK_STREAM, 0);  // af_inet is for ipv4, sock_Streams is for tcp,
    
    /* proxy_socketID => variable(int)that will store the socket descriptor,socket descriptor is a handle that uniquely identifies the socket in the context of the application.*/
    /*socket-> function call to the socket function, which creates a new socket.*/
    /*AF_INET: the address family for the socket.AF_INET stands for Address Family Internet, it is used for IPv4 . include AF_INET6 for IPv6 */
    /*SOCK_STREAM indicates a stream socket, which means it provides a sequenced, reliable, two-way, connection-based byte stream. */
    /*Specifying 0 means that the default protocol for the specified address family and socket type will be used. here it is TCP*/

    if(proxy_socketID < 0){
        perror("failed to create a SOCKET");
        exit(1);
    }

    int reuse = 1; // global socket is made, which is used

    if(setsockopt(proxy_socketID, SOL_SOCKET, SO_REUSEADDR, (const char* )&reuse, sizeof(reuse)) < 0){  // setsockopt is for setting options on the socket
        perror("setsockopt failed \n");

    }

    memset(&server_Add,0,sizeof(server_Add)); // setting all socket as it hold garbage val by default
    //bzero

    server_Add.sin_family = AF_INET;
    server_Add.sin_port= htons(port);  // converts 16, 32 or 64 bits in network byte order i.e. converts the number into numbes that internet understands
    server_Add.sin_addr.s_addr = INADDR_ANY;   //  the server we will be connecting to at the end, assign any address to it

    int binded = bind(proxy_socketID, (struct sockaddr *)&server_Add, sizeof(server_Add)); 
    // assigns the address specified by server_Add to the socket identified by proxy_socketID. This makes the socket available to accept connections to that address and port.
    //binds a socket to a specific local address and port number {bind -> sys call to bind a socket to local addr } , 
    // { (struct sockaddr *)&server_Add => pointer to a sockaddr structure that contains the address and port to which the socket should be bound. }
 	if(binded<0) {
 		perror("Error on binding! \n");
  		exit(1);
 	}

    printf("binding on port %d", port);

    int listenf = listen(proxy_socketID, MAX_CLIENTS);  // proxy server socket starts to listen

    if(listenf < 0){
        perror("error in listen");
        exit(1);
    }

    int i = 0;

    int connected_socketId[MAX_CLIENTS];

    while(true){
        memset(&client_Add,0,sizeof(client_Add));
        //bzero

        client_len = sizeof(client_Add);

        client_socketID = accept(proxy_socketID, (struct sockaddr *)&client_Add, (socklen_t*)&client_len); // ??
        //accept: This is a system call used to accept an incoming connection request on a listening socket.
        /*The accept function waits for an incoming connection on a listening socket.
        It creates a new socket for the connection and returns a new socket descriptor.
        The client’s address is stored in the client_Add structure, and the length of this address is updated in client_len.
        The original listening socket remains open to accept further connections.
        */
        if(client_socketID < 0){
            printf("not able to connect\n");
            exit(1);
        }else{
            connected_socketId[i] = client_socketID;
        }

        struct sockaddr_in * client_ptr = (struct sockaddr_in *)&client_Add; //making a copy of client add
        struct in_addr ip_Add = client_ptr->sin_addr;  // obtaining ip from client ptr
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip_Add, str, INET6_ADDRSTRLEN);
        printf("client connected with port %d and ip address is %s\n", ntohs(client_Add.sin_port), str);   // all 4 lines above are for conversion of netword network to understandable address

        // pthread_create(&tid[i], NULL, thread_fn, (void *)&connected_socketId[i]);
        // whichever client joined execute thread_fn for it,  and whichever client socket was opened use that socket, and if new client enter make a new socket for it

        i++;

    }

    close(proxy_socketID);
    return 0;
    

}




