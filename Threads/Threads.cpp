#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>
#include <atomic>
#include <pthread.h>
#include <unistd.h>
using namespace std;

void bubbleSort(vector<int> &nums);
void insertionSort(vector<int> &nums);
void quickSort(vector<int> &nums, int low, int high);
int partition(std::vector<int> &nums, int low, int high);
int getRandom(int low, int high);
vector<int> readNums(int start, int end);
void writeNums(vector<int> &nums, int index, int size);
void *threadCheck(void *threadId);
void *threadSort(void *threadId);
void threadmanager();

atomic<int> mtx;
atomic<bool> sorted;
atomic<bool> check;
int size;
int threadCount;


int main(int argc, char* argv[])
{
    // validate command line input
    size = 10;

    // ask for thread count and validate
    threadCount = 5;

    // fill file with random numbers
    srand(time(NULL));
    fstream file("Sorted.txt", fstream::out);
    for (int i = 0; i < size; i++)
    {
        file << getRandom(1, 999) << "\t";
    }
    file.close();

    // create threads
    mtx = 1;
    sorted = false;
    check = true;

    pthread_t threads[threadCount];
    for (int i = 0; i < threadCount; i++)
    {
        int val;
        if (i == 0)
        {
            val = pthread_create(&threads[i], NULL, threadCheck, (void*) (long) i);
        }
        else
        {
            val = pthread_create(&threads[i], NULL, threadSort, (void*) (long) i);
        }

        if (val)
        {
            cout << "Unable to create thread " << val << endl;
            exit(-1);
        }
    }

    // join threads
    for (int i = 0; i < threadCount; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

void bubbleSort(vector<int> &nums)
{
    for (int i = 0; i < nums.size(); i++)
    {
        for (int j = 0; j < nums.size()-i-1; j++)
        {
            if (nums.at(j) > nums.at(j+1))
            {
                iter_swap(nums.begin() + j, nums.begin() + j + 1);
            }
        }
    }
}

void insertionSort(vector<int> &nums)
{
    for (int i = 1; i < nums.size(); i++)
    {
        int j = i;

        while (j > 0)
        {
            if (nums.at(j-1) > nums.at(j))
            {
                iter_swap(nums.begin() + j, nums.begin() + j - 1);
                j--;
            }
            else
            {
                break;
            }
        }
    }
}

void quickSort(vector<int> &nums, int low, int high)
{
    if (low < high)
    {
        int pivot = partition(nums, low, high);
        quickSort(nums, low, pivot - 1);
        quickSort(nums, pivot + 1, high);
    }
}

int partition(std::vector<int> &nums, int low, int high)
{
    int pivot = nums[high];
    int i = (low - 1);

    for(int j = low; j <= high - 1; j++){
        if(nums[j] < pivot){
            i++;
            iter_swap(nums.begin() + i, nums.begin() + j);
        }
    }
    iter_swap(nums.begin() + i+1, nums.begin() + high);
    return i+1;
}

int getRandom(int low, int high)
{
    return low + rand() % ((high + 1) - low);
}

void writeNums(vector<int> &nums, int index)
{
    vector<int> array = readNums(0, size - 1);
    fstream file("Sorted.txt", fstream::out);

    for (int i = 0; i < nums.size(); i++)
    {
        array[i + index] = nums.at(i);
    }

    for (int i = 0; i < size; i++)
    {
        file << array.at(i) << "\t";
    }

    file.close();
}

vector<int> readNums(int start, int end)
{
    ifstream file("Sorted.txt");
    vector<int> array;

    if (file.is_open())
    {
        int num;

        while (file >> num)
        {
            array.push_back(num);
        }

        file.close();
    }
    vector<int> result(array.begin() + start, array.begin() + end + 1);

    return result;
}

void *threadCheck(void *threadId)
{
    while (!sorted)
    {
        if (check)
        {
            cout << "checking" << endl;
            vector<int> nums = readNums(0, size);
            int count = 0;

            if (nums.size() == 1)
            {
                sorted = true;
            }
            else
            {
                for (int i = 1; i < nums.size(); i++)
                {
                    if (nums.at(i) > nums.at(i-1))
                    {
                        count++;
                    }
                }

                if (count == size)
                {
                    sorted = true;
                }
            }
            check = false;
        }
    }

    return (void*)0;
}

void *threadSort(void *threadId)
{
    int id = (int) (long) threadId;

    while (!sorted)
    {
        if (mtx == id)
        {
            int i = getRandom(0, size - 2);
            int j = getRandom(i + 1, size - 1);
            cout << i << " " << j << endl;

            vector<int> nums = readNums(i, j);

            switch (getRandom(0, 2))
            {
                case 0:
                    insertionSort(nums);
                    break;
                case 1:
                    insertionSort(nums);
                    break;
                case 2:
                    bubbleSort(nums);
                    break;
                default:
                    break;
            }

            writeNums(nums, i);

            if (mtx == threadCount - 1)
            {
                mtx = 1;
            }
            else
            {
                mtx++;
            }
            check = true;
        }
    }

    return (void*)0;
}

