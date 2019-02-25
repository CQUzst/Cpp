//排序算法总结
#include <iostream>
#include <vector>
using namespace std;
//插入排序
void insertion_sort(vector<int> arr, int length){
    int i,j;
    for(i=1;i<length;i++){
        int tmp=arr[i];
        for(j=i;j>0&&arr[j-1]>tmp;j--){
            arr[j]=arr[j-1];
        }
        arr[j]=tmp;
    }
}

int main(){
    vector<int> arr{2,6,2,6,4,6,4};
    insertion_sort(arr,arr.size());
    return 0;
}
