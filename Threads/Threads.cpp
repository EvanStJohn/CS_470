#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

void bubbleSort(vector<int> &nums);
void insertionSort(vector<int> &nums);
void quickSort(vector<int> &nums, int low, int high);
int partition(std::vector<int> &nums, int low, int high);
vector<int> readNums(int start, int end);
void writeNums(vector<int> &nums, int index, int size);
void createFile();
void checkSorted();
void threadSort(int size);
void threadmanager();


int main(int argc, char* argv[])
{
    vector<int> test = {1, 2, 3};
    writeNums(test, 2, 7);

    vector<int> nums = readNums(0, 6);
    for (int i = 0; i < nums.size(); i++)
    {
        cout << nums.at(i) << endl;
    }
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

void writeNums(vector<int> &nums, int index, int size)
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

void threadSort(int size)
{
    // create random i and j

    // read subarray

    // choosing a random sortin algorithm

    // write numbers back to file
}
