//
//  server.c
//  ServerClient2
//
//  Created by Can Xiong on 6/3/15.
//  Copyright (c) 2015 Can Xiong. All rights reserved.
//
/*
	C socket server example, handles multiple clients using threads
 */

#include<stdio.h>
#include<string.h>	//strlen
#include<stdlib.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<pthread.h> //for threading , link with lpthread

#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

//the thread function
void *connection_handler(void *);
static int32_t total = 0;

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
    
    //Create socket
    // SOCK_STREAM (TCP) or SOCK_DGRAM (UDP)
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
    
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
    
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
    
    //Listen
    listen(socket_desc , 3);
    
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    
    
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
        
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
        
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
    
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    
    return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    int32_t num = 0;
    char *message, *client_integer = (char*)&num ;
    
//    //Send some messages to the client
//    message = "Greetings! I am your connection handler\n";
//    write(sock , message , strlen(message));
//    
//    message = "Now type something and i shall repeat what you type \n";
//    write(sock , message , strlen(message));
    
    //Receive a message from client
    while( (read_size = recv(sock , client_integer , sizeof(int32_t) , 0)) > 0 )
    {
        total += ntohl(num);
        printf( "server---> %s <> %d, total: %d\n", client_integer, ntohl(num), total );
        
        int32_t sendint = htonl(total);
        message = (char*)&sendint;
        //Send the message back to client
        write(sock , message , 4);
    }
    
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    
    //Free the socket pointer
    free(socket_desc);
    
    return 0;
}
