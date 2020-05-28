#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

struct process {
   char name[16];
   int32_t id;
   unsigned char activityStatus;
   int32_t burst;
   int32_t baseRegister;
   long limitRegister;
   char priority;
};

process readProcess(int index, char* path);
void writeProcess(int index, process data, char* path);
void printProcess(process data);
bool priority(int size, char* path);
char getHighestPriority(int size, char* path);
void ageProcesses(int size, char* path);
int getSize(char* path);
bool roundRobin(int size, char* path);
void runScheduler(char* path);
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
      runScheduler(argv[1]);
   }
   else
   {
      cout << "This file does not exist" << endl;
   }
   
   return 0; 
}

process readProcess(int index, char* path)
{
   ifstream file(path, ios::in | ios::binary);
   process data;

   if (file.is_open())
   {
      file.seekg((index * 38), ios::beg);

      file.read(data.name, 16);
      file.read(reinterpret_cast<char *> (&data.id), 4);
      file.read(reinterpret_cast<char *> (&data.activityStatus), 1);
      file.read(reinterpret_cast<char *> (&data.burst), 4);
      file.read(reinterpret_cast<char *> (&data.baseRegister), 4);
      file.read(reinterpret_cast<char *> (&data.limitRegister), 8);
      file.read(reinterpret_cast<char *> (&data.priority), 1);

      file.close();
   }
   
   return data;
}

void writeProcess(int index, process data, char* path)
{
   ofstream file(path, ios::out | ios::binary | ios::in);

   if (file.is_open())
   {
      file.seekp((index * 38), ios::beg);
      
      file.write(data.name, 16);
      file.write(reinterpret_cast<char *> (&data.id), 4);
      file.write(reinterpret_cast<char *> (&data.activityStatus), 1);
      file.write(reinterpret_cast<char *> (&data.burst), 4);
      file.write(reinterpret_cast<char *> (&data.baseRegister), 4);
      file.write(reinterpret_cast<char *> (&data.limitRegister), 8);
      file.write(reinterpret_cast<char *> (&data.priority), 1);

      file.close();
   }
}

void printProcess(process data)
{
   int activity = (int) data.activityStatus;
   int priority = (int) data.priority;

   cout << "Name:" << data.name << endl;
   cout << "Id:" << data.id << endl;
   cout << "Activity status:" << activity << endl;
   cout << "Burst Time:" << data.burst << endl;
   cout << "Base Register:" << data.baseRegister << endl;
   cout << "Limit Register:" << data.limitRegister << endl;
   cout << "Priority:" << priority << endl;
}

void runScheduler(char* path)
{
   bool finished = false;
   int size = getSize(path);

   while (!finished)
   {
      for (int i = 0; i < 2; i++)
      {
         if (i == 0)
         {
            cout << "\nStarting Round Robin algorithm" << endl;
            finished = roundRobin(size, path);
         }
         else
         {
            cout << "\nStarting Priority algorithm" << endl;
            finished = priority(size, path);
         }

         if (finished)
         {
            break;
         }
      }
   }
   cout << "\nAll processes have been executed" << endl;
}

bool roundRobin(int size, char* path)
{
   int quantum = 0;
   int completionCount = 0;

   while (quantum < 30 && completionCount != size)
   {
      completionCount = 0;

      for (int i = 0; i < size; i++)
      {
         process data = readProcess(i, path);

         if (data.burst > 0)
         {
            data.burst--;
            quantum++;
            cout << "\nexecuting process at position " << i << endl;
            printProcess(data);
            writeProcess(i, data, path);

            if (data.burst == 0)
            {
               cout << "Process has finished" << endl;
            }

            //usleep(500000);
         }
         else
         {
            completionCount++;
         }
         
         if (completionCount == size)
         {
            return true;
         }
         
         if (quantum == 30)
         {
            break;
         }
      }
   }
   return false;
}

bool priority(int size, char* path)
{
   int quantum = 0;
   int completionCount = 0;
   int ageCount = 0;

   while (quantum < 30 && completionCount != size)
   {
      int highestPriority = getHighestPriority(size, path);
      completionCount = 0;

      for (int i = 0; i < size; i++)
      {
         process data = readProcess(i, path);

         if (data.burst > 0)
         {
            if (data.priority == highestPriority)
            {
               data.burst--;
               quantum++;
               ageCount++;
               cout << "\nexecuting process at position " << i << endl;
               printProcess(data);
               writeProcess(i, data, path);

               if (data.burst == 0)
               {
                  cout << "Process has finished" << endl;
               }

               if (ageCount == 2)
               {
                  cout << "Aging processes by 1" << endl;
                  ageProcesses(size, path);
                  ageCount = 0;
               }         

               //usleep(500000);
            }
         }
         else
         {
            completionCount++;
         }
         
         if (completionCount == size)
         {
            return true;
         }
         
         if (quantum == 30)
         {
            break;
         }
      }
   }
   return false;
}

char getHighestPriority(int size, char* path)
{
   char min = INT8_MAX;

   for (int i = 0; i < size; i++)
   {
      process data = readProcess(i, path);

      if (data.priority < min && data.burst != 0)
      {
         min = data.priority;
      } 
   }
   return min;
}

void ageProcesses(int size, char* path)
{
   for (int i = 0; i < size; i++)
   {
      process data = readProcess(i, path);

      if (data.priority > 0)
      {
         data.priority--;
      }
      
      writeProcess(i, data, path);
   }
}

int getSize(char* path)
{
   int fileSize;
   int processes;

   ifstream file(path, ios::in | ios::binary);

   if (file.is_open())
   {
      file.seekg(0, ios::end);
      fileSize = file.tellg();
      file.close();
   }

   processes = fileSize / 38;

   cout << "This file contains " << processes << " processes" << endl;
   cout << "which is a total of " << fileSize << " bytes\n" << endl;

   return processes;
}

bool validateFile(char* path)
{
   ifstream file(path);
   return (bool)file;
}

