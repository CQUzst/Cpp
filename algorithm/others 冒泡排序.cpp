//冒泡排序
void bubbleSort(vector<int>& a)
{
    int temp = 0;
    for (int i = 0; i < a.size() - 1; i++) {
        for (int j = 0; j < a.size() - 1 - i; j++) {
            if (a[j] > a[j + 1]) {
                temp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = temp;
            }
        }
    }
}
int main(){
    vector<int> arr{2,6,2,6,4,6,4};
    bubbleSort(arr);
    return 0;
}