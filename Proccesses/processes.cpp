#include <iostream> 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fstream>
#include <cctype>
#include <string.h>
#include <filesystem>
using namespace std;

void runProcess(char* path);
void countLetters(int index, int fd[], char* path);
bool validateFile(char* path);

int main(int argc, char* argv[])
{ 
    if (argc < 2)
    {
        cout << "You need to Provide a file to execute the program" << endl;
    }
    else if (argc > 2)
    {
        cout << "You have provided too many parameters" << endl;
    }
    else if (validateFile(argv[1]))
    {
        runProcess(argv[1]);
    }
    else
    {
        cout << "This file does not exist" << endl;
    }
    
    return 0; 
}

// create multiple children and print their returned results
void runProcess(char* path)
{
    pid_t pid;
    int fd[2 * 26];

    // create a child for each letter in the alphabet
    for (int i = 0; i < 26; i++)
    {
        pipe(&fd[2*i]);
        pid = fork();

        if (pid < 0)
        {
            cout << "Fork Failed" << endl;
            exit(-1);
        }

        if (pid == 0)
        {
            countLetters(i, fd, path);
            exit(0);
        }    
    }

    // wait for children to finish
    for (int i = 0; i < 26; i++)
    {
        wait(NULL);
    }

    // print out statistics from pipes
    for (int i = 0; i < 26; i++)
    {
        char buffer[100];
        char letter = 'a' + i;

        close(fd[2*i + 1]);

        read(fd[2*i], buffer, 100);
        close(fd[2*i]);
        cout << letter << " Occurred: " << buffer << " times" << endl;
    }
}

// count a specific letter from a file and pipe result to parent
void countLetters(int index, int fd[], char* path)
{
    ifstream file(path);
    int count;

    close(fd[2*index]);
    
    if (file.is_open())
    {
        char ch;

        // read a file character by charachter
        while (!file.eof())
        {
            file >> ch;
            ch = toupper(ch) - 'A';
            if (ch == index)
            {
                count++;
            }
        }
        file.close();

        string result = to_string(count);
        char const *buffer = result.c_str();

        // pipe result to the parent
        write(fd[2*index + 1], buffer, strlen(buffer)+1);
        close(fd[2*index + 1]);

        file.close();
    }
}

// determine if a file exists
bool validateFile(char* path)
{
    ifstream file(path);
    return (bool)file;
}
