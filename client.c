//
//  main.c
//  ServerClient2
//
//  Created by Can Xiong on 6/3/15.
//  Copyright (c) 2015 Can Xiong. All rights reserved.
//
/*
	C ECHO client example using sockets
 */
#include<stdio.h>	//printf
#include<string.h>	//strlen
#include<sys/socket.h>	//socket
#include<arpa/inet.h>	//inet_addr

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    int32_t recnum = 0;
    char *integer, *server_reply = (char*)&recnum;
    
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
    
    // Loop back ip address for testing on the same machine
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 ); // Port number: 8888
    
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
    
    puts("Connected\n");
    
    printf("Option a: specify a number for addition.\n");
    printf("Option b: get total from server\n");
    
    //keep communicating with server
    while(1)
    {
        printf("Please specify actions, a or b? ");
        char reponse;
        scanf("%c" , &reponse);
        int n = 0;
        
        switch (reponse) {
            case 'a': {
                int32_t num = 0;
                printf("Enter a number for addition: ");
                scanf("%d" , &num);
                // TODO: make sure input is int
                
                //*((int*)integer) = num;
                int32_t sendint = htonl(num);
                integer = (char*)&sendint;
                //Send some data
                n = send(sock , integer , sizeof(int32_t) , 0);
                if( n < 0) {
                    puts("Send failed");
                    return 1;
                }
                break;
            }
            case 'b': {
                //Receive a reply from the server
                n = recv(sock , server_reply , sizeof(int32_t) , 0);
                if( n < 0)
                {
                    puts("recv failed");
                    break;
                }
                
                printf("Server reply : %s <> %d\n", server_reply, ntohl(recnum));
                break;
            }
//            default: {
//                printf("Wrong Option.\n");
//                break;
//            }
        }
        
        if (n < 0)
            break;
    }
    
    close(sock);
    return 0;
}
