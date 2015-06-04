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
    
    printf("**********************************\nOption a: specify a number for addition.\n");
    printf("Option b: get total from server.\n");
    printf("Option c: quit.\n**********************************\n");
    
    //keep communicating with server
    while(1) {
        printf("Please specify actions, a or b? ");
        char reponse;
        scanf("\n%c" , &reponse);
        
        ssize_t n = 0;
        // send action to server
        n = send(sock , &reponse , sizeof(reponse) , 0);
        if( n < 0) {
            puts("Send failed");
            return 1;
        }
        
        switch (reponse) {
            case 'a': {
                printf("---- Addition action: [%c] ----\n", reponse);
                int32_t num = 0;
                printf("Please enter a number: ");
                scanf("%d" , &num);
                // TODO: make sure input is int
                
                //*((int*)integer) = num;
                int32_t sendint = htonl(num);
                integer = (char*)&sendint;
                //Send some data
                n = send(sock , integer , sizeof(int32_t) , 0);
                if( n < 0) {
                    puts("Send failed");
                    break;
                }
                printf("------ Addition is done ------\n\n");
            }
                break;
                
            case 'b': {
                printf("--- Get total action: [%c] ---\n", reponse);
                //Receive a reply from the server
                n = recv(sock , server_reply , sizeof(int32_t) , 0);
                if( n < 0) {
                    puts("recv failed");
                    break;
                }
                printf("-------- Total = [%d] --------\n\n", ntohl(recnum));
            }
                break;
            case 'q': {
                printf("------------ Byebye -----------\n\n");
                n = -1; // Release sock at the end
                break;
            }
            default:
                printf("Wrong Option.\n\n");
                break;
        }
        
        if (n < 0)
            break;
    }
    
    close(sock);
    return 0;
}
