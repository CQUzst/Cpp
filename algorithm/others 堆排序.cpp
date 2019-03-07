#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
/*
堆排序 不稳定排序
完全二叉树特性：
左边子节点位置 = 当前父节点的两倍 + 1
右边子节点位置 = 当前父节点的两倍 + 2
步骤二 从最后一个非叶子结点开始（arr.length/2-1）从下至上进行调整。
将i位置的值更新，如果子树有更大的值，替换到i处
*/
//函数功能：将arr以n为界限，下标i的子树建立成最大堆
void heapify(vector<int> &arr, int n, int i) {
    int largest = i;
    int l = 2*i + 1; // left = 2*i + 1
    int r = 2*i + 2; // right = 2*i + 2
    if (l < n && arr[l] > arr[largest])
        largest = l;
    if (r < n && arr[r] > arr[largest])
        largest = r;
    if (largest != i) {
        swap(arr[i], arr[largest]);
        //交换有可能破坏子树的最大堆性质，所以对所交换的那个子节点进行一次维护
        heapify(arr, n, largest);
    }
}
void heapSort(vector<int> &arr, int n) {
    //调整数组当中数的位置将其处理为一个最大堆
    //从length/2-1递减到0就意味着，从最后的一个非叶节点开始“从小到大”地进行维护
    //保证每一个点的子树都是最大堆，可以确保最大堆的性质，而如果从0递增到length/2-1则无法保证堆的性质
    for (int i = n/2-1; i >= 0; i--)
        heapify(arr, n, i);
    // 一个个从堆顶取出元素
    for (int i=n-1; i>=0; i--) {
        swap(arr[0], arr[i]);  //arr[0]经过最大堆已经是最大值，放到后面
        heapify(arr, i, 0);  //i为边界，将0下标位置更新成最大堆根
    }
}

int main()
{
    vector<int> data = { 12, 4, 34, 6, 8, 65, 3, 2, 988, 45 };
    cout << "排序前：";
    for(size_t i=0;i<data.size();i++)
        cout << data[i]<< "  ";
    cout << endl;
    heapSort(data, data.size());
    cout << "排序后：";
    for(size_t i=0;i<data.size();i++)
        cout << data[i]<< "  ";
    return 0;
}
