#include <iostream>
using namespace std;

//冒泡排序 时间复杂度n*n，空间复杂度1
void bubble_sort (int a[], int n) {
    for(int i=0;i<n;++i){//n轮循环
        for(int j=0;j<n-1;++j){//每轮循环进行n-i次比较,将最大值放到尾部
            if(a[j]>a[j+1])
            {
                int tmp=a[j+1];
                a[j+1]=a[j];
                a[j]=tmp;
            }
        }
    }
}

//优化冒泡 时间复杂度n*n，最好为n，空间复杂度1
void optimized_bubble_sort(int a[],int n){
    bool swapFlag=true;//记录一个标志位表示依次循环比较是否发生交换，如果没有交换，说明已经有序，break
    while(swapFlag){
        swapFlag=false;
        for(int i=0;i<n-1;++i){
            if(a[i]>a[i+1]){
                int tmp=a[i];
                a[i]=a[i+1];
                a[i+1]=tmp;
                swapFlag=true;
            }
        }
    }
}

//插入排序 时间复杂度n*n，最优情况为基本有序，时间复杂度为n，空间复杂度1
void insertion_sort(int a[],int n){
    int i,j;
    for(i=0;i<n;++i){
        int tmp=a[i];//每次选择一个元素
        for(j=i;j-1>=0&&a[j-1]>tmp;--j){//将tmp循环与排序好的元素从大到小比较，将大的元素向后移
            a[j]=a[j-1];
        }
        a[j]=tmp;
    }
}

//希尔排序 插入排序的修改版
void shellSort(int a[],int n){
    for(int step=n/2;step>0;step/=2){//逐步减小步长
        for(int i=step;i<n;++i){//在每个步长间隔里进行插入排序
            int j=i;
            int tmp=a[j];
            for(;j-step>=0&&a[j-step]>tmp;j-=step){
                a[j]=a[j-step];
            }
            a[j]=tmp;
        }
    }
}

int main(){
    int a[10]={3,4,7,6,1,2,5,9,8,0};
    int n=sizeof(a)/sizeof(a[0]);

    //bubble_sort(a,n);
    //optimized_bubble_sort(a,n);
    //insertion_sort(a,n);
    shellSort(a,n);

    for(int i=0;i<n;++i)
        cout<<a[i]<<" ";
    return 0;
}
