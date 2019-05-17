#include <iostream>
using namespace std;
/***************************冒泡排序******************************/
//时间复杂度n*n，空间复杂度1
void bubbleSort (int a[], int n) {
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
/***************************优化冒泡排序****************************/
//时间复杂度n*n，最好为n，空间复杂度1
void optimizedBubbleSort(int a[],int n){
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
/***************************插入排序*******************************/
//时间复杂度n*n，最优情况为基本有序，时间复杂度为n，空间复杂度1
void insertionSort(int a[],int n){
    int i,j;
    for(i=0;i<n;++i){
        int tmp=a[i];//每次选择一个元素
        for(j=i;j-1>=0&&a[j-1]>tmp;--j){//将tmp循环与排序好的元素从大到小比较，将大的元素向后移
            a[j]=a[j-1];
        }
        a[j]=tmp;
    }
}
/****************************希尔排序******************************/
//插入排序的修改版 时间复杂度O(n^(3/2))
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

/*********************************基数排序*************************/
//找最大值
int getMax(int a[],int n){
    int m=a[0];
    for(int i=1;i<n;++i){
        if(a[i]>m)
            m=a[i];
    }
    return m;
}
//按照位数进行桶排序
void countSort(int a[],int n,int exp){
    int out[n]={0},buckets[10]={0};
    for(int i=0;i<n;i++)//将当前位数按0-9排序
        buckets[(a[i]/exp)%10]++;
    for(int i=1;i<10;i++)//更改后buckets[i]的值，对应该数据在output[]中的位置
        buckets[i]+=buckets[i-1];
    for(int i=n-1;i>=0;i--){//按位进行排序
        out[buckets[(a[i]/exp)%10]-1]=a[i];
        buckets[(a[i]/exp)%10]--;
    }
    for(int i=0;i<n;i++)
        a[i]=out[i];
}
//基数排序 时间复杂度O(d(r+n))，空间复杂度O(rd+n)，r代表关键字基数，d代表长度，n代表关键字个数
void radixSort(int a[],int n){
    int m=getMax(a,n);
    for(int exp=1;m/exp>0;exp*=10)
        countSort(a,n,exp);
}

/****************************归并排序********************************/
//合并处理
void merge(int a[],int left,int mid,int right){
    int leftN=mid-left+1,rightN=right-mid;
    int leftArray[leftN]={0},rightArray[rightN]={0};
    for(int i=0;i<leftN;i++)
        leftArray[i]=a[left+i];
    for(int i=0;i<rightN;i++)
        rightArray[i]=a[mid+1+i];
    int i=0,j=0,k=left;
    while(i<leftN&&j<rightN){
        if(leftArray[i]<rightArray[j])
            a[k++]=leftArray[i++];
        else
            a[k++]=rightArray[j++];
    }
    while(i<leftN)
        a[k++]=leftArray[i++];
    while (j<rightN)
        a[k++]=rightArray[j++];
}

void mergeSort(int a[],int left,int right){
    if(left<right){
        int mid=left+(right-left)/2;
        mergeSort(a,left,mid);
        mergeSort(a,mid+1,right);
        merge(a,left,mid,right);
    }
}

int main(){
    int a[10]={3,4,7,6,1,2,5,9,8,0};
    //int a[9]={53, 3, 542, 748, 14, 214, 154, 63, 616};
    int n=sizeof(a)/sizeof(a[0]);

    //bubbleSort(a,n);
    //optimizedBubbleSort(a,n);
    //insertionSort(a,n);
    //shellSort(a,n);
    //radixSort(a,n);
    mergeSort(a,0,n-1);

    for(int i=0;i<n;++i)
        cout<<a[i]<<" ";
    return 0;
}
