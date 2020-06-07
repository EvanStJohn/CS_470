#include <iostream>
#include <stdio.h>  
#include <string.h>   //strlen 
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  

using namespace std;

void initSocket();
void checkTickets();
     
#define TRUE   1  
#define FALSE  0  
#define PORT 8888  
     
int main(int argc , char *argv[])   
{    
    int client_socket[30]; 
    int max_clients = 30;     
    struct sockaddr_in address;   
         
    char buffer[1025];  //data buffer of 1K  
         
    //set of socket descriptors  
    fd_set readfds;   
         
    //a message  
    char *message = (char*)"ECHO Daemon v1.0 \r\n";   
     
    //initialise all client_socket[] to 0 so not checked  
    for (int i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
         
    //create a master socket  
    int master_socket = socket(AF_INET , SOCK_STREAM , 0);  //add error check
     
    //type of socket created  
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( PORT );   
         
    //bind the socket to localhost port 8888  
    bind(master_socket, (struct sockaddr *)&address, sizeof(address)); // add error check

    printf("Listener on port %d \n", PORT);   
         
    //try to specify maximum of 3 pending connections for the master socket  
    listen(master_socket, 3); // change number add error
         
    //accept the incoming connection  
    int addrlen = sizeof(address);   
    puts("Waiting for connections ...");   
         
    while(TRUE)   
    {
        //clear the socket set  
        FD_ZERO(&readfds);   
     
        //add master socket to set  
        FD_SET(master_socket, &readfds);   
        int max_sd = master_socket;   
 
        //add child sockets to set  
        for (int i = 0 ; i < max_clients ; i++)   
        {   
            //socket descriptor  
            int sd = client_socket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
            {
                FD_SET( sd , &readfds);
            }   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
            {
                max_sd = sd;
            }   
        }   

        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        int activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);    

        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds))   
        {   
            int new_socket = accept(master_socket,  (struct sockaddr *)&address, (socklen_t*)&addrlen); // add error check

            //inform user of socket number - used in send and receive commands  
            printf("New connection , socket fd is %d , ip is : %s , port : %d  \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs (address.sin_port));     
                 
            //add new socket to array of sockets  
            for (int i = 0; i < max_clients; i++)   
            {   
                //if position is empty  
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;   
                    printf("Adding to list of sockets as %d\n" , i);   
                         
                    break;   
                }   
            }   
        }   
             
        //else its some IO operation on some other socket 
        for (int i = 0; i < max_clients; i++)   
        {   
            int sd = client_socket[i];   
                 
            if (FD_ISSET( sd , &readfds))   
            {   
                int valread = read( sd , buffer, 1024);
                //Check if it was for closing , and also read the  
                //incoming message  
                if (valread == 0)
                {   
                    //Somebody disconnected , get his details and print  
                    getpeername(sd , (struct sockaddr*)&address ,(socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                         
                    //Close the socket and mark as 0 in list for reuse  
                    close( sd );   
                    client_socket[i] = 0;   
                }   
                     
                //Echo back the message that came in  
                else 
                {   
                    //set the string terminating NULL byte on the end  
                    //of the data read  
                    buffer[valread] = '\0';
                    std::cout << buffer << std::endl;
                    send(sd , buffer , strlen(buffer) , 0 );   
                }   
            }   
        }   
    }   
         
    return 0;   
}



