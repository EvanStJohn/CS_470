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
#include <vector>
#include <string.h>

using namespace std;

vector<vector<int>> createSeats(int row, int col);
void runSockets(int port, vector<vector<int>> seats);
string checkTickets(char* input, vector<vector<int>> &seats);
bool checkGrid(vector<vector<int>> &seats);
void printSeats(vector<vector<int>> &seats);
int validateInput(char* input);
      
bool seatsAvailable = true;
int connectedClients = 0;
     
int main(int argc , char *argv[])   
{    

    if(argc > 4)
    {
        cout << "You have entered too many parameters" << endl; //redo
    }
    if(argc == 1)
    {
        cout << "To start this server you need to enter: port row_size column_size" << endl;
        cout << "Example: ./server 5432 10 10" << endl;
    }
    else if (argc == 2)
    {
        int port = validateInput(argv[1]);

        if (port < 1025)
        {
            cout << "You have provided a bad port number" << endl;
            return 0;
        }

        cout << "using default rows 10x10\n" << endl;
        runSockets(port, createSeats(10, 10));
    }
    else if (argc == 3)
    {
        cout << "Please Provide both grid sizes or none at at all";
    }
    else
    {
        int port = validateInput(argv[1]);
        int row = validateInput(argv[2]);
        int col = validateInput(argv[3]);

        if (port < 1025)
        {
            cout << "You have provided a bad port number" << endl;
            return 0;
        }
        
        if (row == 0 || col == 0)
        {
            cout << "You have provided a bad grid size" << endl;
            return 0;
        }
        
        runSockets(port, createSeats(row, col));
    }
         
    return 0;   
}

int validateInput(char* input)
{
    string str(input);

    for (int i = 0; i < str.length(); i++) {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }

    int num = atoi(input);

    if (num < 0)
    {
        return 0;
    }

    return num;
}

vector<vector<int>> createSeats(int row, int col)
{
    vector<vector<int>> seats;

    for (int i = 0; i < row; i++)
    {
        vector<int> temp(col, 0);
        seats.push_back(temp);
    }
    
    return seats;
}

void printSeats(vector<vector<int>> &seats)
{
    for (int i = 0; i < seats.size(); i++)
    { 
        for (int j = 0; j < seats[i].size(); j++)
        {
            cout << seats[i][j] << " ";
        }
        cout << endl; 
    }
    cout << endl;
}

bool checkGrid(vector<vector<int>> &seats)
{
    for (int i = 0; i < seats.size(); i++)
    { 
        for (int j = 0; j < seats[i].size(); j++)
        {
            if (seats[i][j] == 0)
            {
                return false;
            }
        }
    }

    return true;
}

string checkTickets(char* input, vector<vector<int>> &seats)
{
    // split user input
    input = strtok(input, " ");
    int row = atoi(input);
    input = strtok(NULL, " ");
    int col = atoi(input);
    cout << "A client is trying to reserve seats: " << row << " " << col << endl;

    if (seats[row][col] == 0)
    {
        seats[row][col] = 1;

        if (checkGrid(seats))
        {
            cout << "Selection Successful. all seats have now been chosen" << endl;;
            printSeats(seats);
            seatsAvailable = false;
            return "Server: Selected seats have been reserved. All seats have been selected\n";
        }
        else
        {
            cout << "Selection Successful" << endl;
            printSeats(seats);
            return "Server: Selected seats have been reserved\n";
        }
    }
    else
    {
        if (checkGrid(seats))
        {
            cout << "Selection Unsuccessful. all seats have been chosen already\n" << endl;;
            return "Server: All seats have been selected\n";
        }
        else
        {
            cout << "Selection Unsuccessful" << endl;
            printSeats(seats);
            return "Server: Selected seats are already taken.\n";
        }
    }
}

void runSockets(int port, vector<vector<int>> seats)
{
    fd_set readfds;
    int client_socket[10] = {0}; 
    int max_clients = 10;       
    char buffer[1025]; 
         
    //setup sockets 
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons(port);   
         
    //bind the socket to localhost 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        cout << "binding failed" << endl;
        exit(-1);
    }
    

    cout << "Listening on 127.0.0.1:" << port << endl; 
    listen(master_socket, 10); 
    int addrlen = sizeof(address);   
    cout << "Server is now waiting for new connections\n" << endl;
         
    while(true)   
    {
        // prepare sockets
        FD_ZERO(&readfds);     
        FD_SET(master_socket, &readfds);   
        int max_sd = master_socket;   
 
        // get max for select function  
        for (int i = 0 ; i < max_clients ; i++)   
        {    
            int sd = client_socket[i];   
                  
            if(sd > 0)   
            {
                FD_SET( sd , &readfds);
            }   
                  
            if(sd > max_sd)   
            {
                max_sd = sd;
            }   
        }   

        // wait for incoming socket activity
        int activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);    

        // add new client
        if (FD_ISSET(master_socket, &readfds))   
        {   
            int new_socket = accept(master_socket,  (struct sockaddr *)&address, (socklen_t*)&addrlen); // add error check
 
            cout << "New connection at " << inet_ntoa(address.sin_addr) << ":" << ntohs(address.sin_port) << endl << endl;
            connectedClients++;

            string greeting = to_string(seats.size()) + " " + to_string(seats[0].size());
            send(new_socket, greeting.c_str(), greeting.length(), 0);
                   
            for (int i = 0; i < max_clients; i++)   
            {    
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;     
                         
                    break;   
                }   
            }   
        }   
        
        // check for client activity
        for (int i = 0; i < max_clients; i++)   
        {   
            int sd = client_socket[i];   
                 
            if (FD_ISSET(sd, &readfds))   
            {   
                int valread = read(sd, buffer, 1025);
 
                // client disconnected
                if (valread == 0)
                {    
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);   
                    cout << "Client disconnected at " << inet_ntoa(address.sin_addr) << ":" << ntohs (address.sin_port) << endl << endl;
                    connectedClients--;   
                    close(sd);   
                    client_socket[i] = 0;

                    if (connectedClients > 0)
                    {
                        cout << "Waiting for " << connectedClients << " more clients to disconnect\n" << endl;
                    }
                }   
                else 
                {   
                    // purchase seats
                    string output = checkTickets(buffer, seats);

                    send(sd, output.c_str(), output.length(), 0);
                }   
            }   
        }

        if (!seatsAvailable && connectedClients == 0)
        {
            cout << "Server is now shutting down" << endl;
            break;
        }
    }
}
