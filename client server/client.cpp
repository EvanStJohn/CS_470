#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

using namespace std;

void manual();
void automatic();
void send();

string ipAddress = "127.0.0.1"; //local machine

int main(int argc , char *argv[])   
{
    if(argc > 2)
    {
        cout << "Only input two parameters!" << endl; //redo
    }
    if(argc == 1)
    {
        cout << "Using default ip address of " << ipAddress << endl; //redo   
    }

    //create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        cout << "Unable to create Socket" << endl;
        return -1;
    }

    int port = 8888; //same as server
    
    sockaddr_in address;

    address.sin_family = AF_INET; 
    address.sin_port = htons(port);


    if (inet_pton(AF_INET, ipAddress.c_str(), &address.sin_addr) <= 0)
    {
        cout << "Inet_pton error occured" << endl;
        return -1;
    }
             

    //connect to the server on the socket  
    if(connect(sock, (sockaddr*)&address, sizeof(address)) == -1)
    {
        cerr << "Connection error. EXITING...." << endl;   
        return -2;
    }

    cout << "Connection established." << endl;
    cout << "Enter exit to exit. Otherwise type in what seats you want to purchase." << endl;
    //while loop   
    char buffer[1025];
    string user_input;

    while(true){        
        //enter lines of text
        cout << "> ";
        getline(cin,user_input);
                
        //send to server
        int send_result = send(sock,user_input.c_str(),user_input.size()+1, 0);
        //todo: check if that failed
        if(send_result == -1)
        {
            cout << "Could not send to server! \r\n";
            continue;        
        }        
        //wait for response 
        memset(buffer, 0, 1025);               
        if(int bytes_recieved = read(sock,buffer,1025) == -1)
        {
            cout << "Server has stopped responding \r\n" << endl;           
        }
        else
        {          
            //display response
            cout << "SERVER> " << string(buffer, bytes_recieved) << "\r\n";
        }    
    }
    
    //close the socket
    close(sock);

    return 0;   
}