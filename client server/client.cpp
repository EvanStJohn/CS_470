#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <bits/stdc++.h>

using namespace std;

string manual(int row, int col);
string automatic(int row, int col);
void initSocket(string ip, int port, bool automated);
int validateInput(char* input);

int main(int argc , char* argv[])   
{
    if(argc > 4)
    {
        cout << "You have entered too many parameters" << endl; //redo
    }
    if(argc == 1)
    {
        cout << "To start this client you need to enter: ip_address port manual/automatic" << endl;
        cout << "Example: ./client 127.0.0.1 5432 manual" << endl;
    }
    else
    {
        string ip(argv[1]);
        int port = validateInput(argv[2]);
        string automated(argv[3]);

        if (port < 1025)
        {
            cout << "You have provided a bad port number" << endl;
            return 0;
        }
        
        if (automated == "manual")
        {
            initSocket(ip, port, false);
        }
        else if (automated == "automatic")
        {
            initSocket(ip, port, true);
        }
        else
        {
            cout << "you need to provide the keyword 'manual' or 'automatic'" << endl;
        } 
    }

    return 0;   
}

string manual(int row, int col)
{
    cout << "Please Select your tickets in the ranges [0-" << row - 1 << "] [0-" << col - 1 << "]" << endl; 
    string input;
    bool cont = true;
    vector <int> tokens;
    do
    {
        cout << "Select your seats: ";
        getline(cin,input);
        
        cont = true;
        try
        {
            tokens.clear(); 
            
            stringstream check(input); 
            
            string temp; 
            
            while(getline(check, temp, ' ')) 
            { 
                tokens.push_back(stoi(temp)); 
            }
        }
        catch(const std::exception& e)
        {
            cont = false;
            cout << "you need to enter two integer values" << endl;
        }

        if (tokens.size() != 2)
        {
            cont = false;
            cout << "you have provided the wrong amount of values" << endl;
        }
        else
        {
            if (tokens[0] >= row || tokens[1] >= col || tokens[0] < 0 || tokens[1] < 0)
            {
               cont = false;
               cout << "You have entered a value that is out of range" << endl;
            }
        }
        
        
    } while (!cont);

    return input;
}

string automatic(int row, int col)
{
    row = rand() % row;
    col = rand() % col;

    cout << "choosing random seats: " << row << " " << col << endl;

    string output = to_string(row) + " " + to_string(col);

    return output;
}

void initSocket(string ip, int port, bool automated)
{
    srand(time(NULL));

    //create socket
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd == -1)
    {
        cout << "Unable to create Socket" << endl;
        return;
    }
    
    sockaddr_in address;

    address.sin_family = AF_INET; 
    address.sin_port = htons(port);


    if (inet_pton(AF_INET, ip.c_str(), &address.sin_addr) <= 0)
    {
        cout << "Your ip address is in the incorrect format" << endl;
        return;
    }        

    //connect to the server on the socket  
    for (int i = 2; i >= 0; i--)
    {
        if(connect(sd, (sockaddr*)&address, sizeof(address)) == -1)
        {
            cout << "Client could not connect to server. retrying " << i << " more times in 10 seconds" << endl;   
        }
        else
        {
            break;
        }
        

        if (i == 0)
        {
            cout << "You have run out of connection attempts. Please check if your server is running" << endl;
            return;
        }

        sleep(10);
    }
    
    cout << "You are now connected to " << ip << ":" << port << endl << endl;

    // get grid size from server
    char buffer[1025];
    memset(buffer, 0, 1025);
    read(sd,buffer,1025);
    char* sizes = buffer;
    sizes = strtok(buffer, " ");
    int row = atoi(buffer);
    sizes = strtok(NULL, " ");
    int col = atoi(buffer);
    cout << "\nTicket Grid has " << row - 1 << " rows and " << col - 1 << " columns\n" << endl;

    while(true)
    {        
        //get user input
        string input;
        if (automated)
        {
            input = automatic(row, col);
        }
        else
        {
            input = manual(row, col);
        }
                
        //send to server
        int send_result = send(sd,input.c_str(), input.size()+1, 0);
        if(send_result == -1)
        {
            cout << "Could not send to server! \r\n";
            continue;        
        }

        //wait for response 
        memset(buffer, 0, 1025);               
        if(int bytes_recieved = read(sd,buffer,1025) == -1)
        {
            cout << "Server has stopped responding \r\n" << endl;           
        }
        else
        {          
            //display response
            string response(buffer);
            cout << response << "\r\n";

            if (response.find("All seats have been selected") != string::npos)
            {
                break;
            }

            if (automated)
            {
                sleep(1);
            }
            
        }    
    }
    
    //close the socket
    cout << "client is now shutting down!" << endl;
    close(sd);
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
