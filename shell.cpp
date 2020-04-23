#include <stdlib.h>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <sys/sysinfo.h>
#include <fstream>
using namespace std;

vector<string> parse(string &input);
void close(vector<string> &cmnd, int &status);
int changePrompt(vector<string> &cmnd, string &prompt);
int cpuInfo(vector<string> &cmnd);
int memInfo(vector<string> &cmnd);
int manual();
void runShell();
string retrieve(string path, string info);

int main() {
    runShell();
}

void runShell()
{
    string prompt = "cwushell";
    int status = 0;
    string input;
    vector<string> cmnd;

    cout << "Welcome to the cwushell, use the command 'manual' for assistance" << endl;

    while (true)
    {
        cout << prompt + ">";
        getline(cin, input);
        cmnd = parse(input);

        if(cmnd[0] == "exit")
        {
            close(cmnd, status);
        }
        else if (cmnd[0] == "prompt")
        {
            status = changePrompt(cmnd, prompt);
        }
        else if (cmnd[0] == "cpuinfo")
        {
            status = cpuInfo(cmnd);
        }
        else if (cmnd[0] == "meminfo")
        {
            status = memInfo(cmnd);
        }
        else if (cmnd[0] == "manual")
        {
            status = manual();
        }
        else
        {
            status = system(input.c_str());
        }
    }
    
}

void close(vector<string> &cmnd, int &status)
{
    if (cmnd.size() == 1)
    {
        cout << "Terminating Shell with status code " + to_string(status) << endl;
        exit(status);
    }
    else if (cmnd.size() > 2)
    {
        cout << "Too many parameters were given" << endl;
        status = 1;
    }
    else if (cmnd[1] == "-h" || cmnd[1] == "--help")
    {
        cout << "Usage: exit [N]" << endl;
        cout << "Exits the cwushell with the status of entered N" << endl;
        status = 0;
    }
    else
    {
        char* p;
        long num = strtol(cmnd[1].c_str(), &p, 10);
        if (*p) {
            cout << "Provided status code is not an integer" << endl;
            status = 1;
        }
        else {
            cout << "Terminating Shell with status code " + to_string(num) << endl;
            exit(num);
        }
    }
}

int changePrompt(vector<string> &cmnd, string &prompt)
{
    if (cmnd.size() == 1)
    {
        prompt = "cwushell";
    }
    else if (cmnd.size() > 2)
    {
        cout << "Error: Too many parameters were given" << endl;
        return 1;
    }
    else if (cmnd[1] == "-h" || cmnd[1] == "--help")
    {
        cout << "Usage: prompt [name]" << endl;
        cout << "Changes the terminal prompt to the entered name" << endl;
        cout << "If name is not provided prompt will revert to 'cwushell'" << endl;
    }
    else
    {
        prompt = cmnd[1];
    }

    return 0;
}

int cpuInfo(vector<string> &cmnd)
{
    bool flag[4] = {false, false, false, false};
    bool check = false;

    if (cmnd.size() == 1)
    {
        cout << "Missing Input use -h or --help for assistance" << endl;
        return 1;
    }

    for (int i = 1; i < cmnd.size(); i++)
    {
        if (cmnd[i] == "-c")
        {
            flag[0] = true;
            check = true;
        }

        if (cmnd[i] == "-t")
        {
            flag[1] = true;
            check = true;
        }

        if (cmnd[i] == "-n")
        {
            flag[2] = true;
            check = true;
        }

        if (cmnd[i] == "-h" || cmnd[i] == "--help")
        {
            flag[3] = true;
            check = true;
        }

        if (check)
        {
            check = false;
        }
        else
        {
            cout << "Invalid Switch Provided: try using -c, -t, -n or -h, --help for assistance" << endl;
            return 1;
        }
    }

    if (flag[0])
    {
        float result = atof(retrieve("/proc/cpuinfo", "cpu MHz		: ").c_str()) / 1000;
        result = round(result*10)/10;
        cout << "CPU Clock: " + to_string(result).substr(0,3) + " GHz" << endl;
    }

    if (flag[1])
    {
        cout << "CPU Type: " + retrieve("/proc/cpuinfo", "vendor_id	: ") << endl;
    }

    if (flag[2])
    {
        cout << "# of cores: " + retrieve("/proc/cpuinfo", "cpu cores	: ") << endl;
    }

    if (flag[3])
    {
        cout << "Usage: cpuinfo [Option]" << endl;
        cout << "Prints out system cpu info" << endl;
        cout << "-c \tprint the CPU Clock" << endl;
        cout << "-t \tprint the CPU Type" << endl;
        cout << "-n \tprint the number of CPU Cores" << endl;
        cout << "-h, --help \tprint out a short help message" << endl;
    }

    return 0;
}

