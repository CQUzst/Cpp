//希尔排序(不稳定）
//基本思想：算法先将要排序的一组数按某个增量d（n/2,n为要排序数的个数）分成若干组
//每组中记录的下标相差d.对每组中全部元素进行直接插入排序，
//然后再用一个较小的增量（d/2）对它进行分组，在每组中再进行直接插入排序
//当增量减到1时，进行直接插入排序后，排序完成。
//ex:4,8,5,3 gap=2,第一个区域为4，5；第二区域为8，3
int shellSort(vector<int> arr, int n) {
    for (int gap = n/2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i += 1) {//对每个划分区域进行直接插入排序
            int temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap)
                arr[j] = arr[j - gap];
            arr[j] = temp;
        }
    }
    return 0;
}


int main(){
    vector<int> arr{2,6,2,6,4,6,4};
    shellSort(arr,arr.size());
    return 0;
}