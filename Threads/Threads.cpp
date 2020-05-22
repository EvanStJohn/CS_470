#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <atomic>
#include <pthread.h>
#include <unistd.h>
#include <sys/file.h>
using namespace std;

void bubbleSort(vector<int> &nums);
void insertionSort(vector<int> &nums);
void quickSort(vector<int> &nums, int low, int high);
int partition(std::vector<int> &nums, int low, int high);
int getRandom(int low, int high);
bool validateInput(char* input);
vector<int> readNums(int start, int end);
void writeNums(vector<int> &nums, int index);
void *threadCheck(void *threadId);
void *threadSort(void *threadId);
void threadLauncher();

atomic<bool> sorted;
atomic<bool> check;
int size;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "You need to provide how many numbers to sort" << endl;
    }
    else if (argc > 2)
    {
        cout << "You have provided too many parameters" << endl;
    }
    else if (validateInput(argv[1]))
    {
        threadLauncher();
    }
    else
    {
        cout << "You need to provide an integer value that is greater than 1 to start sorting" << endl;
    }

    return 0;
}

bool validateInput(char* input)
{
    string str(input);

    for (int i = 0; i < str.length(); i++) {
        if (!isdigit(str[i]))
        {
            return false;
        }
    }
    size = atoi(input);

    if (size < 2)
    {
        return false;
    }

    return true;
}

void threadLauncher()
{
    sorted = false;
    check = true;
    int threadCount = 0;

    cout << "How many threads do you want to sort the file?" << endl;
    cin >> threadCount;
    while (cin.fail() || threadCount < 1)
    {
        cout << "You need to input an integer greater than 0 to start" << endl;
        cin.clear();
        cin.ignore(256, '\n');
        cin >> threadCount;
    }
    threadCount += 1;

    srand(time(NULL));
    fstream file("Sorted.txt", fstream::out);
    for (int i = 0; i < size; i++)
    {
        file << getRandom(1, 999) << "\t";
    }
    file.close();

    string str;
    vector<int> nums = readNums(0, size - 1);
    cout << "File before sorting:" << endl;
    for (int i = 0; i < nums.size(); i++)
    {
        if (i == nums.size() - 1)
        {
            str.append(to_string(nums[i]) + "\n");
        }
        else
        {
            str.append(to_string(nums[i]) + ", ");
        }
    }
    cout << str << endl;

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

    nums = readNums(0, size - 1);
    str = "";
    cout << "File after sorting:" << endl;
    for (int i = 0; i < nums.size(); i++)
    {
        if (i == nums.size() - 1)
        {
            str.append(to_string(nums[i]) + "\n");
        }
        else
        {
            str.append(to_string(nums[i]) + ", ");
        }
    }
    cout << str << endl;
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

    for(int j = low; j <= high - 1; j++)
    {
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
    int id = (int) (long) threadId;
    while (!sorted)
    {
        if (check)
        {
            int fd;
            fd = open("Sorted.txt", O_RDWR);
            flock(fd, LOCK_EX);

            vector<int> nums = readNums(0, size);
            flock(fd, LOCK_UN);
            close(fd);

            int count = 0;

            if (nums.size() == 1)
            {
                sorted = true;
            }
            else
            {
                for (int i = 1; i < nums.size(); i++)
                {
                    if (nums.at(i) >= nums.at(i-1))
                    {
                        count++;
                    }
                }

                if (count == size - 1)
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
        if (!check)
        {
            int i = getRandom(0, size - 2);
            int j = getRandom(i + 1, size - 1);

            // lock the file
            int fd;
            fd = open("Sorted.txt", O_RDWR);
            flock(fd, LOCK_EX);

            vector<int> nums = readNums(i, j);

            string method;
            switch (getRandom(0, 2))
            {
                case 0:
                    quickSort(nums, 0, nums.size() - 1);
                    method = "QuickSort";
                    break;
                case 1:
                    insertionSort(nums);
                    method = "Insertion Sort";
                    break;
                case 2:
                    bubbleSort(nums);
                    method = "Bubble Sort";
                    break;
                default:
                    break;
            }

            writeNums(nums, i);

            string output = "Thread " + to_string(id) + " used " + method + " on "  + to_string(i) + " - " + to_string(j) + ":\n";
            for (int i = 0; i < nums.size(); i++) {
                if (i == nums.size() - 1)
                {
                    output.append(to_string(nums[i]) + "\n");
                }
                else
                {
                    output.append(to_string(nums[i]) + ", ");
                }
            }
            cout << output << endl;

            //unlock file
            flock(fd, LOCK_UN);
            close(fd);
            check = true;
        }
    }

    return (void*)0;
}

