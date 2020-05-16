#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

vector<int> createNums(int size);
void bubbleSort(vector<int> &nums);
void swap(int &a, int &b);
void insertionSort(vector<int> &nums);
void quickSort(vector<int> &nums, int low, int high);
int partition(std::vector<int> &nums, int low, int high);
void readNums();
void writeNums();

int main(int argc, char* argv[])
{
    vector<int> nums = {5, 1, 6, 7, 8, 2};

    bubbleSort(nums);
    
    for (int i = 0; i < nums.size(); i++)
    {
        cout << nums.at(i) << endl;
    }
    
}

void bubbleSort(vector<int> &nums)
{
    for (int i = 0; i < nums.size(); i++)
    {
        for (int j = 0; j < nums.size()-i-1; i++)
        {
            if (nums.at(i) > nums.at(i+1))
            {
                iter_swap(nums.begin() + j, nums.begin() + j - 1);
            }
        }
    }
}

void swap(int &a,int &b)
{
    int temp = a;
    a = b;
    b = temp;
}

void insertionSort(vector<int> &nums)
{
    for (int i = 0; i < nums.size(); i++)
    {
        int j = i;

        while (nums.at(j-1) > nums.at(j) && j > 0)
        {
            iter_swap(nums.begin() + j, nums.begin() + j - 1);
        }
    }
}

void quickSort(vector<int> &nums, int low, int high)
{
    int pivot = partition(nums, low, high);
    quickSort(nums, low, pivot - 1);
    quickSort(nums, pivot + 1, high);
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
