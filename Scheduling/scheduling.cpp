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
void printProcess(process data);
void priority();
bool roundRobin(int size);
void runScheduler();
int getSize();

int main()
{
   runScheduler();
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

      int test = (int) data.activityStatus;
      int test2 = (int) data.priority;
      cout << data.name << endl;
      cout << data.id << endl;
      cout << test << endl;
      cout << data.burst << endl;
      cout << data.baseRegister << endl;
      cout << data.limitRegister << endl;
      cout << test2 << endl << endl;

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

void printProcess(process data)
{

}

void runScheduler()
{
   bool finished = false;
   int size = 100;

   // get amount of processes and file size

   while (!finished)
   {
      for (int i = 0; i < 2; i++)
      {
         if (i == 0)
         {
            finished = roundRobin(size);
         }
         else
         {
            //priority();
            finished = roundRobin(size);
         }

         // check if finished
      }
   }
}

bool roundRobin(int size)
{
   int quantum = 0;
   int count = 0;

   while (quantum < 30 && count != size)
   {
      count = 0;

      for (int i = 0; i < size; i++)
      {
         process data = readProcess(i);

         if (data.burst > 0)
         {
            data.burst--;
            quantum++;
            cout << "executing process at position " << i << endl;
            printProcess(data);
            writeProcess(i, data);
            // sleep
         }
         else
         {
            count++;
         }
         
         if (count == size)
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