int memInfo(vector<string> &cmnd)
{
    bool flag[4] = {false, false, false, false};
    bool check = false;

    if (cmnd.size() == 1)
    {
        cout << "Missing Input use -h or --help for assistance" << endl;
        return 1;
    }

    for (int i = 1; i < cmnd.size(); i++)
    {
        if (cmnd[i] == "-t")
        {
            flag[0] = true;
            check = true;
        }

        if (cmnd[i] == "-u")
        {
            flag[1] = true;
            check = true;
        }

        if (cmnd[i] == "-c")
        {
            flag[2] = true;
            check = true;
        }

        if (cmnd[i] == "-h" || cmnd[i] == "--help")
        {
            flag[3] = true;
            check = true;
        }

        if (check)
        {
            check = false;
        }
        else
        {
            cout << "Invalid Switch Provided: try using -c, -t, -n or -h, --help for assistance" << endl;
            return 1;
        }
    }

    struct sysinfo info;
    sysinfo(&info);

    if (flag[0])
    {
        cout << "Total RAM: " + to_string(info.totalram) + " Bytes" << endl;
    }

    if (flag[1])
    {
        cout << "Used RAM: " + to_string(info.totalram - info.freeram) + " Bytes" << endl;
    }

    if (flag[2])
    {
        string result = retrieve("/sys/devices/system/cpu/cpu0/cache/index2/size", "");
        result = result.substr(0, result.size()-1) + "000 Bytes";
        cout << "L2 Cache Size: " + result << endl;
    }

    if (flag[3])
    {
        cout << "Usage: meminfo [Option]" << endl;
        cout << "Prints out system memory info" << endl;
        cout << "-t \tprint the total RAM memory available in the system in bytes" << endl;
        cout << "-u \tprint the used RAM memory" << endl;
        cout << "-c \tprint the size of the L2 cache in bytes" << endl;
        cout << "-h, --help \tprint out a short help message" << endl;
    }

    return 0;
}

int manual()
{
    cout << "This is the cwushell manual" << endl;
    cout << "This Shell accepts all basic linux and command and built-in commands listed below\n" << endl;
    cout << "quit [N]: exits the cwushell" << endl;
    cout << "the shell will return the provided status code N if one is provided\n" << endl;
    cout << "prompt [name]: changes the current shell prompt to the entered prompt. " << endl;
    cout << "using prompt with no parameter will revert the prompt to 'cwushell'\n" << endl;
    cout << "cpuinfo [OPTION]: prints out system CPU info for each provided switch" << endl;
    cout << "-t \tprint the total RAM memory available in the system in bytes" << endl;
    cout << "-u \tprint the used RAM memory" << endl;
    cout << "-c \tprint the size of the L2 cache in bytes" << endl;
    cout << "-h, --help \tprint out a short help message\n" << endl;
    cout << "meminfo [OPTION]: prints out system memory info for each provided switch" << endl;
    cout << "-t \tprint the total RAM memory available in the system in bytes" << endl;
    cout << "-u \tprint the used RAM memory" << endl;
    cout << "-c \tprint the size of the L2 cache in bytes" << endl;
    cout << "-h, --help \tprint out a short help message" << endl;
    return 0;
}

vector<string> parse(string &input)
{
    vector <string> tokens;
    string temp;

    stringstream check1(input);

    while(getline(check1, temp, ' '))
    {
        if (temp != "")
        {
            tokens.push_back(temp);
        }
    }

    return tokens;
}

string retrieve(string path, string info)
{
    string line;
    ifstream file(path);
    
    if (file.is_open())
    {
        while (!file.eof())
        {
            getline(file, line);
            
            if (!line.find(info))
            {
                file.close();
                return line.substr(info.size(), line.size() - info.size());
            }
        }
    }

    return NULL;
}
