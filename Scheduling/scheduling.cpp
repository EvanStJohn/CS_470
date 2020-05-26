#include <iostream>
#include <fstream>

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

process readProcess(int index);
void writeProcess(int index, process data);
void priority();
void roundRobin();
void runScheduler();
int getSize();

int main()
{
   
}

process readProcess(int index)
{
   ifstream file("processes.bin", ios::in | ios::binary);
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

void writeProcess(int index, process data)
{
   ofstream file("processes.bin", ios::out | ios::binary | ios::in);

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
