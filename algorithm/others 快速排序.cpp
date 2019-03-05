#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
////c++实现快速排序
////原理：先找到一个基准，比之小的放左边，大的放右边，然后放回原序列，进行递归
//void quicksort(vector<int> &data, int left,int right)
//{
//    if (left >= right)
//        return;
//    int pivot = (left+right)/2;
//    vector<int> less;
//    vector<int> greater;
//    int tmp;
//    for (int i = left; i < right; i++){
//        if (i == pivot)
//            tmp = data[pivot];
//        else if (data[i] < data[pivot])
//            less.push_back(data[i]);
//        else
//            greater.push_back(data[i]);
//    }
//    for (int i = left; i < right; i++){
//        if (i < left + less.size())
//            data[i] = less[left +less.size()-i-1 ];//less.size()个下标-1，顺序不管
//        else if (i == left + less.size())
//            data[i] = tmp, pivot = i;
//        else
//            data[i] = greater[left+greater.size()+ less.size() - i];
//    }
//    quicksort(data,left, pivot);
//    quicksort(data, pivot+1,right);
//}


//int main()
//{
//    vector<int> data = { 12, 4, 34, 6, 8, 65, 3, 2, 988, 45 };
//    cout << "排序前：";
//    for(int i=0;i<data.size();i++)
//        cout << data[i]<< "  ";
//    cout << endl;
//    quicksort(data, 0, data.size());
//    cout << "排序后：";
//    for(int i=0;i<data.size();i++)
//        cout << data[i]<< "  ";
//    return 0;
//}
int partition(vector<int> &arr, int left, int right)  //找基准数 划分
{
    int i = left + 1 ;
    int j = right;
    int temp = arr[right];
    while(i <= j){
        while (arr[i] < temp)
            i++;
        while (arr[j] > temp)
            j--;
        if (i < j)//将大的交换到左边，小的交换到右边
            swap(arr[i++], arr[j--]);
        else i++;
    }
    swap(arr[j], arr[left]);//将选择的轴值交换到中间
    return j;

}

void quick_sort(vector<int> &arr, int left, int right)
{
    if (left > right)
        return;
    int j = partition(arr, left, right);
    quick_sort(arr, left, j - 1);
    quick_sort(arr, j + 1, right);
}
int main()
{
    vector<int> data = { 12, 4, 34, 6, 8, 65, 3, 2, 988, 45 };
    cout << "排序前：";
    for(int i=0;i<data.size();i++)
        cout << data[i]<< "  ";
    cout << endl;
    quick_sort(data, 0, data.size()-1);
    cout << "排序后：";
    for(int i=0;i<data.size();i++)
        cout << data[i]<< "  ";
    return 0;
}
